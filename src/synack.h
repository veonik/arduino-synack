#ifndef _SYNACK_H
#define _SYNACK_H

#include <Arduino.h>

#ifdef SYNACK_DEBUG
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
    char _body[60];
    volatile MessageHandler _succeed = NULL;
    volatile FailureHandler _fail = NULL;

public:
    int rssi = 0;
    size_t size = 0;

    Message(const char *body, size_t size);
    Message(const char *body) : Message(body, strlen(body)) { }
    Message(String body) : Message(body.c_str(), body.length()) { }
    ~Message() {
        _succeed = NULL;
        _fail = NULL;
    }

    const char *getBody();

    void then(MessageHandler successHandler);
    void then(MessageHandler successHandler, FailureHandler failHandler);

    void succeed();
    void fail(FailureReason reason);
};

class Radio {
public:
    virtual bool send(Message *message) = 0;
    virtual bool listen(MessageHandler handler) = 0;
    virtual bool stopListening() = 0;
    virtual bool isListening() = 0;
    virtual void tick() = 0;
};


#endif // _SYNACK_H
