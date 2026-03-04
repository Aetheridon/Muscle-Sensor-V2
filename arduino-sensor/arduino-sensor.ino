#include <SPI.h>
#include <WiFi.h>
#include <WifiUdp.h>

#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

WiFiUDP udp;
unsigned int port = 8888;

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

  if (packetSize) { // packetSize = 0 if no available packets
    char packetBuffer[255];
    int len = udp.read(packetBuffer, 255);

    if (len > 0) {
      packetBuffer[len] = '\0'; // null terminates buffer
    }

    if (strcmp(packetBuffer, "START") == 0) {
      IPAddress clientIP = udp.remoteIP();
      unsigned int clientPort = udp.remotePort();

      for (int i = 0; i < 500; i++) {
        udp.beginPacket(clientIP, clientPort);
        udp.write((uint8_t*)&i, sizeof(i));
        udp.endPacket();
      }
    }
  }
}
