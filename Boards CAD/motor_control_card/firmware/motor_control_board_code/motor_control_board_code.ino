#include "Arduino.h"
#include "MultiStepper.h"
#include "AccelStepper.h"

#include <SercomSPISlave.h>
SercomSPISlave SPISlave;

byte received_signal_raw_bytes[12]; //raw bytes unloaded from the spi port
uint16_t received_signal_reconstructed[6] = {0, 0, 0, 0, 0, 0}; //data coming out of the spi port reformated into int16's
byte buf[1];

volatile byte indx = 0;
volatile boolean process;
byte homestatus1, homestatus2;
long stepper1PosHome, stepper2PosHome;

//permanant pin mapping order: step, dir, enable
const uint32_t pins_for_stepper_1[2] = {4, 5};
const uint32_t pins_for_stepper_2[2] = {7, 8};
const uint32_t switch_pins[2] = {2, 3};
volatile byte switch_states[2];

////Declaration of steppers//////////
AccelStepper stepper1(1, 4, 5); //type, step, dir
AccelStepper stepper2(1, 6, 7); //type, step, dir

//#define DEBUG
#define DEBUG_OUTPUT

void setup(void)
{
  Serial.begin(115200);

  indx = 0;              // received_signal_raw_bytes scal
  process = false;

  pinMode(switch_pins[0], INPUT);
  pinMode(pins_for_stepper_1[0], OUTPUT);
  pinMode(pins_for_stepper_2[0], OUTPUT);

  pinMode(switch_pins[1], INPUT);
  pinMode(pins_for_stepper_1[1], OUTPUT);
  pinMode(pins_for_stepper_2[1], OUTPUT);

  // These are homing switch interrupts that we are not using now...
  //  attachInterrupt(digitalPinToInterrupt(switch_pins[0]), switch_state_stepper_1, CHANGE);
  //  attachInterrupt(digitalPinToInterrupt(switch_pins[1]), switch_state_stepper_2, CHANGE);
  SPISlave.Sercom1init();
}

void loop(void)
{
//  #ifdef DEBUG
//    SerialUSB.println("looping");
//  #endif
  if (process) {
    for (int x = 0; x < 6; x++) {
      received_signal_reconstructed[x] = readuint16_t((x * 2), received_signal_raw_bytes);
      #ifdef DEBUG_OUTPUT
      SerialUSB.print("i : ");
      SerialUSB.print(x);
      SerialUSB.print(" : ");
      SerialUSB.println(received_signal_reconstructed[x], HEX);
      #endif
    }
    stepper1.setAcceleration((float) received_signal_reconstructed[0]);
    stepper2.setAcceleration((float) received_signal_reconstructed[1]);
    stepper1.setMaxSpeed((float) received_signal_reconstructed[2]);
    stepper2.setMaxSpeed((float) received_signal_reconstructed[3]);
    stepper1.moveTo(received_signal_reconstructed[4]);
    stepper2.moveTo(received_signal_reconstructed[5]);

    indx = 0;
    process = false;
  }
  stepper1.run();
  stepper2.run();
}


void receive_byte(byte c)
{
  // byte c = SPDR; // read byte from SPI Data Register
  if (indx < sizeof received_signal_raw_bytes) {
    received_signal_raw_bytes[indx] = c; // save data in the next index in the array received_signal_raw_bytes
    indx++;
  }
  if (indx >= sizeof received_signal_raw_bytes) {
    process = true;
  }
}
