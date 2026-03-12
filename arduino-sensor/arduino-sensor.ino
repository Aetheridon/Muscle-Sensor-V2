#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

WiFiUDP udp;
unsigned int port = 8888;

IPAddress clientIP;
unsigned int clientPort;
bool streaming = false;

void check_wifi_status() {
  // Visual indicator since board is being powered via external batt
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // configure onboard LED to take voltage

  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    delay(500);
  }

  udp.begin(port);
}

void loop() {
  check_wifi_status();

  int packetSize = udp.parsePacket();
  char packetBuffer[255];
  int len = 0;

  if (packetSize) { // packetSize = 0 if no available packets
    len = udp.read(packetBuffer, 255);
  }

  if (len > 0) { // make sure packet wasn't empty
    packetBuffer[len] = '\0'; // null terminates buffer

    if (strcmp(packetBuffer, "ONLINE") == 0) {
      clientIP = udp.remoteIP();
      clientPort = udp.remotePort();

      udp.beginPacket(clientIP, clientPort);
      udp.write((const uint8_t*)"ACK", 3);
      udp.endPacket();
    }

    if (strcmp(packetBuffer, "START") == 0) {
      clientIP = udp.remoteIP();
      clientPort = udp.remotePort();
      streaming = true;
    }

    else if (strcmp(packetBuffer, "STOP") == 0) {
      if (udp.remoteIP() == clientIP && udp.remotePort() == clientPort) { // Ensures we only disconnect if the packet is from the client
        streaming = false;
      }
    }
  }

  if (streaming) {
    int voltages[2];

    voltages[0] = analogRead(A0);
    voltages[1] = analogRead(A1);
        
    udp.beginPacket(clientIP, clientPort);
    udp.write((uint8_t*)voltages, sizeof(voltages));
    udp.endPacket();
  }
}