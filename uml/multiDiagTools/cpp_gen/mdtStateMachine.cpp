
#include "mdtStateMachine.h"
#include "mdtState.h"

mdtStateMachine::mdtStateMachine(const QObject & parent) {
}

mdtStateMachine::~mdtStateMachine() {
}

void mdtStateMachine::addState(const mdtState & state) {
}

int mdtStateMachine::currentState() const {
}

bool mdtStateMachine::waitOnState(int state, int timeout) {
}

void mdtStateMachine::stateChanged(int id) {
}

void mdtStateMachine::stateChangedForUi(int id, const QString & text) {
}

void mdtStateMachine::onStateEntered(const mdtState & state) {
}

void mdtStateMachine::onWaitTimeout() {
}

