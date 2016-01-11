/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "mbed.h"
#include "cc3000.h"
#include "main.h"
#include "TCPSocketConnection.h"
#include "TCPSocketServer.h"
#include "MODSERIAL.h"
using namespace mbed_cc3000;

/* cc3000 module declaration specific for user's board. Check also init() */
#if (MY_BOARD == WIGO)
cc3000 wifi(PTA16, PTA13, PTD0, SPI(PTD2, PTD3, PTC5), "ssid", "key", WPA2, false);
Serial pc(USBTX, USBRX);
#elif (MY_BOARD == WIFI_DIPCORTEX)
cc3000 wifi(p28, p27, p30, SPI(p21, p14, p37), "ssid", "key", WPA2, false);
Serial pc(UART_TX, UART_RX);
#elif (MY_BOARD == MBED_BOARD_EXAMPLE)
cc3000 wifi(p9, p10, p8, SPI(p5, p6, p7), "CloneBox", "iamthecos0", WPA2, false);
Serial pc(USBTX, USBRX);
#else

#endif
Timeout to1;
TCPSocketConnection socket_computer;
TCPSocketConnection socket_driver;
MODSERIAL uart(p28, p27,512, 1024);

/**
 *  \brief TCP client demo
 *  \param none
 *  \return int
 */
void toComputer(char mess[]){
        printf("sending\n");
        socket_computer.send_all(mess, sizeof(mess)-1);
    }

void toDriver(char mess[]){
        socket_driver.send_all(mess, sizeof(mess) - 1);
    }
    
void send()
     {
         char message[125];
        
        uart.scanf("%s", message);
        message[strlen(message)] = '\n';
        pc.printf("%s", message);
        
         wait(0.1);
         //toDriver(message);
         wait(0.25);
         toComputer(message);
         wait(0.1);
     }

int main() {

    init();
    pc.baud(115200);
    
    
    printf("cc3000 tcp client demo. \r\n");
    wifi.init();
    if (wifi.connect() == -1) {
        printf("Failed to connect. Please verify connection details and try again. \r\n");
    } else {
        printf("IP address: %s \r\n", wifi.getIPAddress());
    }
    printf("wifi connected\n");
    
     
    //const char* ECHO_SERVER_ADDRESS_DRIVER = "192.168.0.13"; //tablet
    const char* ECHO_SERVER_ADDRESS_DRIVER = "192.168.0.12"; //phone
    const int ECHO_SERVER_PORT_DRIVER = 5000;
    
    const char* ECHO_SERVER_ADDRESS_COMPUTER = "173.255.204.192";
    const int ECHO_SERVER_PORT_COMPUTER = 38236;
    
    while (socket_computer.connect(ECHO_SERVER_ADDRESS_COMPUTER, ECHO_SERVER_PORT_COMPUTER) < 0) {
         printf("Unable to connect to (%s) on port (%d) \r\n", ECHO_SERVER_ADDRESS_COMPUTER, ECHO_SERVER_PORT_COMPUTER);
         wait(1);
     }
     printf("computer connected\n");
    
     while (socket_driver.connect(ECHO_SERVER_ADDRESS_DRIVER, ECHO_SERVER_PORT_DRIVER) < 0) {
         printf("Unable to connect to (%s) on port (%d) \r\n", ECHO_SERVER_ADDRESS_DRIVER, ECHO_SERVER_PORT_DRIVER);
         wait(1);
     }
     printf("driver connected\n");
     uart.baud(9600);
     Ticker func;
     
     while(1)
     {
        uart.txBufferFlush(); 
        uart.rxBufferFlush();
        func.attach(&send, 2.0);
     }
} 
