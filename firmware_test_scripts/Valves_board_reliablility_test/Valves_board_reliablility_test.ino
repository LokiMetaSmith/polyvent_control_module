#include "SPI.h"
const int CS = 14;
//const int ESP_CS_PINS[13] = {4, 5, 13, 14, 15, 16, 17, 21, 22, 25, 26, 32, 33};


uint16_t transmission[13] = {200, 0, 0, 0, 0, 0, 0, 0xFFFF, 0, 0, 0, 0, 0};
uint16_t transmission_2[13] = {500, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t transmission_3[13] = {100, 0, 0, 0, 0, 0, 0, 0, 0xFFFF, 0, 0, 0, 0};


volatile long val1 = 0, val2 = 0, val3 = 0, lastVal1 = 0, lastVal2 = 0, lastVal3 = 0;
long start_time;
long failcount = 0, iteration_count = 0;
int time_of_edge = 0;

void setup() {
  Serial.begin(115200);
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);
  SPI.begin();
  SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));

  pinMode(26, INPUT_PULLUP);
  pinMode(32, INPUT_PULLUP);
  pinMode(33, INPUT_PULLUP);

  //detect when each channel changes phase
  attachInterrupt(26, change1, CHANGE);
  attachInterrupt(32, change2, CHANGE);
  attachInterrupt(33, change3, CHANGE);
}







void loop() {
  Serial.println("start");
  ///////////////////////////// First Command, send and check if it worked ////////////////////////////////////////////////


  digitalWrite(CS, LOW); // enable Slave Select
  for (int x = 0; x < 13; x++)
  {
    send_uint16_t(transmission[x]);
  }
  digitalWrite(CS, HIGH); // disable Slave Select

  // check output, see if it's accurate
  delayMicroseconds(5000);
  Serial.println("command 1 sent");
  if (digitalRead(26) || digitalRead(32)) { // checks if digital outputs are working properly
    failcount++;
    Serial.println("pins 26 and/or 32 aren't high");
  }
  else {
    int counter = 0;
    lastVal3 = 0;
    
    while (counter < 6) {
      
      if (val3 != lastVal3) { // checks if pwm is working properly, analyzes 3 waves
        Serial.println("interrupt triggered");
        time_of_edge = val3 - lastVal3;
        //this edge should be 2500 microseconds long, I'm allowing for up to 4% error (should refine this value later)
        if ((time_of_edge < 2000 || time_of_edge > 3000) && counter != 0) { //should be 2500 +- 100
          failcount++;
          counter = 6;
        }
        Serial.println(time_of_edge);
        
        lastVal3 = val3;
        counter++;
      }
      
    } 
  }
  iteration_count++;


  ///////////////////////////// Second Command, send and check if it worked ////////////////////////////////////////////////


  digitalWrite(CS, LOW); // enable Slave Select
  for (int x = 0; x < 13; x++)
  {
    send_uint16_t(transmission_2[x]);
  }
  digitalWrite(CS, HIGH); // disable Slave Select


  // check output, see if it's accurate
  delayMicroseconds(5000);
  Serial.println("command 2 sent");
  if (digitalRead(33)) // checks if digital outputs are working properly
    failcount++;
  else {
    int counter = 0;
    while (counter < 6) { //checks first pwm output
      if (val1 != lastVal1) { // checks if pwm is working properly, analyzes 3 waves
        Serial.println("interrupt triggered");
        time_of_edge = val1 - lastVal1;
        if ((time_of_edge < 280 || (time_of_edge > 330 && time_of_edge < 1670) || time_of_edge > 1720) && counter != 0) { //305 +- 25 or 1695 +-25
          failcount++;
          counter = 6;
        }
        Serial.println(time_of_edge);
        lastVal1 = val1;
        counter++;
      }
    }
    
    counter = 0;
    while (counter < 6) { //checks second pwm output
      if (val2 != lastVal2) { // checks if pwm is working properly, analyzes 3 waves
        Serial.println("interrupt triggered");
        time_of_edge = val2 - lastVal2;
        if ((time_of_edge < 280 || (time_of_edge > 330 && time_of_edge < 1670) || time_of_edge > 1720) && counter != 0) {
          failcount++;
          counter = 6;
        }
        Serial.println(time_of_edge);
        lastVal2 = val2;
        counter++;
      }
    }
    
  }
  iteration_count++;

  
//  /////////////////////////////////////// Third Command, send and check if it worked ////////////////////////////////////////////////
//
//
//  digitalWrite(CS, LOW); // enable Slave Select
//  for (int x = 0; x < 13; x++)
//  {
//    send_uint16_t(transmission_3[x]);
//  }
//  digitalWrite(CS, HIGH); // disable Slave Select
//  // check output, see if it's accurate
//
//
//  // check output, see if it's accurate
//  delayMicroseconds(500);
//  Serial.println("command 3 sent");
//  if (digitalRead(26) || !digitalRead(32)) // checks if digital outputs are working properly
//    failcount++;
//  else {
//    int counter = 0;
//    while (counter < 6) {
//      if (val3 != lastVal3) { // checks if pwm is working properly, analyzes 3 waves
//        counter++;
//        time_of_edge = val3 - lastVal3;
//        //a cycle should last 10000 microseconds
//        if (time_of_edge < 3033 || (time_of_edge > 3233 && time_of_edge < 6767) || time_of_edge > 6967) {  //  6867 +- 100 or 3133 +- 100
//          failcount++;
//          counter = 6;
//        }
//        lastVal3 = val3;
//      }
//    }
//  }
//  iteration_count++;
//

  /////////////////////////////////////// Output to Serial Monitor ////////////////////////////////////////////////

  Serial.print("iteration_count: ");
  Serial.print(iteration_count);
  Serial.print(" failcount: ");
  Serial.print(failcount);
  Serial.print(" micros value: ");
  Serial.println(micros());

}


void change1() {
  val1 = micros();
}
void change2() {
  val2 = micros();
}
void change3() {
  val3 = micros();
}

void send_uint16_t(uint16_t u)
{
  // Robert Read's code
  SPI.transfer(*(((byte *)&u) + 0));
  byte b1 = *(((byte *)&u) + 0);

  SPI.transfer(*(((byte *)&u) + 1));
  byte b2 = *(((byte *)&u) + 1);
}
