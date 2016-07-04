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

typedef void (*messageHandler)(Message message);
typedef void (*failureHandler)(int reason);

class Message {
private:
    String _body;
    volatile messageHandler _succeed;
    volatile failureHandler _fail;

public:
    Message(String body);
    Message(const char *body);

    String getBody();

    void then(messageHandler successHandler);
    void then(messageHandler successHandler, failureHandler failHandler);

    void succeed();
    void fail(int reason);
};

class Radio {
public:
    virtual boolean send(Message message);
    virtual boolean listen(messageHandler handler);
    virtual boolean stopListening();
    virtual boolean isListening();
    virtual void tick();
};


#endif // _SYNACK_H
