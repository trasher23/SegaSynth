/*
  Yamaha YM2612 FM Chip
*/

#ifndef YM2612_H_
#define YM2612_H_

#include <Arduino.h>

//Digital pins: D0-D7

//Control pins
#define _IC PIN_B0
#define _CS PIN_B1
#define _WR PIN_B2
#define _RD PIN_B3
#define _A0 PIN_B4
#define _A1 PIN_B5

#define RESET_WAIT 25

class YM2612 {
private:  
  void resetChip();
  void setVoice();
  void setRegister(byte address, byte data, byte port=0);
  void setVelocity(byte velocity);
public:   
  YM2612();
  void keyOn(byte note, byte velocity);
  void keyOff(byte note, byte velocity);
};

#endif
