#include "telnetMessenger.h"

// Used to debug the library. Uncomment to enable serial debugging messages.
//#define SERIAL_DEBUG

// Connect to a WiFi network and setup a telnet connection with the provided info.
TelnetMessenger::TelnetMessenger(char* ssid, char* pass, uint16_t localPort, uint16_t remotePort, IPAddress remoteIP) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  this->telConn(localPort, remotePort, remoteIP);
}

// Just setup a telnet connection, assuming we are already connected to a WiFi network. 
TelnetMessenger::TelnetMessenger(uint16_t localPort, uint16_t remotePort, IPAddress remoteIP) {
  this->telConn(localPort, remotePort, remoteIP); 
}


// Send the string to the remote connection.
void TelnetMessenger::sendMessage(char* msg) {

  #ifdef SERIAL_DEBUG
    Serial.println("Sending message:");
    Serial.println(this->remoteIP);
    Serial.println(this->remotePort);
    Serial.println(msg);
  #endif
  
  this->Udp.beginPacket(this->remoteIP, this->remotePort);
  this->Udp.write("\r\n> ");
  this->Udp.write(msg);
  int code = this->Udp.endPacket();

  delay(1000);

  #ifdef SERIAL_DEBUG
    Serial.print("Message sending complete, return code: ");
    Serial.println(code);
  #endif
}

void TelnetMessenger::disconnect() {
  this->Udp.stop();
  WiFi.disconnect();
  while (WiFi.status() != WL_DISCONNECTED) {
    delay(500);
  }
}

// Setup a telnet connection with the provided info.
void TelnetMessenger::telConn(uint16_t localPort, uint16_t remotePort, IPAddress remoteIP) {

  #ifdef SERIAL_DEBUG
    Serial.println("Connecting to telnet...");
  #endif
    
  this->localPort = localPort;
  this->remotePort = remotePort;
  this->remoteIP = remoteIP;

  this->Udp.begin(this->localPort);
}
