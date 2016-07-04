#include "synack.h"

Message::Message(const char *body) {
    _body = body;
}

Message::Message(String body) {
    _body = body;
}

String Message::getBody() {
    return _body;
}

void Message::then(messageHandler successHandler) {
    then(successHandler, NULL);
}

void Message::then(messageHandler successHandler, failureHandler failHandler) {
    _succeed = successHandler;
    _fail = failHandler;
}

void Message::succeed() {
    if (_succeed) {
        _succeed(*this);
    }
}

void Message::fail(int reason) {
    if (_fail) {
        _fail(reason);
    }
}
