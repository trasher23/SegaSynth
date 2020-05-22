#include "ChannelManager.h"

Channel* ChannelManager::findFreeChannel() {
  //find inactive channel
  Channel* channel = NULL;
  for ( int i=0; i<NumberOfChannels; i++ ) {
      if ( !channels[i].getIsActive() ) {
          channels[i].setChannelNumber(i); //set channel number
          channel = &(channels[i]); //return free channel
          break;
      }
  }
  return channel;
}

Channel* ChannelManager::noteOn(int note, int velocity) {
  //assign note and velocity to available empty channel
  Channel* channel = findFreeChannel();
  if ( channel == NULL ) {
    return NULL;
  } else {
    channel->setNoteNumber(note);
    channel->setVelocity(velocity);
    return channel;
  }
}

int ChannelManager::noteOff(int note) {
  //return channel number for keyed note
  int noteOffset = -1;
  
  for (int i=0; i<NumberOfChannels; i++) {
    if ( channels[i].getIsActive() && channels[i].getNoteNumber() == note ) {
      noteOffset = channels[i].getNoteOffset();
      channels[i].reset(); //reset channel properties
      break;
    }
  }
  return noteOffset;
}
