#include "SPI.h"
const int CS = 14; // 15

uint16_t transmission[13] = {200, 0, 0, 0, 0xFFFF, 0, 0, 0xFFFF, 52428, 0xFFFF, 0, 0xFFFF, 0};


void setup() {
  Serial.begin(9600);
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);
  SPI.begin();
  SPI.beginTransaction(SPISettings(10000, MSBFIRST, SPI_MODE0));

  delay(3000);

  digitalWrite(CS, LOW); // enable Slave Select
  for (int x = 0; x < 13; x++)
  {
    send_uint16_t(transmission[x]);
  }
  digitalWrite(CS, HIGH); // disable Slave Select
}



void loop() {
}


void send_uint16_t(uint16_t u)
{
  // Robert Reed's code
  SPI.transfer(*(((byte *)&u) + 0));
  byte b1 = *(((byte *)&u) + 0);

  SPI.transfer(*(((byte *)&u) + 1));
  byte b2 = *(((byte *)&u) + 1);
}
