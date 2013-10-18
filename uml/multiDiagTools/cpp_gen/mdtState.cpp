
#include "mdtState.h"

mdtState::mdtState(int id, const QState & parent) {
}

mdtState::~mdtState() {
}

void mdtState::setId(int id) {
}

int mdtState::id() const {
}

void mdtState::setText(const QString & text) {
}

QString mdtState::text() const {
}

void mdtState::setLedColorId(int colorId) {
}

int mdtState::ledColorId() const {
}

void mdtState::setLedOn(bool state) {
}

bool mdtState::ledOn() {
}

void mdtState::setNotifyEnteredToUi(bool notify) {
}

bool mdtState::notifyEnteredToUi() const {
}

void mdtState::entered(const mdtState & state) {
}

void mdtState::onStateEntered() {
}

