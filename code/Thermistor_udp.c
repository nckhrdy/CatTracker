#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include <math.h>
#include "esp_adc_cal.h"

#define WIFI_SSID "Group_1"
#define WIFI_PASS "smartsys"
#define HOST_IP_ADDR "192.168.1.36"
#define PORT_SND 8081
//#define PAYLOAD "HELLO WORLD!"

// THERMISTER/////////////////////////////////////
#define DEFAULT_VREF 1100 // Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES 64  // Multisampling

static esp_adc_cal_characteristics_t *adc_chars;
static const adc_channel_t channel = ADC_CHANNEL_6; // GPIO34 if ADC1, GPIO14 if ADC2
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1;

static void check_efuse(void)
{
  // Check TP is burned into eFuse
  if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK)
  {
    printf("eFuse Two Point: Supported\n");
  }
  else
  {
    printf("eFuse Two Point: NOT supported\n");
  }

  // Check Vref is burned into eFuse
  if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK)
  {
    printf("eFuse Vref: Supported\n");
  }
  else
  {
    printf("eFuse Vref: NOT supported\n");
  }
}

static void print_char_val_type(esp_adc_cal_value_t val_type)
{
  if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP)
  {
    printf("Characterized using Two Point Value\n");
  }
  else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF)
  {
    printf("Characterized using eFuse Vref\n");
  }
  else
  {
    printf("Characterized using Default Vref\n");
  }
}

float convert_to_cel(int reading)
{
  const float R1 = 5000.0; // value of resistor
  const float B = 3950.0;  // value of B (+/- 1 percent)
  const float T0 = 276.0;  // room temp in Kelvin
  float Rt = R1 / (4095.0 / (float)reading) - 1.0;
  float celsius = (1.0 / ((log(Rt / R1)) / B + (1.0 / T0))) - 273.15;
  return celsius;
}

////////////////////////////////////////////////////////////////////////////////////////

static const char *TAG = "udp_client";

void wifi_event_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{
  if (event_id == WIFI_EVENT_STA_START)
  {
    esp_wifi_connect();
  }
  else if (event_id == WIFI_EVENT_STA_CONNECTED)
  {
    ESP_LOGI(TAG, "Connected to AP");
  }
  else if (event_id == WIFI_EVENT_STA_DISCONNECTED)
  {
    esp_wifi_connect();
    ESP_LOGI(TAG, "Disconnected from AP");
  }
}

void ip_event_handler(void *arg, esp_event_base_t event_base,
                      int32_t event_id, void *event_data)
{
  if (event_id == IP_EVENT_STA_GOT_IP)
  {
    ESP_LOGI(TAG, "Got IP address");
  }
}

void app_main()
{
 ////////Thermister//////
  // Check if Two Point or Vref are burned into eFuse
  check_efuse();
  // Configure ADC
  if (unit == ADC_UNIT_1)
  {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(channel, atten);
  }
  else
  {
    adc2_config_channel_atten((adc2_channel_t)channel, atten);
  }

  // Characterize ADC
  adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
  print_char_val_type(val_type);

/////////////////////thermister/////////


  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT,
                                             ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT,
                                             IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  wifi_config_t wifi_config = {
      .sta = {
          .ssid = WIFI_SSID,
          .password = WIFI_PASS,
      },
  };
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());


  while (1)
  {

    /////////////////////thermister/////////
    uint32_t adc_reading = 0;

    // Multisampling
    for (int i = 0; i < NO_OF_SAMPLES; i++)
    {
      if (unit == ADC_UNIT_1)
      {
        adc_reading += adc1_get_raw((adc1_channel_t)channel);
      }
      else
      {
        int raw;
        adc2_get_raw((adc2_channel_t)channel, ADC_WIDTH_BIT_12, &raw);
        adc_reading += raw;
      }
    }

    adc_reading /= NO_OF_SAMPLES;
    // Convert adc_reading to voltage in mV
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
    float Celsius = convert_to_cel(voltage);
    printf("Raw: %ld\tVoltage: %ldmV\tTemperature: %.2f°C\n", adc_reading, voltage, Celsius);
    char cel_str[20];
    sprintf(cel_str, "%f", Celsius);

    vTaskDelay(pdMS_TO_TICKS(1000));

    /////////////////////thermister/////////

    struct sockaddr_in destAddr;
    destAddr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR); // Destination IP
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(PORT_SND); // Destination Port
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0)
    {
      ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
      break;
    }
    ESP_LOGI(TAG, "Socket created");
    int err = sendto(sock, cel_str, strlen(cel_str), 0,
                     (struct sockaddr *)&destAddr, sizeof(destAddr));
    if (err < 0)
    {
      ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
    }
    else
    {
      ESP_LOGI(TAG, "Message Sent");
    }
    close(sock);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
