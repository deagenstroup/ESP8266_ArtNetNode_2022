/**
 * telnetMessenger: A ESP8266 library for sending basic string messages over a telnet
 *                  connection to a remote computer.
 *                  
 * - Run netcat in UDP and listening mode on the specified port to receive debugging info.
 * ex: nc -u -l 8464
 */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
//#include <IPAdress.h>
#include <string.h>

class TelnetMessenger {

  public:

    // Connect to a WiFi network and setup a telnet connection with the provided info.
    TelnetMessenger(char* ssid, char* pass, uint16_t localPort, uint16_t remotePort, IPAddress remoteIP);
   
    // Just setup a telnet connection, assuming we are already connected to a WiFi network. 
    TelnetMessenger(uint16_t localPort, uint16_t remotePort, IPAddress remoteIP);

    // Send the string to the remote connection.
    void sendMessage(char* msg);

    // End the connection.
    void disconnect();

  private:

    // The connection which facilitates telnet communication.
    WiFiUDP Udp;

    // Ports on which telnet runs for both the ESP chip and the remote computer.
    uint16_t localPort, remotePort;

    // IP Address of the remote computer which receives the messages.
    IPAddress remoteIP;

    // Setup a telnet connection with the provided info.
    void telConn(uint16_t localPort, uint16_t remotePort, IPAddress remoteIP);
  
};
