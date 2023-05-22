# Cat Tracker

Authors: Marybel Boujaoude, Hassan Hijazi, Nicholas Hardy, Riya Deokar

Date: 2023-03-21

### Summary
The Cat Tracker System is designed to monitor and interact with cats at home when their owners are away. In this quest, we created a cat tracker that implemented a thermistor to sense if a cat was in its bed through monitoring temperature, an LED that blinks to entertain the cat as a toy, and a Raspberry Pi camera to remotely watch the cat and make sure they are eating.

Data is sent wirelessly and can be accessed remotely through a web portal. The system is developed in stages, starting with testing individual components (Router, Raspberry PI, DYNDNS, PI Web Cam, WiFi-ESP, Transferring Data Using ESP), configuring wireless connectivity, integrating control of the LED with our knowledge of ESP-32 and GPIO pins as well as client-server connection, and designing the web portal. 

The ESP is sending sensor data to the node server which is accesible using DDNS (http://nckhrdypi.ddns.net:8081/). We made sure to include each requirement on the same server, in order to simulate a home page that displays real-time data for the user. When a user accesses the server they will see the tab titled as Cat Cam, they will see a web page displaying the (Cat Camera Stream), a line chart with temperature readings (Bed Temperature), and a button to toggle the LED on and off (Light On/Off). 

The main file is the JavaScript code which creates a web server and a UDP server using the Node.js runtime. It listens for incoming UDP messages from an ESP32 device or in the case of the LED sends messages, displays a live camera stream, and logs the temperature data. It also allows users to toggle an LED on the ESP32 device. We speak more on the logistics of this file and the files it is communnicating with, in the code README. 

### Supporting Artifacts
- Link to video technical presentation: https://drive.google.com/file/d/12eR8UgpHKT6jM2_554wGzWqDpfIph-i9/view?usp=sharing
- Link to video demo: https://drive.google.com/file/d/1ePfheROx0aEy6aM1t0R8ZFqLJavN3SvL/view?usp=sharing

### Self-Assessment 

| Objective Criterion | Rating | Max Value  | 
|---------------------------------------------|:-----------:|:---------:|
| Objective One | 1 |  1     | 
| Objective Two | 1 |  1     | 
| Objective Three | 1 |  1     | 
| Objective Four | 1 |  1     | 
| Objective Five | 1 |  1     | 
| Objective Six | 1 |  1     | 
| Objective Seven | 1 |  1     | 


### Solution Design

In order to create this cat tracker, we used a two ESP 32's, an LED, a thermister, and a Raspberry PI with a V2 Camera. There are three units to consider: the bed unit, toy unit, and food unit. To demonstrate our units: we created a UDP server and setup an HTTP server that sends temperature readings, camera stream, and a button on the server which toggles a LED on and off. We rendered a line chart using Chart.js to display the constantly updating temperature readings and associated times, to quantify sleeping habits. We used port forward in order to stream the Raspberry PI camera content and be able to remotely monitor the cat. The server listens for incoming messages from the client ESP32 thermister sensor and updates the web interface with accurate temperature readings accordingly. While for the LED toggle, the server ESP32 is listening for a message from the Node.js client. Our final solution controlled the LED with remote control access, displayed real-time data, streamed video, and maintained wireless connectivity with ESPs and RPIis. 


### Sketches/Diagrams
![Untitled (Draft)](https://user-images.githubusercontent.com/47408944/226778419-2b3fc474-b882-459a-ae11-dd482e9d716d.jpeg)

<img width="615" alt="Screen Shot 2023-03-21 at 9 30 23 PM" src="https://user-images.githubusercontent.com/47408944/226778562-4ecacd10-c57e-4b8c-b929-cb0f00450609.png">


### Modules, Tools, Source Used Including Attribution
HUZZAH32 ESP32 Feather Board, Raspberry Pi, Raspberry Pi Camera V2, Motion, Node.js, E1200 V2 Router, thermistor, LED

### References
WiFi channel allocation: https://github.com/BU-EC444/01-EBook/blob/main/docs/utilities/docs/wifi-channels.md

Router Configuration: http://freshtomato.org/

Raspberry Pi: https://www.raspberrypi.com/documentation/computers/remote-access.html

DDNS: https://www.noip.com/

Raspberry Pi Camera: https://pimylifeup.com/raspberry-pi-webcam-server/
