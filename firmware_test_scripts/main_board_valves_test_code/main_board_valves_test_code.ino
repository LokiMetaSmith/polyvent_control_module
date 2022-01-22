#include "SPI.h"
const int CS = 14; // 15

uint16_t transmission[13] = {200, 0xFFFF, 0xFFFF, 0xFFFF, 15000, 0, 0, 0, 0, 0, 0, 0, 0};


void setup() {
   Serial.begin(9600);
   pinMode(CS, OUTPUT);
   digitalWrite(CS, HIGH);
   SPI.begin();
  SPI.beginTransaction(SPISettings(10000, MSBFIRST, SPI_MODE0));
}


 
void loop() {
  Serial.println("looping");
   
   digitalWrite(CS, LOW); // enable Slave Select
  for (int x = 0; x < 13; x++)
  {
    send_uint16_t(transmission[x]);
  }
   digitalWrite(CS, HIGH); // disable Slave Select
   delay(1000);

   digitalWrite(CS, LOW); // enable Slave Select
   send_uint16_t(200);
   for (int x = 0; x < 12; x++)
   {
     send_uint16_t(0x0000);
   }
   digitalWrite(CS, HIGH); // disable Slave Select
   delay(1000);
}


void send_uint16_t(uint16_t u)
{
  // Robert Reed's code
  SPI.transfer(*(((byte *)&u) + 0));
  byte b1 = *(((byte *)&u) + 0);

  SPI.transfer(*(((byte *)&u) + 1));
  byte b2 = *(((byte *)&u) + 1);
}
