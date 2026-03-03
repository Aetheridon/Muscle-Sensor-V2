#include <SPI.h>
#include <WiFi.h>

#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // configure onboard LED to take voltage

  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    delay(500);
  }
}

void loop() {
  // Visual indicator since board is being powered via external batt
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}
