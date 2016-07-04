#include "synack.h"

Message::Message(const char *body) {
    _body = new String(body);
}

Message::Message(String body) {
    _body = new String(body);
}

String Message::getBody() {
    return *_body;
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
