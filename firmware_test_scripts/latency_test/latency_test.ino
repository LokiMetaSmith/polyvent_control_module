#include "SPI.h"
#include <HoneywellTruStabilitySPI.h>
#include <SFM3X00.h>


const int CS = 14; // 15
float tankSensorVal = 0, close_output_sensor_val = 0, far_output_sensor_val = 0;
const int ESP_CS_PINS[13] = {4, 5, 13, 14, 15, 16, 17, 21, 22, 25, 26, 32, 33};
int stage = 0;

int fiO2 = 50; //0-100 value


#define FLOW_SENSOR_ADDRESS 0x40

SFM3X00 flowSensor(FLOW_SENSOR_ADDRESS);

TruStabilityPressureSensor sensor1( ESP_CS_PINS[5], -101.97, 101.97 ); // airway mBar
TruStabilityPressureSensor sensor2( ESP_CS_PINS[6], -101.97, 101.97 ); // piv mBar
TruStabilityPressureSensor sensor3( ESP_CS_PINS[9], -6.89476, 6.89476 ); // the modification, cs 8 and 9 are I2c pins  //tank pressure sensor, in bar
//TruStabilityPressureSensor sensor4( ESP_CS_PINS[10], -101.97, 101.97); //not used



uint16_t air_input[13] = {200, 0xFFFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t oxygen_input[13] = {200, 0, 0xFFFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t solenoid_output[13] = {200, 0, 0, 10500, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t patient_inflating_valve[13] = {200, 0, 0, 0, 0xFFFF, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t all_off[13] = {200, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void setup() {
  Wire.begin();
  Serial.begin(115200);
  
  flowSensor.begin();
  delay(100);
    
  SPI.begin();
  

  for (int x = 0; x < sizeof(ESP_CS_PINS) / sizeof(ESP_CS_PINS[0]); x++) {
    pinMode(ESP_CS_PINS[x], OUTPUT);
    digitalWrite(ESP_CS_PINS[x], HIGH);
  }


  sensor1.begin(); // run sensor initialization
  sensor2.begin();
  sensor3.begin();
  //  sensor4.begin();
   // flowSensor.begin();

  Serial.println();
  Serial.print("sensor serial number: ");
  Serial.println(flowSensor.serialNumber, HEX);
  Serial.print("sensor article number: ");
  Serial.println(flowSensor.articleNumber, HEX);
  Serial.println();
  Serial.print("read scale factor: ");
  Serial.println(flowSensor.flowScale);
  Serial.print("read flow offset: ");
  Serial.println(flowSensor.flowOffset);

  delay(3000);

}

void loop() {

  while (sensor3.readSensor() != 0) { }
  float tankSensorValEmpty = sensor3.pressure();

  float delta = 1.5 - tankSensorValEmpty; //1.5 is the final tank pressure
  float O2_partial_pressure = delta * (fiO2 / 100.00);
  // float Air_partial_pressure = delta * ((100 - fiO2)/100);


  digitalWrite(14, LOW);
  for (int x = 0; x < 13; x++)
    send_uint16_t(oxygen_input[x]);
  digitalWrite(14, HIGH);

  wait_until_pressure(O2_partial_pressure + tankSensorValEmpty, 1);

  digitalWrite(14, LOW);
  for (int x = 0; x < 13; x++)
    send_uint16_t(air_input[x]);
  digitalWrite(14, HIGH);


  wait_until_pressure(1.5, 1);
 

int startMillis = millis();
  digitalWrite(14, LOW);
  for (int x = 0; x < 13; x++)
    send_uint16_t(solenoid_output[x]);
  digitalWrite(14, HIGH);

  
//////////////////////////// Latency testing starts here /////////////////////////////

while((millis() - startMillis) < 1000){
  
  Serial.print("Flow: "); 
  Serial.print(flowSensor.readFlow());
  Serial.print(" Pressure: ");
  if (sensor2.readSensor() == 0)
    Serial.print(sensor2.pressure());
  else
    Serial.print(0);
    
  Serial.print(" Time: ");
  Serial.print(millis() - startMillis);
  Serial.println();
  
}

//////////////////////////// Latency testing ends here ////////////////////////////////



  digitalWrite(14, LOW);
  for (int x = 0; x < 13; x++)
    send_uint16_t(all_off[x]);
  digitalWrite(14, HIGH);
 // Serial.println(tankSensorVal);

  delay(1000);

  digitalWrite(14, LOW);
  for (int x = 0; x < 13; x++)
    send_uint16_t(patient_inflating_valve[x]);
  digitalWrite(14, HIGH);
//  Serial.println(tankSensorVal);

  delay(3000);

}

//waits until the pressure reaches a certain value in bar. if edge is 0, then the function waits for a falling edge, 1 has the opposite effect
void wait_until_pressure(float pressure_setpoint, bool edge) {
  if (sensor3.readSensor() == 0)
    tankSensorVal = sensor3.pressure();

  while ((edge && tankSensorVal < pressure_setpoint) || (!edge && tankSensorVal > pressure_setpoint)) {
    if (sensor3.readSensor() == 0)
      tankSensorVal = sensor3.pressure();
    if (sensor1.readSensor() == 0)
      close_output_sensor_val = sensor1.pressure();
    if (sensor2.readSensor() == 0)
      far_output_sensor_val = sensor2.pressure();
//
//    Serial.print(close_output_sensor_val);
//    Serial.print(", ");
//    Serial.print(far_output_sensor_val);
//    Serial.print(", ");
//    Serial.println(tankSensorVal);

    delay(10); // keep the loop speed reasonable, no need to go that fast
  }
}


void send_uint16_t(uint16_t u)
{
  // Robert Reed's code
  SPI.transfer(*(((byte *)&u) + 0));
  byte b1 = *(((byte *)&u) + 0);

  SPI.transfer(*(((byte *)&u) + 1));
  byte b2 = *(((byte *)&u) + 1);
}
