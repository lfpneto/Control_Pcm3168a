#include "Control_Pcm3168a.h"
#include "HardwareSerial.h"

void AudioControlPCM3168A::TesteSerial(void) {
  if (debugToSerial) Serial.print("Serial Print is Working \n");
  return;
}

bool AudioControlPCM3168A::enable(void) {
  if (debugToSerial) {
    Serial.printf("<I2C> Clock '%d'\n", Wire.getClock()); //return: bus frequency in Hz
    Serial.printf("<I2C> SCL pin '%d'\n", Wire.getSCL()); // get the current SCL pin
    Serial.printf("<I2C> SDA pin '%d'\n", Wire.getSDA()); // get the current SDA pin
    if (Wire.getError())              // Check if error occured
      Serial.print("<I2C> FAIL\n");
  }

  // Setup for Master mode, pins 18/19, external pullups, 400kHz, 200ms default timeout
  Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, 400000);
  Wire.setDefaultTimeout(200000); // 200ms
  // TODO: Option Between Master Or Slave
  if (debugToSerial) Serial.println("<I2C> PCM3168A enable done");
  return true;
}

bool AudioControlPCM3168A::disable(void) {
  if (debugToSerial) Serial.print("RUN Disable()\n");
  return true;
}

void AudioControlPCM3168A::setSlaveAddress(uint8_t adress) {
  //-------------------------------------------------------------------------
  // setSlaveAddress - Set's the I2C slave adress of the PCM3168A
  // return: none
  // parameters:
  //      adress = adress to set
  //
  //      PCM3168A Adress = 10001'x1''x2’
  //      X1 & X2 defined by hardware conection on pins 45 and 44
  //      //-----x1--x2
  //      0x44---L---L
  //      0x45---L---H
  //      0x46---H---L
  //      0x47---H---H

  if (adress != 0x44) {
    i2c_addr = adress;
  }
  if (debugToSerial) Serial.printf("<I2C> Slave Adress '0x%2X'\n", i2c_addr);
}

void AudioControlPCM3168A::readReg_(uint8_t startReg, uint8_t lng) {
  //-------------------------------------------------------------------------
  // readReg_ - Read a number of bytes from starting at register
  // return: none
  // parameters:
  //      startReg = target register address
  //      length = number of bytes requested
  //
  Wire.beginTransmission(i2c_addr); //Initialize Tx buffer for transmit to Slave at address
  Wire.write(startReg);             // First register to read
  Wire.endTransmission(I2C_NOSTOP);
  Wire.requestFrom(i2c_addr, lng);  // target, number of bytes requested

  // Check if error occured
  if (debugToSerial) {
    if (Wire.getError())
      Serial.print("<readReg_> FAIL\n");
    else
    {
      while (Wire.available()) { // slave may send less than requested
        Serial.print(Wire.read(), BIN);
        Serial.print("\n");
      }
    }
  }
  return;
}

void AudioControlPCM3168A::writeReg_(uint8_t data, uint8_t startReg) {
  //-------------------------------------------------------------------------
  // writeReg_ - Write a number of bytes, starting at register
  // return: none
  // parameters:
  //      startReg = target register address
  //      length = number of bytes requested
  //
  if (debugToSerial) Serial.printf("_<writeReg_> Writing '0x%2X' at register '0x%2X' \n", data, startReg);
  Wire.beginTransmission(i2c_addr);                   // Initialize Tx buffer for transmit to Slave at address
  Wire.write(startReg);                               // Aqui peço qual o registo onde começa a leitura
  Wire.write(data);                                   // Aqui passo os dados a registar
  Wire.endTransmission(I2C_STOP);                     // Stop para enviar um STOP
  if (debugToSerial) {
    if (Wire.getError())
      Serial.print("_<writeReg_> FAIL\n");
  }

  return;
}