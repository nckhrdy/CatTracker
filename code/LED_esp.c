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

#define WIFI_SSID "Group_1"
#define WIFI_PASS "smartsys"
#define HOST_IP_ADDR "192.168.1.36"
#define PORT_RCV 8081
#define LED_PIN 12

static const char *TAG = "udp_server";

void wifi_event_handler(void* arg, esp_event_base_t event_base,
                        int32_t event_id, void* event_data) {
    if (event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_id == WIFI_EVENT_STA_CONNECTED) {
        ESP_LOGI(TAG, "Connected to AP");
    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        ESP_LOGI(TAG, "Disconnected from AP");
    }
}

void ip_event_handler(void* arg, esp_event_base_t event_base,
                      int32_t event_id, void* event_data) {
    if (event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "Got IP address");
    }
}

void app_main() {
    int counter = 0;
    esp_rom_gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
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
    while (1) {
        struct sockaddr_in srcAddr;
        socklen_t socklen = sizeof(srcAddr);
        char buffer[128];
        int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created");
        srcAddr.sin_family = AF_INET;
        srcAddr.sin_port = htons(PORT_RCV);
        srcAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        int err = bind(sock, (struct sockaddr *)&srcAddr, sizeof(srcAddr));
        if (err < 0) {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket bound, waiting for data");
        while (1) {
            int len = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&srcAddr, &socklen);
            if (len < 0) {
                ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
            } else {
                buffer[len] = 0; // Null-terminate whatever we received and treat like a string
                ESP_LOGI(TAG, "Received %d bytes from %s:", len, inet_ntoa(srcAddr.sin_addr));
                ESP_LOGI(TAG, "%s", buffer);
                if (strcmp(buffer, "toggle_led") == 0) {
                    counter = counter + 1;
                  int countermod = counter % 2;
                    gpio_set_level(LED_PIN, countermod); // Toggle the LED
                }
            }
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        close(sock);
    }
}


