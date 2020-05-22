#include "YM2612.h"
#include "ChannelManager.h"

Channel* channel;
ChannelManager channelManager;

YM2612::YM2612() {
  //Data pins
  DDRD = 0xFF;
  PORTD = 0x00;

  //Control pins
  DDRB = 0xFF;
  PORTB = 0x3C; //00111100
  
  resetChip();
  setVoice();
}

void YM2612::resetChip() {
  //Reset the YM's state.
  digitalWriteFast(_IC, HIGH);
  delayMicroseconds(RESET_WAIT);
  digitalWriteFast(_IC, LOW);
  delayMicroseconds(RESET_WAIT);
  digitalWriteFast(_IC, HIGH);
  delayMicroseconds(RESET_WAIT);
}

void YM2612::setRegister(byte address, byte data, byte port) {  
  //Write the register address first.
  digitalWriteFast(_A1, port);
  digitalWriteFast(_A0, LOW);
  PORTD = address;

  //Technically we should give *some* time between setting the bytes and
  //pulling the pins, but it seems to work fine without that.
  digitalWriteFast(_CS, LOW);
  digitalWriteFast(_WR, LOW);

  // We must wait for the chip to finish reading our address before loading in the next byte.
  // We shouldn't raise the WR and CS until at least WRITE_WAIT have passed.
  delayMicroseconds(RESET_WAIT);
  digitalWriteFast(_WR, HIGH);
  digitalWriteFast(_CS, HIGH);

  //Same as above, but with A0 high to write the register data.
  digitalWriteFast(_A0, HIGH);
  PORTD = data;
  delayMicroseconds(RESET_WAIT);
  digitalWriteFast(_CS, LOW);
  digitalWriteFast(_WR, LOW);
  delayMicroseconds(RESET_WAIT);

  //Disable the chip and write pins.
  digitalWriteFast(_WR, HIGH);
  digitalWriteFast(_CS, HIGH);
  delayMicroseconds(RESET_WAIT);
}

void YM2612::setVoice() { 
  setRegister(0x22, 0x00); // LFO off
  setRegister(0x27, 0x00); // CH3 Normal
  setRegister(0x28, 0x00); // Key off, channel 1
  setRegister(0x28, 0x01); // Key off, channel 2
  setRegister(0x28, 0x02); // Key off, channel 3
  setRegister(0x28, 0x03); // Key off, channel 4
  setRegister(0x28, 0x04); // Key off, channel 5
  setRegister(0x28, 0x05); // Key off, channel 6
  setRegister(0x2B, 0x00); // DAC off
  
  for(int port=0; port<2; port++)
  {
    for(int i=0; i<3; i++)
    {
      /*
       port 0 : Channels 1-3
       port 1 :  Channels 4-6
       
        
        0x30 = Ch 1, Op 1
          0x31 = Ch 2, Op 1
          0x32 = Ch 3, Op 1
 
        0x34 = Ch 1, Op 2
          0x35 = Ch 2, Op 2
          0x36 = Ch 3, Op 2
          
        0x38 = Ch 1, Op 3
          0x39 = Ch 2, Op 3
          0x3A = Ch 3, Op 3
          
        0x3C = Ch 1, Op 4
          0x3D = Ch 2, Op 4
          0x3E = Ch 3, Op 4


        DT1/Mul : detune/multiple
          Both DT1 (detune) and MUL (multiple) relate the operator’s frequency to the overall frequency.

          DT1 gives small variations from the overall frequency × MUL. 
          The MSB of DT1 is a primitive sign bit, and the two LSB’s are magnitude bits. 
          
          MUL ranges from 0 to 15 (decimal), and multiplies the overall frequency, with the exception that 0 results in multiplication by 1/2. 
          That is, MUL=0 to 15 gives ×1/2, ×1, ×2, ... ×15.

        Total Level : total level
          TL (total level) represents the envelope’s highest amplitude, with 0 being the largest and 127 (decimal) the smallest. 
          A change of one unit is about 0.75 dB.
          To make a note softer, only change the TL of the slots (the output operators). 
          Changing the other operators will affect the flavor of the note. 
          
        RS/AR : rate scaling/attack rate
          Register 50H contains RS (rate scaling) and AR (attack rate). 
          
          RS affects AR, D1R, D2R and RR in the same way. 
          RS is the degree to which the envelope becomes narrower as the frequency becomes higher. 
          
          AR is the steepness of the initial amplitude rise, shown on page 5.
          
        AM/D1R : amplitude modulation/first decay rate          
          AM is the amplitude modulation enable, whether of not this operator will be subject to amplitude modulation by the LFO. 
          This bit is not relevant unless both the LFO is enabled and register B4′s AMS (amplitude modulation sensitivity) is non-zero. 

          D1R (first decay rate) is the initial steep amplitude decay rate (see page 4). 
          It is, like all rates, 0-31 in value and affected by RS.
          
        D2R : secondary decay rate
          D2R (secondary decay rate) is the long tail off of the sound that continues as long as the key is depressed. 
          
        D1L/RR : secondary amplitude/release rate
          D1L is the secondary amplitude reached after the first period of rapid decay. 
          It should be multiplied by 8 if one wishes to compare it to TL. 
          Again as TL, the higher the number, the more attenuated the sound.
          
          RR is the release rate, the final sharp decrease in volume after the key is released. 
          All rates are 5 bit numbers, but there are only four bits available in the register.
          
          Thus, for comparison and calculation purposes, these four bits are the MSBs and the LSB is always 1. 
          In other words, double it and add one. 
        
        SSG EG
          This register is proprietary and should be set to zero. 

        ----

        Algorithm/Feedback
              D7|D6|D5|D4|D3|D2|D1|D0
        BoH+  x  x |0  0  0 |0  0  0
                   |Feedback|Algorithm
        
        i.e. Algorithm 5 = 00000101 = 0x03
        Same algorithm with full feedback = 0x3D = 00111101 (xx 111 101)
      */
      
      //Operator 1
      setRegister(0x30 + i, 0x71, port); // DT1/Mul
      setRegister(0x40 + i, 0x23, port); // Total Level
      setRegister(0x50 + i, 0x5F, port); // RS/AR
      setRegister(0x60 + i, 0x05, port); // AM/D1R
      setRegister(0x70 + i, 0x02, port); // D2R
      setRegister(0x80 + i, 0x11, port); // D1L/RR
      setRegister(0x90 + i, 0x00, port); // SSG EG
       
      //Operator 2
      setRegister(0x34 + i, 0x0D, port); // DT1/Mul
      setRegister(0x44 + i, 0x2D, port); // Total Level
      setRegister(0x54 + i, 0x99, port); // RS/AR
      setRegister(0x64 + i, 0x05, port); // AM/D1R
      setRegister(0x74 + i, 0x02, port); // D2R
      setRegister(0x84 + i, 0x11, port); // D1L/RR
      setRegister(0x94 + i, 0x00, port); // SSG EG
       
      //Operator 3
      setRegister(0x38 + i, 0x33, port); // DT1/Mul
      setRegister(0x48 + i, 0x26, port); // Total Level
      setRegister(0x58 + i, 0x5F, port); // RS/AR
      setRegister(0x68 + i, 0x05, port); // AM/D1R
      setRegister(0x78 + i, 0x02, port); // D2R
      setRegister(0x88 + i, 0x11, port); // D1L/RR
      setRegister(0x98 + i, 0x00, port); // SSG EG
               
      //Operator 4
      setRegister(0x3C + i, 0x01, port); // DT1/Mul
      setRegister(0x4C + i, 0x00, port); // Total Level
      setRegister(0x5C + i, 0x94, port); // RS/AR
      setRegister(0x6C + i, 0x07, port); // AM/D1R
      setRegister(0x7C + i, 0x02, port); // D2R
      setRegister(0x8C + i, 0xA6, port); // D1L/RR
      setRegister(0x9C + i, 0x00, port); // SSG EG

      //Other
      setRegister(0xB0 + i, 0x03, port); // Ch FB/Algo
      setRegister(0xB4 + i, 0xC0, port); // Both Spks on
    }
  }
}

