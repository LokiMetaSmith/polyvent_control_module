
uint32_t lastMicros = 0;
uint32_t timeSinceLastSwitch = 0;
uint8_t lastState = 0;

//should be called as fast as possible
void pwmAtFrequency(uint16_t frequency, uint16_t dutyCycle, uint8_t valve_num) { // this function doesn't deal with micros rollover, won't necessarily work for more than 70 minutes
  
  uint32_t pulse_mic_s = 1000000 / frequency;
  uint32_t duty_cycle_scaled_mic_s = map(dutyCycle, 0, 65535, 0, pulse_mic_s);

  uint32_t curMicros = micros();
  uint32_t timeSinceLastSwitch;
  if (curMicros < lastMicros) {  // TODO: This means the clock has rolled over.
    lastMicros = 0; // Note this is slightly innacurate for one cycle every 70 minuts when micros rolls over.
     uint32_t mics_since_clock_rolled_over = UINT32_MAX - lastMicros;
     timeSinceLastSwitch = curMicros + mics_since_clock_rolled_over;
  } else {
     timeSinceLastSwitch = curMicros - lastMicros; 
  }
  if (lastState == LOW) {
    if (timeSinceLastSwitch >= duty_cycle_scaled_mic_s) {
      lastMicros = curMicros;
      lastState = HIGH;
      digitalWrite(valve_num, LOW); // This takes 3.45 micros defined by the documentation
    } 
  } else {    // if cycle is odd, stay off for the rest of the duty cycle
    if (timeSinceLastSwitch >= (pulse_mic_s - duty_cycle_scaled_mic_s)) {
      lastMicros = curMicros;
      lastState = LOW;
      digitalWrite(valve_num, HIGH); // This takes 3.45 micros defined by the documentation
    }
  }
}
