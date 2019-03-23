/* FluentLogger Sample code
 * Copyright (c) 2018 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
#include "FluentLogger.h"
#include "TCPSocket.h"

// Default network interface object. Don't forget to change the WiFi SSID/password in mbed_app.json if you're using WiFi.
NetworkInterface *net = NetworkInterface::get_default_instance();

FluentLogger logger(net,"192.168.1.101",24224);  // please set your Fluentd server

int main() 
{
    // Connect to the internet (DHCP is expected to be on)
    printf("Connecting to the network using the default network interface...\n");
    net = NetworkInterface::get_default_instance();

    nsapi_error_t net_status = -1;
    for (int tries = 0; tries < 3; tries++) {
        net_status = net->connect();
        if (net_status == NSAPI_ERROR_OK) {
            break;
        } else {
            printf("Unable to connect to network. Retrying...\n");
        }
    }

    if (net_status != NSAPI_ERROR_OK) {
        printf("ERROR: Connecting to the network failed (%d)!\n", net_status);
        return -1;
    }

    printf("Connected to the network successfully. IP address: %s\n", net->get_ip_address());
    printf("MAC: %s\n", net->get_mac_address());
    printf("IP: %s\n", net->get_ip_address());
    printf("Netmask: %s\n", net->get_netmask());
    printf("Gateway: %s\n", net->get_gateway());
    // printf("RSSI: %d\n\n", net->get_rssi());

    uMP mp(128);     //Message body

    //array sample with primitive functions
    //YYYY-MM-DD HH:MM:SS zzzzz debug.test: ["sint",0,1,-1,-128,-32768,-2147483648]
    mp.init();                    // need init every time
    mp.start_array(7);            // declare array for seven elements. it can mixed type.
    mp.set_str("sint", 4);        // 1st element
    mp.set_sint(0);               // 2nd element
    mp.set_sint(1);               // 3rd element
    mp.set_sint(-1);              // 4th element
    mp.set_sint(-128);            // 5th element
    mp.set_sint(-32768);          // 6th element
    mp.set_sint(-2147483648);     // 7th element
    logger.log("debug.test", mp); // emit

    //array sample with primitive functions
    //YYYY-MM-DD HH:MM:SS zzzzz debug.test: ["uint",0,1,128,255,65535,4294967295]
    mp.init();                    // need init every time
    mp.start_array(7);            // declare array for seven elements. it can mixed type.
    mp.set_str("uint", 4);        // 1st element
    mp.set_uint(0);               // 2nd element
    mp.set_uint(1);               // 3rd element
    mp.set_uint(128);             // 4th element
    mp.set_uint(0xff);            // 5th element
    mp.set_uint(0xffff);          // 6th element
    mp.set_uint(0xffffffff);      // 7th element
    logger.log("debug.test", mp);

    //map sample with primitive functions
    //YYYY-MM-DD HH:MM:SS zzzzz debug.test: {"string":"Hi!","float":0.3333333432674408,"double":0.3333333333333333}
    mp.init();                         // need init every time
    mp.start_map(3);                   // declare map for three paires.
    mp.set_str("string", 6);           // 1st key 
    mp.set_str("Hi!", 3);              // 1st value
    mp.set_str("float", 5);            // 2nd key 
    mp.set_float(1.0/3);               // 2nd value
    mp.set_str("double", 6);           // 3rd key
    mp.set_double(1.0/3);              // 3rd value
    logger.log("debug.test", mp);      // emit

    //map sample with simple method. same as prev sample (but need more CPU cycle)
    //YYYY-MM-DD HH:MM:SS zzzzz debug.test: {"string":"Hi!","float":0.3333333432674408,"double":0.3333333333333333}
    mp.init();                         // need init every time
    mp.start_map(3);                   // declare map for two paires.
    mp.map("string", "Hi!");           // 1st key/value pair
    mp.map("float", (float)(1.0/3));   // 2nd key/value pair
    mp.map("double", (double)(1.0/3)); // 3rd key/value pair
    logger.log("debug.test", mp);      // emit

    while(1) {
        // Infinite loop sending data to debug and treasure data
        printf("\r\n --- Infinite Loop\r\n");
        logger.log("td.fluentd_database.test",mp);
        logger.log("debug.test",mp);
        printf("\r\n Data sent, waiting 10s...");
        wait_ms(10000);
    }

    net->disconnect();  
}
