void SERCOM1_Handler() // 25.7 Register Summary, page 454 atmel 42181, samd21
{
  
#ifdef DEBUG
  SerialUSB.println("In SPI Interrupt");
#endif

  uint8_t data = 0;
  uint8_t interrupts = SERCOM1->SPI.INTFLAG.reg; //Read SPI interrupt register
  
#ifdef DEBUG
  SerialUSB.print("Interrupt: "); SerialUSB.println(interrupts);
#endif



  if (interrupts & (1 << 3)) // 8 = 1000 = SSL
  {
#ifdef DEBUG
    SerialUSB.println("SPI SSL Interupt");
#endif
    SERCOM1->SPI.INTFLAG.bit.SSL = 1; //clear slave select interrupt
    data = SERCOM1->SPI.DATA.reg; //Read data register
#ifdef DEBUG
    SerialUSB.print("DATA: "); SerialUSB.println(data);
#endif
    SERCOM1->SPI.INTFLAG.bit.RXC = 1; //clear receive complete interrupt
  }



  // This is where data is received, and is written to a buffer, which is used in the main loop
  if (interrupts & (1 << 2)) // 4 = 0100 = RXC
  {
#ifdef DEBUG
    SerialUSB.println("SPI Data Received Complete Interrupt");
#endif
    data = SERCOM1->SPI.DATA.reg; //Read data register
    buf[0] = data; // copy data to buffer
#ifdef DEBUG
    SerialUSB.print("DATA: ");
    SerialUSB.println(data);
#endif
    SERCOM1->SPI.INTFLAG.bit.RXC = 1; //clear receive complete interrupt
    receive_byte(data);
  }



  

  if (interrupts & (1 << 1)) // 2 = 0010 = TXC
  {
#ifdef DEBUG
    SerialUSB.println("SPI Data Transmit Complete Interrupt");
#endif
    SERCOM1->SPI.INTFLAG.bit.TXC = 1; //clear receive complete interrupt
  }


  

  if (interrupts & (1 << 0)) // 1 = 0001 = DRE
  {
#ifdef DEBUG
    SerialUSB.println("SPI Data Register Empty Interrupt");
#endif
    SERCOM1->SPI.DATA.reg = 0xAA;
    SERCOM1->SPI.INTFLAG.bit.DRE = 1;
  }


#ifdef DEBUG
  SerialUSB.println("----------");
#endif
}

//uint16_t readuint16_t(int loc_in_buffer, byte *buff)
//{
//  int n = loc_in_buffer;
//  uint16_t v;
//  *(((byte *)&v)) = buff[n];
//  *(((byte *)&v) + 1) = buff[n + 1];
//  return v;
//}

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
