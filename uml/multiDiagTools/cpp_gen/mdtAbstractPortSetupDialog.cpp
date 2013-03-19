
#include "mdtAbstractPortSetupDialog.h"

mdtAbstractPortSetupDialog::mdtAbstractPortSetupDialog(const QWidget & parent) {
}

mdtAbstractPortSetupDialog::~mdtAbstractPortSetupDialog() {
}

void mdtAbstractPortSetupDialog::setPortManager(const mdtPortManager & portManager) {
}

mdtPortManager mdtAbstractPortSetupDialog::portManager() {
}

void mdtAbstractPortSetupDialog::setStateFromPortManager(int state) {
}

void mdtAbstractPortSetupDialog::showStatusMessage(const QString & message, int timeout,  ) {
}

void mdtAbstractPortSetupDialog::showStatusMessage(const QString & message, const QString & details, int timeout) {
}

void mdtAbstractPortSetupDialog::on_buttonBox_clicked(const QAbstractButton & button) {
}

