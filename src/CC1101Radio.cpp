#include "CC1101Radio.h"

// TODO: Support more than a single radio.
// Defines a pointer to an interrupt callback function.
typedef void (*__interruptFunc)(void);
CC1101Radio *__radioInterrupts[1];
void __interruptNoop(void) {}
void __interruptRadio0(void) {
    if (__radioInterrupts[0]) {
        CC1101Radio *radio = __radioInterrupts[0];
        radio->interrupt();
    }
}
// Returns a radio-specific interrupt callback. This is necessary
// because we can't pass pointer-to-methods where Arduino expects
// a pointer-to-function.
__interruptFunc getInterruptFunc(CC1101Radio *radio, int index) {
    switch (index) {
        case 0:
            __radioInterrupts[0] = radio;
            return __interruptRadio0;
        default:
            return __interruptNoop;
    }
}




CC1101Radio::CC1101Radio() {
    _radio.init();
    _radio.setSyncWord(_syncWord);
    _radio.setCarrierFreq(CFREQ_433);
    // TODO: addresses and channels
    _radio.disableAddressCheck();
    // TODO: transmission power customization
    _radio.setTxPowerAmp(PA_LongDistance);

    debug(F("CC1101_PARTNUM "));
    debugln(_radio.readReg(CC1101_PARTNUM, CC1101_STATUS_REGISTER));
    debug(F("CC1101_VERSION "));
    debugln(_radio.readReg(CC1101_VERSION, CC1101_STATUS_REGISTER));
    debug(F("CC1101_MARCSTATE "));
    debugln(_radio.readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) & 0x1f);

    debugln(F("CC1101 radio initialized."));
    resumeListening();
}

CCPACKET messageToPacket(Message *message) {
    CCPACKET packet;
    packet.length = (unsigned char) message->size;
    memcpy(&packet.data, message->getBody(), packet.length);
    return packet;
}

Message packetToMessage(CCPACKET packet) {
    return Message((char *)packet.data, packet.length);
}

bool CC1101Radio::send(Message *message) {
    bool res;
    if (isListening()) {
        pauseListening();
        res = _radio.sendData(messageToPacket(message));
        resumeListening();
    } else {
        res = _radio.sendData(messageToPacket(message));
    }
    if (res) {
        message->succeed();
    } else {
        message->fail(SYNACK_SEND_ERROR);
    }
    return res;
}

bool CC1101Radio::listen(MessageHandler handler) {
    _handler = handler;
    return resumeListening();
}

bool CC1101Radio::stopListening() {
    _handler = NULL;
    return pauseListening();
}

bool CC1101Radio::pauseListening() {
    detachInterrupt(CC1101Interrupt);
    _listening = false;
    return true;
}

void CC1101Radio::interrupt(void) {
    _packetWaiting = true;
}

bool CC1101Radio::resumeListening() {
    attachInterrupt(CC1101Interrupt, getInterruptFunc(this, 0), FALLING);
    _listening = true;
    return true;
}

bool CC1101Radio::isListening() {
    return _listening;
}

// Get signal strength indicator in dBm.
// See: http://www.ti.com/lit/an/swra114d/swra114d.pdf
int rssi(char raw) {
    uint8_t rssi_dec;
    // TODO: This rssi_offset is dependent on baud and MHz; this is for 38.4kbps and 433 MHz.
    uint8_t rssi_offset = 74;
    rssi_dec = (uint8_t) raw;
    if (rssi_dec >= 128)
        return ((int)( rssi_dec - 256) / 2) - rssi_offset;
    else
        return (rssi_dec / 2) - rssi_offset;
}

#ifdef SYNACK_DEBUG
// Get link quality indicator.
int lqi(char raw) {
    return 0x3F - raw;
}
#endif

void CC1101Radio::tick() {
    if (_packetWaiting) {
        pauseListening();
        _packetWaiting = false;

        CCPACKET packet;
        if (_radio.receiveData(&packet) > 0) {
#ifdef SYNACK_DEBUG
            if (!packet.crc_ok) {
                debugln("crc not ok");
            }
            debug(F("lqi: "));
            debugln(lqi(packet.lqi));
            debug(F("rssi: "));
            debug(rssi(packet.rssi));
            debugln(F("dBm"));
#endif
            if (packet.crc_ok && packet.length > 0) {
#ifdef SYNACK_DEBUG
                debug(F("packet: len "));
                debugln(packet.length);
#endif
                if (_handler) {
                    Message msg = packetToMessage(packet);
                    msg.rssi = rssi(packet.rssi);
                    _handler(&msg);
                }
            }
        }

        resumeListening();
    }
}










