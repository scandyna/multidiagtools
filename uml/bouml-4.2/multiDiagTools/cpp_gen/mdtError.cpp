
#include "mdtError.h"
#include "mdtErrorOut.h"

 mdtError::mdtError(int number, const QString & text, const level_t & level) {
}

void mdtError::setSystemError(int number, const QString & text) {
}

void mdtError::setSource(const QString & fileName, int fileLine, const QString & className, const QString & function) {
}

void mdtError::commit() {
}

int mdtError::number() {
}

QString mdtError::text() {
}

level_t mdtError::level() {
}

int mdtError::systemNumber() {
}

QString mdtError::systemText() {
}

QString mdtError::functionName() {
}

QString mdtError::fileName() {
}

int mdtError::fileLine() {
}

