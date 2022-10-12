#include "SPI.h"
#include <HoneywellTruStabilitySPI.h>
const int CS = 14; // 15
float tankSensorVal = 0;
const int ESP_CS_PINS[11] = {4, 5, 13, 14, 15, 16, 17, 25, 26, 32, 33};
int stage = 0;


TruStabilityPressureSensor sensor1( ESP_CS_PINS[5], -101.97, 101.97 );
TruStabilityPressureSensor sensor2( ESP_CS_PINS[6], -101.97, 101.97 );
TruStabilityPressureSensor sensor3( ESP_CS_PINS[8], -101.97, 101.97);
TruStabilityPressureSensor sensor4( ESP_CS_PINS[7], -6.89476, 6.89476 );


uint16_t air_input[13] = {200, 0xFFFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t oxygen_input[13] = {200, 0, 0xFFFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t solenoid_output[13] = {200, 0, 0, 0, 40000, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t patient_inflating_valve[13] = {200, 0, 0, 0xFFFF, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t all_off[13] = {200, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void setup() {
  Serial.begin(115200);
  for (int x = 0; x < sizeof(ESP_CS_PINS) / sizeof(ESP_CS_PINS[0]); x++) {
    pinMode(ESP_CS_PINS[x], OUTPUT);
    digitalWrite(ESP_CS_PINS[x], HIGH);
  }

  SPI.begin();

  sensor1.begin(); // run sensor initialization
  sensor2.begin();
  sensor3.begin();
  sensor4.begin();

  delay(3000);
}

void loop() {
  
  if (sensor4.readSensor() == 0)
    tankSensorVal = sensor4.rawPressure();
  Serial.println(stage);

  digitalWrite(14, LOW);
  for (int x = 0; x < 13; x++)
    send_uint16_t(air_input[x]);
  digitalWrite(14, HIGH);
  Serial.println(tankSensorVal);

  wait_until_pressure(3000, 1);

  digitalWrite(14, LOW);
  for (int x = 0; x < 13; x++)
    send_uint16_t(solenoid_output[x]);
  digitalWrite(14, HIGH);
  Serial.println(tankSensorVal);

  wait_until_pressure(2500, 0);

  digitalWrite(14, LOW);
  for (int x = 0; x < 13; x++)
    send_uint16_t(all_off[x]);
  digitalWrite(14, HIGH);
  Serial.println(tankSensorVal);

  delay(1000);

  digitalWrite(14, LOW);
  for (int x = 0; x < 13; x++)
    send_uint16_t(patient_inflating_valve[x]);
  digitalWrite(14, HIGH);
  Serial.println(tankSensorVal);

  delay(3000);

}

//waits until the pressure reaches a certain value in bar. if edge is 0, then the function waits for a falling edge, 1 has the opposite effect
void wait_until_pressure(float pressure_setpoint, bool edge) {
  if (sensor4.readSensor() == 0)
    tankSensorVal = sensor4.rawPressure();

  while ((edge && tankSensorVal < pressure_setpoint) || (!edge && tankSensorVal > pressure_setpoint)) {
    if (sensor4.readSensor() == 0)
      tankSensorVal = sensor4.rawPressure();
      Serial.println(tankSensorVal);
    delay(1); // keep the loop speed reasonable, no need to go that fast
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
