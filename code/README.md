# Code Readme

### Node.js
Our JavaScript code imports required modules (http, dgram, csv-parse/lib/sync, and csv-writer), define constants for the UDP server's IP address and port, creates the UDP server and an empty string messageFromEsp to store incoming messages, create a flag buttonClicked to track the state of the button on the web page, defines a message message1 to be sent to the client ESP32 device to toggle the LED

For the server ESP32, the .js file creates an HTTP server that listens for incoming requests: 
-- If messageFromEsp is not empty: the server will, parse the messages and log them, display a live camera stream and temperature data on an HTML page, create a button to toggle the LED on the ESP32 device, which sends the toggle_led message via UDP, and save the received data to a CSV file.
-- If messageFromEsp is empty: the server will display an HTML page with a message indicating that no data is received from the ESP32 device.

It also defines event listeners for the UDP server, handling received messages and errors, binds the UDP server to the specified IP address and port. The web server listens on port 8081 and serves an HTML page displaying the camera stream and temperature data (http://nckhrdypi.ddns.net:8081/). Users can toggle the LED on the ESP32 device by clicking the "Light On/Off" button on the web page. The server logs temperature data to a CSV file as it receives messages from the ESP32 device.

### ESP32 code for thermister
It reads the temperature from a thermistor, connects to a Wi-Fi network, and sends the temperature data to a remote server over UDP.

Included necessary headers and define constants: Headers for Wi-Fi, ADC, GPIO, FreeRTOS, and other necessary components are included. Constants such as Wi-Fi SSID, password, and the IP address of the remote server are defined.

Functions are provided for checking the eFuse for calibration values, printing the type of ADC calibration used, and converting the ADC reading to a Celsius temperature using a thermistor. These functions were obtained from the previous thermister skill. 

Functions are provided to handle Wi-Fi events, such as connecting to an access point and receiving an IP address.

The main function executed when the code runs. It initializes the ADC and calibrates it for thermistor reading, initializes and configures the Wi-Fi, and connects to the Wi-Fi network. Inside the while(1) loop, the code reads the temperature from the thermistor, converts the reading to Celsius, and sends it over UDP to the remote server. The loop has a delay of 1 second between each iteration to avoid overloading the server with too many messages.

In general this code reads temperature values from a thermistor connected to an ESP32 microcontroller, connects to a Wi-Fi network, and sends the temperature data over UDP to a specified IP address and port.

### ESP32 client code for LED  
The ESP32 application creates a Wi-Fi connection and sets up both a UDP client and a UDP server. It defines Wi-Fi SSID and password, host IP address, ports for sending and receiving, and the LED pin number. It sets up the wifi_event_handler and ip_event_handler to handle Wi-Fi and IP events, such as connecting to the Wi-Fi access point, getting an IP address, and handling disconnections.

In the app_main function it initializes the LED pin as an output and the NVS (non-volatile storage), networking, and event loop. The app_main also creates and configures the Wi-Fi station, registers the event handlers, and starts the Wi-Fi connection. The UDP client is set up in a while loop that creates a UDP socket, sets up the destination address.

The main point is that it listens for incoming messages and prints the message content and sender's IP address, which is how we were able to realize that the server and client are communicating. If the received message is "toggle_led", it toggles the state of the LED connected to the specified LED pin, and waits for 100 milliseconds before checking for another message.
