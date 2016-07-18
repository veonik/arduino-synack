#ifndef _SYNACK_H
#define _SYNACK_H

#include <Arduino.h>

#ifdef DEBUG
#define debug(message) Serial.print(message);
#define debugln(message) Serial.println(message);
#else
#define debug(message); // no op
#define debugln(message); // no op
#endif

class Message;
class Radio;

enum FailureReason {SYNACK_SEND_ERROR, SYNACK_TIMEOUT};

typedef void (*MessageHandler)(Message *message);
typedef void (*FailureHandler)(FailureReason reason);

class Message {
private:
    String *_body;
    volatile MessageHandler _succeed = NULL;
    volatile FailureHandler _fail = NULL;

public:
    int rssi = 0;

    Message(String body);
    Message(const char *body);
    ~Message() {
        delete _body;
        _succeed = NULL;
        _fail = NULL;
    }

    String getBody();

    void then(MessageHandler successHandler);
    void then(MessageHandler successHandler, FailureHandler failHandler);

    void succeed();
    void fail(FailureReason reason);
};

class Radio {
public:
    virtual boolean send(Message *message);
    virtual boolean listen(MessageHandler handler);
    virtual boolean stopListening();
    virtual boolean isListening();
    virtual void tick();
};


#endif // _SYNACK_H
