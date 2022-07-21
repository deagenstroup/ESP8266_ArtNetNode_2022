/*
ESP8266_ArtNetNode v2.0.0
Copyright (c) 2016, Matthew Tong
https://github.com/mtongnz/ESP8266_ArtNetNode_v2

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.
If not, see http://www.gnu.org/licenses/
*/

extern TelnetMessenger *msgr;

// Configuration file which defines preproccessor variables for default device info.
// Comment out to use defaults defined below.
#include "device_info.h"

// Setting default device info values, if not defined already.
// All default settings below can be customized by defining them in the
// device_info.h file. (So as to not register changes in the git repo by
// modifying this file directly)

// If true, the device starts in hotspot mode by default.
#ifndef STAND_ALONE
  #define STAND_ALONE false
#endif

// The hostname of the device.
#ifndef NODE_NAME
  #define NODE_NAME "espArtNetNode"
#endif
#ifndef LONG_NODE_NAME
  #define LONG_NODE_NAME "espArtNetNode by Matthew Tong"
#endif

// Name of the WiFi AP to connect to when not in hotspot mode.
#ifndef WIFI_SSID
  #define WIFI_SSID ""
#endif
// Password of the WiFi AP to connect to when not in hotspot mode.
#ifndef WIFI_PASS
  #define WIFI_PASS ""
#endif

// Name of the hotspot created when in hotspot mode.
#ifndef HOTSPOT_SSID
  #define HOTSPOT_SSID "espArtNetNode"
#endif
// Password of the hotspot created when in hotspot mode.
#ifndef HOTSPOT_PASS
  #define HOTSPOT_PASS "byMtongnz2017"
#endif

// IP address of the node when connected to a WiFi AP.
#ifndef IP_ADDR_A
  #define IP_ADDR_A 10
  #define IP_ADDR_B 10
  #define IP_ADDR_C 0
  #define IP_ADDR_D 3
#endif

// Subnet mask of the node when connected to a WiFi AP.
#ifndef SUB_A
  #define SUB_A 255
  #define SUB_B 255
  #define SUB_C 255
  #define SUB_D 0
#endif

// Default gateway (WiFi AP's IP address usually) when connected to a WiFi AP.
#ifndef GATE_A
  #define GATE_A 10
  #define GATE_B 10
  #define GATE_C 0
  #define GATE_D 1
#endif

// Broadcast address when connected to a WiFi AP.
#ifndef BROAD_A
  #define BROAD_A 10
  #define BROAD_B 10
  #define BROAD_C 0
  #define BROAD_D 255
#endif

// Change this if the settings structure changes
#define CONFIG_VERSION "a5W"

// Dont change this
#define CONFIG_START 0

enum fx_mode {
  FX_MODE_PIXEL_MAP = 0,
  FX_MODE_12 = 1
};

enum p_type {
  TYPE_DMX_OUT = 0,
  TYPE_RDM_OUT = 1,
  TYPE_DMX_IN = 2,
  TYPE_WS2812 = 3
};

enum p_protocol {
  PROT_ARTNET = 0,
  PROT_ARTNET_SACN = 1
};

enum p_merge {
  MERGE_LTP = 0,
  MERGE_HTP = 1
};

struct StoreStruct {
  // StoreStruct version
//  char version[4];

  // Device settings:
  IPAddress ip, subnet, gateway, broadcast, hotspotIp, hotspotSubnet, hotspotBroadcast, dmxInBroadcast;
  bool dhcpEnable, standAloneEnable;
  char nodeName[18], longName[64], wifiSSID[40], wifiPass[40], hotspotSSID[20], hotspotPass[20];
  uint16_t hotspotDelay;
  uint8_t portAmode, portBmode, portAprot, portBprot, portAmerge, portBmerge;
  uint8_t portAnet, portAsub, portAuni[4], portBnet, portBsub, portBuni[4], portAsACNuni[4], portBsACNuni[4];
  uint16_t portAnumPix, portBnumPix, portApixConfig, portBpixConfig;
  bool doFirmwareUpdate;
  uint8_t portApixMode, portBpixMode;
  uint16_t portApixFXstart, portBpixFXstart;
  uint8_t resetCounter, wdtCounter;
  
} deviceSettings = {
//  CONFIG_VERSION,
  
  // The default values

  // Networking info when connected to a WiFi AP.
  IPAddress(IP_ADDR_A, IP_ADDR_B, IP_ADDR_C, IP_ADDR_D), IPAddress(SUB_A, SUB_B, SUB_C, SUB_D), IPAddress(GATE_A, GATE_B, GATE_C, GATE_D), IPAddress(BROAD_A, BROAD_B, BROAD_C, BROAD_D), 
  
  IPAddress(2,0,0,1), IPAddress(255,0,0,0), IPAddress(2,255,255,255), IPAddress(10,10,0,255),
  true, STAND_ALONE,
  NODE_NAME, LONG_NODE_NAME, WIFI_SSID, WIFI_PASS, HOTSPOT_SSID, HOTSPOT_PASS,
  15,
  TYPE_WS2812, TYPE_WS2812, PROT_ARTNET, PROT_ARTNET, MERGE_HTP, MERGE_HTP,
  0, 0, {1, 0, 0, 0}, 0, 0, {1, 0, 0, 0}, {1, 2, 3, 4}, {5, 6, 7, 8},
  36, 36, 0, 0,
  false,
  FX_MODE_PIXEL_MAP, FX_MODE_PIXEL_MAP,
  1, 1,
  0, 0
};

void eepromSave() {
  for (uint16_t t = 0; t < sizeof(deviceSettings); t++)
    EEPROM.write(CONFIG_START + t, *((uint8_t*)&deviceSettings + t));
  
  EEPROM.commit();

  #ifdef TEL_PORT
    if (msgr != NULL)
      msgr->sendMessage("Configuration saved to EEPROM.");
  #endif
}

void eepromLoad() {

    // Store defaults for if we need them
    StoreStruct tmpStore;
    tmpStore = deviceSettings;
    
    // Copy data to deviceSettings structure
    for (uint16_t t = 0; t < sizeof(deviceSettings); t++)
      *((uint8_t*)&deviceSettings + t) = EEPROM.read(CONFIG_START + t);
    
    // If we want to restore all our settings
    if (deviceSettings.resetCounter >= 5 || deviceSettings.wdtCounter >= 10) {
      deviceSettings.wdtCounter = 0;
      deviceSettings.resetCounter = 0;

      // Store defaults back into main settings
      deviceSettings = tmpStore;
    }

    #ifdef TEL_PORT
      if (msgr != NULL)
        msgr->sendMessage("Configuration loaded from EEPROM.");
    #endif

}
