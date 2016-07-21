#include "synack.h"

Message::Message(const char *body, size_t siz) {
    memcpy(_body, body, siz);
    size = siz;
}

const char *Message::getBody() {
    return _body;
}

void Message::then(MessageHandler successHandler) {
    then(successHandler, NULL);
}

void Message::then(MessageHandler successHandler, FailureHandler failHandler) {
    _succeed = successHandler;
    _fail = failHandler;
}

void Message::succeed() {
    if (_succeed != NULL) {
        _succeed(this);
    }
}

void Message::fail(FailureReason reason) {
    if (_fail != NULL) {
        _fail(reason);
    }
}
