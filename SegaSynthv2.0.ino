/*
  Yamaha YM2612 Chip Synthesiser
  Teensy ++ 2.0
*/

#include "YM2612.h"

//Sound chips
YM2612* ym2612 = new YM2612();

void setup() {  
  Serial.begin(38400);  
  usbMIDI.setHandleNoteOn(noteOn);
  usbMIDI.setHandleNoteOff(noteOff);
}

void loop() {
  usbMIDI.read();
}

void noteOn(byte channel, byte note, byte velocity) 
{
  ym2612->keyOn(note, velocity);
}

void noteOff(byte channel, byte note, byte velocity) 
{
  ym2612->keyOff(note, velocity);
}
