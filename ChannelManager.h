#ifndef CHANNELMANAGER_H
#define CHANNELMANAGER_H

#include <Arduino.h>
#include "Channel.h"

class ChannelManager {
private:
  static const int NumberOfChannels = 6;
  Channel channels[NumberOfChannels];
public:
  Channel* findFreeChannel();
  Channel* noteOn(int note, int velocity);
  int noteOff(int note);
};

#endif
