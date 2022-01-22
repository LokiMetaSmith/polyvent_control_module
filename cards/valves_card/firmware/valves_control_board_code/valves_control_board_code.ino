/*
  Copyright (C) 2021 Public Invention, Nathaniel Bechard, Robert L. Read

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  For the GNU General Public License see https://www.gnu.org/licenses/

  Contact Information
  -------------------
  Robert L. Read <read.robert@gmail.com>
*/
// runs on the polyvent valves board
#include "Arduino.h"
#include <SercomSPISlave.h>
SercomSPISlave SPISlave;


//#define DEBUG // comment this line out to not print debug data on the SerialUSB bus
#define DEBUG_OUTPUT

byte buf[1];
byte received_signal_raw_bytes [26];
uint16_t received_signal_reconstructed[13];
volatile byte indx;
volatile boolean process;
uint16_t frequency_of_pwm = 1;

//pin mapping, in order, fist controls solenoid zero, second controls solenoid 1, etc
const int pins [12] = {2, 3, 4, 5, 6, 7, 8, 9, 14, 15, 16, 17};
const int number_of_pins = sizeof(pins) / sizeof(*pins);

uint16_t readuint16_t(int loc_in_buffer, byte *buff)
{
  int n = loc_in_buffer;
  uint16_t v;
  *(((byte *)&v)) = buff[n];
  *(((byte *)&v) + 1) = buff[n + 1];
  return v;
}

void setup (void) {
  SerialUSB.begin(115200);
  for (int x = 0; x < number_of_pins; x++) {
    pinMode(pins[x], OUTPUT);
    digitalWrite(pins[x], LOW);
#ifdef DEBUG_OUTPUT
    SerialUSB.print("pin number: ");
    SerialUSB.print(pins[x]);
    SerialUSB.println("set to output");
#endif
  }

  indx = 0; // received_signaler empty
  process = false;
  SPISlave.Sercom1init();
}


void loop (void) {
  if (process) {
    SerialUSB.print("process: ");
    SerialUSB.println(process);
    

    for (int i = 0; i < 13; i++) {
      received_signal_reconstructed[i] = readuint16_t((i * 2), received_signal_raw_bytes);
#ifdef DEBUG_OUTPUT
      SerialUSB.print("i : ");
      SerialUSB.print(i);
      SerialUSB.print(" : ");
      SerialUSB.println(received_signal_reconstructed[i]);
#endif
    }

    frequency_of_pwm = received_signal_reconstructed[0];
    for (int i = 0; i < 12; i++) {
      if (received_signal_reconstructed[i + 1] == 65535) {
        digitalWrite(pins[i], HIGH);
      }
      else if (received_signal_reconstructed[i + 1] == 0) {
        digitalWrite(pins[i], LOW);
      }
    }
    process = false;
    indx = 0; // indx should be 26 before this, we have to reset
    SerialUSB.print("process end: ");
    SerialUSB.println(process);
  }

  for (int x = 0; x < 12; x++) {
    if (received_signal_reconstructed[x + 1] != 65535 && received_signal_reconstructed[x + 1] != 0) {
      pwmAtFrequency(frequency_of_pwm, received_signal_reconstructed[x + 1], pins[x]);
      
    }
  }

}
