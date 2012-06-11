
#include "mdtFileCopier.h"
#include "mdtFileCopierItem.h"

 mdtFileCopier::mdtFileCopier(const QObject & parent) {
}

 mdtFileCopier::~mdtFileCopier() {
}

void mdtFileCopier::setShowProgressDialog(bool show) {
}

void mdtFileCopier::setDirectDestOverwrite(bool directOverwrite) {
}

void mdtFileCopier::diseableAllDialogs(bool diseable) {
}

void mdtFileCopier::setDefaultSetup() {
}

void mdtFileCopier::addCopy(const QString & srcFilePath, const QString & destFilePath, bool syncAfterCopy, bool checkAfterCopy) {
}

void mdtFileCopier::startCopy() {
}

bool mdtFileCopier::waitFinished() {
}

void mdtFileCopier::finished(bool success) {
}

bool mdtFileCopier::sync() {
}

void mdtFileCopier::run() {
}

