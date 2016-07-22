#ifndef _SYNACK_CC1101RADIO_H
#define _SYNACK_CC1101RADIO_H

#include "synack.h"
#include <cc1101.h>

#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)
#define CC1101Interrupt 4 // Pin 19
#else
#define CC1101Interrupt 0 // Pin 2
#endif

class CC1101Radio : public Radio {
private:
    CC1101 _radio;
    byte _syncWord[2] = {199, 10};
    bool _listening;
    bool _packetWaiting;
    MessageHandler _handler;

    bool pauseListening();
    bool resumeListening();

public:
    CC1101Radio();
    void interrupt();

    bool send(Message *message);
    bool listen(MessageHandler handler);
    bool stopListening();
    bool isListening();
    void tick();

};

#endif // _SYNACK_CC1101RADIO_H
