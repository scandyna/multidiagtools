
#include "mdtSqlDialog.h"
#include "mdtAbstractSqlWidget.h"

mdtSqlDialog::mdtSqlDialog(const QWidget & parent) {
}

mdtSqlDialog::~mdtSqlDialog() {
}

void mdtSqlDialog::setSqlWidget(const mdtAbstractSqlWidget & sqlWidget) {
}

void mdtSqlDialog::addChildWidget(const QWidget & widget, const QString & label) {
}

void mdtSqlDialog::setCurrentRow(int row) {
}

bool mdtSqlDialog::setCurrentRow(const QString & fieldName, const QVariant & matchData) {
}

bool mdtSqlDialog::setCurrentRow(const QList<QPair<QString, QVariant> > & matchList) {
}

void mdtSqlDialog::insertRow() {
}

void mdtSqlDialog::accept() {
}

