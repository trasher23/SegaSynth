#ifndef CHANNEL_H
#define CHANNEL_H

#define mask(s) (~(~0<<s))

//note to freq system by diegodorado: https://github.com/diegodorado/arduinoProjects/tree/master/ym2612
const static float freq[12] = {
  //You can create your own note frequencies here. C4#-C5. There should be twelve entries.
  //YM3438 datasheet note set
  277.2, 293.7, 311.1, 329.6, 349.2, 370.0, 392.0, 415.3, 440.0, 466.2, 493.9, 523.3
}; 
const static float multiplier[] = {
  0.03125f,   0.0625f,   0.125f,   0.25f,   0.5f,   1.0f,   2.0f,   4.0f,   8.0f,   16.0f,   32.0f 
}; 
    
class Channel {
private:
  int mChannelNumber;
  int mChannelOffset;
  int mPort;
  int mNoteNumber;
  int mNoteOffset;
  int mHighFreq;
  int mLowFreq;
  int mVelocity;
  int mAlgorithm;
  bool mIsActive;
public:
  // setters
  inline void setChannelNumber(int ChannelNumber) {
    //assign channelnumber along with port, offsets and active status
    mChannelNumber = ChannelNumber;
    //if channels 1-3 (0-2) then port = 0, if channels 4-6 (3-5) then port = 1
    mPort = ChannelNumber < 3 ? 0 : 1; 
    mChannelOffset = mChannelNumber % 3;
    mNoteOffset = (mChannelNumber % 3) | (mPort << 2);
    mIsActive = true;
  }
  
  inline void setNoteNumber(int noteNumber) {
    mNoteNumber = noteNumber;

    //calculate frequency from note and assign to private members: mHighFreq & mLowFreq
    float frequency = freq[mNoteNumber%12]*multiplier[mNoteNumber/12]; //note to frequency
    int block = 2;
    while(frequency >= 2048)
    {
      frequency /= 2;
      block++;
    }
    uint16_t freq = (uint16_t )frequency;
    
    mHighFreq = ((freq >> 8) & mask(3)) | ((block & mask(3)) << 3);
    mLowFreq = freq;
  }

  inline void setVelocity(int velocity) { mVelocity = velocity; }

  //getters
  inline int getChannelNumber() { return mChannelNumber; }
  inline int getChannelOffset() { return mChannelOffset; }
  inline int getPort() { return mPort; }
  inline int getNoteNumber() { return mNoteNumber; }
  inline int getNoteOffset() { return mNoteOffset; }
  inline int getHighFrequency() { return mHighFreq; }
  inline int getLowFrequency() { return mLowFreq; }
  inline int getVelocity() { return mVelocity; }
  inline bool getIsActive() { return mIsActive; }

  //init class
  Channel() { reset(); }

  //reset members to default values
  void reset() {
    mChannelNumber = -1;
    mPort = -1;
    mNoteNumber = -1;
    mHighFreq = -1;
    mLowFreq = -1;
    mVelocity = -1; 
    mIsActive = false;
  }
};

#endif
