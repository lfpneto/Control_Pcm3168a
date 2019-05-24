#ifndef control_pcm3168a_h_
#define control_pcm3168a_h_

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "AudioControl.h"
#include "WireKinetis_mod.h"
#include <Arduino.h>
//#include "Wire.h"
#include <i2c_t3.h>
#include <ostream>

// *****************************************************************************
// *******************************   DEFINE   **********************************
// *****************************************************************************

#define RESET_CTRL        0x40
// 7    MRST 1=Normal Operation (default), 0=Set default  value
// 6    SRST 1=Normal Operation (default), 0=Resyncronization
// 1:0  SRDA
//        0x0 = Auto (default)
//        0x1 = Single Rate
//        0x2 = Dual Rate
//        0x3 = Quad Rate

#define DAC_CTRL_1        0x41
// 7    PSMDA
// 6:4  MSDA DAC Master/slave mode select
//      These bits control the audio interface mode for DAC operation.
//      Default value: 000 (slave mode).
//      0X0 = 000=Slave mode (default)
//      0X1 = 001=Master mode, 768 fS
//      0X2 = 010=Master mode, 512 fS
//      0X3 = 011=Master mode, 384 fS
//      0X4 = 100=Master mode, 256 fS
//      0X5 = 101=Master mode, 192 fS
//      0X6 = 110=Master mode, 128 fS
// 3:0  FMT DAC Audio interface format select
//      These bits control the audio interface format for DAC operation.
//      0x0 = 0000 = 24-bit I2S format (default)
//      0x1 = 0001 = 24-bit left-justified format
//      0x2 = 0010 = 24-bit right-justified format
//      0x3 = 0011 = 16-bit right-justified format
//      0x4 = 0100 = 24-bit I2S mode DSP format
//      0x5 = 0101 = 24-bit left-justified mode DSP format
//      0x6 = 0110 = 24-bit I2S mode TDM format
//      0x7 = 0111 = 24-bit left-justified mode TDM format
//      0x8 = 1000 = 24-bit high-speed I2S mode TDM format
//      0x8 = 1001 = 24-bit high-speed left-justified mode TDM format


#define DAC_CTRL_2        0x42
#define DAC_PHASE         0x43
#define DAC_MUTE          0x44
#define DAC_ZF            0x45
#define DAC_CTRL_3        0x46
#define DAC_ATT_0         0x47
#define DAC_ATT_1         0x48
#define DAC_ATT_2         0x49
#define DAC_ATT_3         0x4A
#define DAC_ATT_4         0x4B
#define DAC_ATT_5         0x4C
#define DAC_ATT_6         0x4D
#define DAC_ATT_7         0x4E
#define DAC_ATT_8         0x4F
#define ADC_SRAD          0x50
#define ADC_CTRL_1        0x51
#define ADC_CTRL_2        0x52
#define ADC_INPUT_CONFIG  0x53
#define ADC_PHASE         0x54
#define ADC_MUTE          0x55
#define ADC_OVF           0x56
#define ADC_CTRL_3        0x57
#define ADC_ATT_0         0x58
#define ADC_ATT_1         0x59
#define ADC_ATT_2         0x5A
#define ADC_ATT_3         0x5B
#define ADC_ATT_4         0x5C
#define ADC_ATT_5         0x5D
#define ADC_ATT_6         0x5E

class AudioControlPCM3168A : public AudioControl
{
  public:
    
    // DEBUG TO SERIAL
    AudioControlPCM3168A(void) {
      debugToSerial = false;
    };
    AudioControlPCM3168A(bool _debugToSerial) {
      debugToSerial = _debugToSerial;
    };
    void TesteSerial(void);
    bool debugToSerial;
    
    // I2C Slave
    void setSlaveAddress(uint8_t adress = 0x44);


    // AudioControl
    bool enable(void);
    bool disable(void);

    bool volume(float n) {
      //TODO
      if (debugToSerial) readDac();
      return true;
    }

    bool inputLevel(float n) {
      //TODO
      if (debugToSerial) readAdc();
      return true;
    }
    bool inputSelect(int n) {
      //TODO
      if (debugToSerial) {
        Serial.printf("CALL inputSelect(n)");
        Serial.printf("n = '0x%2X' reads ", n);
      }
      return true;
    }

    // ***
    // *** READ
    void readReg_(uint8_t startReg, uint8_t lng);
    inline void readReg(uint8_t reg) {
      if (debugToSerial) {
        Serial.printf("<readReg> Register '0x%2X' reads ", reg);
      } readReg_(reg, 1);
    }
    inline void readAllReg(void) {
      if (debugToSerial) Serial.printf("<readAllReg()>\n");
      readReg_(0x40, 31);
    }
    inline void readAdc(void) {
      if (debugToSerial) {
        Serial.printf("<readAdc> \n");
      }
      readReg_(0x58, 7);
    }
    inline void readDac(void) {
      if (debugToSerial) {
        Serial.printf("<readDac> \n");
      }
      readReg_(0x47, 9);
    }

    // ***
    // *** WRITE
    void writeReg_(uint8_t data, uint8_t startReg);
    inline void begin(int mode) {
      switch (mode) {
        case 1:
          if (debugToSerial) Serial.printf("<begin()> Mode 1 - DAC I2S Slave - ADC I2S Slave\n");
          writeReg_(0x03, DAC_CTRL_1);      //DAC I2S Slave
          writeReg_(0x03, ADC_CTRL_1);      //ADC I2S Slave
          break;
        case 2:
          if (debugToSerial) Serial.printf("<begin()> Mode 2 - DAC I2S Master - ADC I2S Slave\n");
          writeReg_(0x23, DAC_CTRL_1);      //DAC I2S Master
          writeReg_(0x03, ADC_CTRL_1);      //ADC I2S Slave
          break;
        case 3:
          if (debugToSerial) Serial.printf("<begin()> Mode 3 - DAC I2S Slave - ADC I2S Master\n");
          writeReg_(0x03, DAC_CTRL_1);      //DAC I2S Slave
          writeReg_(0x23, ADC_CTRL_1);      //ADC I2S Master
          break;
        case 4:
          if (debugToSerial) Serial.printf("<begin()> Mode 4 - DAC I2S Master - ADC I2S Master\n");
          writeReg_(0x23, DAC_CTRL_1);      //DAC I2S Master
          writeReg_(0x23, ADC_CTRL_1);      //ADC I2S Master
          break;
      }
    }
    inline void MasterReset(void) {
      if (debugToSerial) Serial.printf("<MasterReset()> \n");
      writeReg_(0x00, RESET_CTRL);
    }
    inline void Resynch(void) {
      if (debugToSerial) Serial.printf("<Resynch()> \n");
      writeReg_(0x40, RESET_CTRL);
      delay(100);
    }

  protected:


  private:
    uint8_t i2c_addr = 0x44;



};
#endif
