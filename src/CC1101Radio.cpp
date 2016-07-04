#include "CC1101Radio.h"

CC1101Radio::CC1101Radio() {
    _radio.init();
    _radio.setSyncWord(_syncWord);
    _radio.setCarrierFreq(CFREQ_433);
    // TODO: addresses and channels
    _radio.disableAddressCheck();
    _radio.setTxPowerAmp(PA_LowPower);

    debug("CC1101_PARTNUM "); //radio=0
    debugln(_radio.readReg(CC1101_PARTNUM, CC1101_STATUS_REGISTER));
    debug("CC1101_VERSION "); //radio=4
    debugln(_radio.readReg(CC1101_VERSION, CC1101_STATUS_REGISTER));
    debug("CC1101_MARCSTATE ");
    debugln(_radio.readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) & 0x1f);

    debugln("CC1101 radio initialized.");
}

CCPACKET messageToPacket(Message message) {
    CCPACKET packet;
    String body = message.getBody();
    packet.length = body.length();
    for (int i = 0; i < packet.length; i++) {
        packet.data[i] = (byte) body[i];
    }
    return packet;
}

Message packetToMessage(CCPACKET packet) {
    String body;
    for (int i = 0; i < packet.length; i++) {
        body += (char) packet.data[i];
    }
    return Message(body);
}

boolean CC1101Radio::send(Message message) {
    boolean res;
    if (isListening()) {
        pauseListening();
        res = _radio.sendData(messageToPacket(message));
        resumeListening();
    } else {
        res = _radio.sendData(messageToPacket(message));
    }
    if (res) {
        message.succeed();
    } else {
        message.fail(0);
    }
    return res;
}

boolean CC1101Radio::listen(messageHandler handler) {
    _handler = handler;
    _listening = true;
    return resumeListening();
}

boolean CC1101Radio::stopListening() {
    _handler = NULL;
    _listening = false;
    return pauseListening();
}

boolean CC1101Radio::pauseListening() {
    detachInterrupt(0);
    return true;
}

void CC1101Radio::interrupt(void) {
    _packetWaiting = true;
}

typedef void (*userFunc)(void);

CC1101Radio *radios[1];

void interruptFn0(void) {
    if (radios[0]) {
        CC1101Radio *radio = radios[0];
        radio->interrupt();
    }
}

userFunc getInterruptFn(CC1101Radio *radio, int index) {
    switch (index) {
        case 0:
            radios[0] = radio;
            return interruptFn0;
    }
}

boolean CC1101Radio::resumeListening() {
    attachInterrupt(0, getInterruptFn(this, 0), FALLING);
    return true;
}

boolean inline CC1101Radio::isListening() {
    return _listening;
}

void CC1101Radio::readLQI()
{
    byte lqi=0;
    byte value=0;
    lqi=(_radio.readReg(CC1101_LQI, CC1101_STATUS_REGISTER));
    value = 0x3F - (lqi & 0x3F);
    debug("CC1101_LQI ");
    debugln(value);
}

void CC1101Radio::readRSSI()
{
    byte rssi=0;
    byte value=0;

    rssi=(_radio.readReg(CC1101_RSSI, CC1101_STATUS_REGISTER));

    if (rssi >= 128)
    {
        value = 255 - rssi;
        value /= 2;
        value += 74;
    }
    else
    {
        value = rssi/2;
        value += 74;
    }
    debug("CC1101_RSSI ");
    debugln(value);
}

void CC1101Radio::tick() {
    if (_packetWaiting) {
        debugln("packet received");

        pauseListening();
        _packetWaiting = false;

        CCPACKET packet;

        unsigned long time = micros();
        if(_radio.receiveData(&packet) > 0){
            debugln("wot2");
            if(!packet.crc_ok) {
                debugln("crc not ok");
            }

            if(packet.length > 0){
                debug("packet: len ");
                debug(packet.length);
                debug(" data: ");
                for (int j = 0; j < packet.length; j++) {
                    debug(packet.data[j]);
                    debug(" ");
                }
                debugln();
                if (_listening) {
                    _handler(packetToMessage(packet));
                }
            }
        } else {
            debugln("else wot");
        }
        resumeListening();
    }
}









