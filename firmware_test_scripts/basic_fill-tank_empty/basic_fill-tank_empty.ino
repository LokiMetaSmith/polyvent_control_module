#include "SPI.h"
#include <HoneywellTruStabilitySPI.h>

const int CS = 14; // 15
const int ESP_CS_PINS[11] = {4, 5, 13, 14, 15, 16, 17, 25, 26, 32, 33};



uint16_t solenoid_empty_tank[13] = {500, 0, 0, 0, 45000, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t solenoid_fill[13] = {500, 65535, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t all_off[13] = {500, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void setup() {
  
  Serial.begin(115200);
  SPI.begin();

  for (int x = 0; x < sizeof(ESP_CS_PINS) / sizeof(ESP_CS_PINS[0]); x++) {
    pinMode(ESP_CS_PINS[x], OUTPUT);
    digitalWrite(ESP_CS_PINS[x], HIGH);
  }
  delay(2000);

    digitalWrite(14, LOW);
  for (int x = 0; x < 13; x++)
    send_uint16_t(all_off[x]);
  digitalWrite(14, HIGH);

  Serial.println("command sent");

  
}

void loop() {
  digitalWrite(14, LOW);
  for (int x = 0; x < 13; x++)
    send_uint16_t(solenoid_fill[x]);
  digitalWrite(14, HIGH);

  delay(3000);

  digitalWrite(14, LOW);
  for (int x = 0; x < 13; x++)
    send_uint16_t(solenoid_empty_tank[x]);
  digitalWrite(14, HIGH);
  
  delay(1000);


  
}






void send_uint16_t(uint16_t u)
{
  // Robert Read's code
  SPI.transfer(*(((byte *)&u) + 0));
  byte b1 = *(((byte *)&u) + 0);

  SPI.transfer(*(((byte *)&u) + 1));
  byte b2 = *(((byte *)&u) + 1);
}