void YM2612::keyOn(byte note, byte velocity) { 
  channel = channelManager.noteOn(note, velocity);

  if ( channel->getChannelNumber() == -1 ) { return; }
  
  //note frequency
  setRegister(0xA4 + channel->getChannelOffset(), channel->getHighFrequency(), channel->getPort());
  setRegister(0xA0 + channel->getChannelOffset(), channel->getLowFrequency(), channel->getPort());
  //velocity
  setVelocity(velocity);
  //channel on
  setRegister(0x28, 0xF0 | channel->getNoteOffset());
}

void YM2612::keyOff(byte note, byte velocity) {
  int noteOffset = channelManager.noteOff(note);

  if ( noteOffset == -1 ) { return; }
  
  //channel off
  setRegister(0x28, noteOffset);
}

void YM2612::setVelocity(byte velocity) {
  //may not use this approach - use the class to store the velocity
  //determine output operator(s) for configured algorithm
  int algorithm = 3; // ?? temp debug code - need to figure out how to get, maybe class property?

  velocity = 100 - velocity; //remember low values are high
  //Serial.println(velocity);
  
  for(int port=0; port<2; port++)
  {
    for(int i=0; i<3; i++)
    {
      switch(algorithm) {
        case 0:case 1:case 2:
        case 3: {
          //algorithms 0-3 all use OP.4 as the 'slot' (AKA output or carrier)
          setRegister(0x4C + i, velocity, port);
          break;
        }
        case 4: {
          //OP.2 & OP.4
          setRegister(0x44 + i, velocity, port);
          setRegister(0x4C + i, velocity, port);
          break;
        }
        case 5: 
        case 6: {
          //algorithms 5-6 use OP.2 & OP.3 & OP.4
          setRegister(0x44 + i, velocity, port);
          setRegister(0x48 + i, velocity, port);
          setRegister(0x4C + i, velocity, port);
          break;
        }
        case 7: {
          //This algorithm uses every operator as a slot
          setRegister(0x40 + i, velocity, port);
          setRegister(0x44 + i, velocity, port);
          setRegister(0x48 + i, velocity, port);
          setRegister(0x4C + i, velocity, port);
          break;
        }
      }
    }
  }
}
