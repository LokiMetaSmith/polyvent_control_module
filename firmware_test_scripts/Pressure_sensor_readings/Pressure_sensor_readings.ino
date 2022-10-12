/*
   PressureSensorTest

   Fetch and print values from a Honeywell
   TruStability HSC Pressure Sensor over SPI

   The sensor values used in this demo are
   for a -15 to 15 psi gauge pressure sensor.

*/

#include <HoneywellTruStabilitySPI.h>

const int ESP_CS_PINS[11] = {4, 5, 13, 14, 15, 16, 17, 25, 26, 32, 33};


TruStabilityPressureSensor sensor1( ESP_CS_PINS[5], -101.97, 101.97 );
TruStabilityPressureSensor sensor2( ESP_CS_PINS[6], -101.97, 101.97 );
TruStabilityPressureSensor sensor3( ESP_CS_PINS[7], -101.97, 101.97 );
TruStabilityPressureSensor sensor4( ESP_CS_PINS[8], -6.89476, 6.89476);

void setup() {
  for(int i = 0; i<sizeof(ESP_CS_PINS)/sizeof(ESP_CS_PINS[0]); i++){
    pinMode(ESP_CS_PINS[i], OUTPUT);
    digitalWrite(ESP_CS_PINS[i], HIGH);
  }
  
  Serial.begin(115200); // start Serial communication
  SPI.begin(); // start SPI communication
  sensor1.begin(); // run sensor initialization
  sensor2.begin(); 
  sensor3.begin(); 
  sensor4.begin(); 
}

void loop() {

  // the sensors return 0 when new data is ready
  if ( sensor1.readSensor() == 0 ) {
    Serial.print( "\t pressure [cmH20] of sensor 1: " );
    Serial.println( sensor1.rawPressure() );
  }
//  if ( sensor2.readSensor() == 0 ) {
//    Serial.print( "\t pressure [cmH20] of sensor 2: " );
//    Serial.println( sensor2.rawPressure() );
//  }
//    if ( sensor3.readSensor() == 0 ) {
//    Serial.print( "\t pressure [cmH20] of sensor 3: " );
//    Serial.println( sensor3.rawPressure() );
//  }
//    if ( sensor4.readSensor() == 0 ) {
//    Serial.print( "\t pressure [bar] of sensor 4: " );
//    Serial.println( sensor4.rawPressure() );
//  }
  delay(100);
}
