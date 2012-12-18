
#include "mdtPortTransaction.h"

mdtPortTransaction::mdtPortTransaction() {
}

mdtPortTransaction::~mdtPortTransaction() {
}

void mdtPortTransaction::setId(int id) {
}

int mdtPortTransaction::id() {
}

void mdtPortTransaction::setType(int type) {
}

int mdtPortTransaction::type() {
}

void mdtPortTransaction::setIo(const mdtAnalogIo & io, bool isInput) {
}

void mdtPortTransaction::setIo(const mdtDigitalIo & io, bool isInput) {
}

mdtAnalogIo mdtPortTransaction::analogIo() {
}

mdtDigitalIo mdtPortTransaction::digitalIo() {
}

void mdtPortTransaction::setForMultipleIos(bool forMultipleIos) {
}

bool mdtPortTransaction::forMultipleIos() {
}

bool mdtPortTransaction::setIsInput() {
}

bool mdtPortTransaction::isInput() {
}

bool mdtPortTransaction::isOutput() {
}

