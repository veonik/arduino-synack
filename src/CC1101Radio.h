#ifndef _SYNACK_CC1101RADIO_H
#define _SYNACK_CC1101RADIO_H

#include "synack.h"
#include <cc1101.h>

class CC1101Radio : public Radio {
private:
    CC1101 _radio;
    byte _syncWord[2] = {199, 10};
    boolean _listening;
    boolean _packetWaiting;
    messageHandler _handler;

    boolean pauseListening();
    boolean resumeListening();

    void readLQI();
    void readRSSI();

public:
    CC1101Radio();
    void interrupt();

    boolean send(Message message);
    boolean listen(messageHandler handler);
    boolean stopListening();
    boolean isListening();
    void tick();
};

#endif // _SYNACK_CC1101RADIO_H
