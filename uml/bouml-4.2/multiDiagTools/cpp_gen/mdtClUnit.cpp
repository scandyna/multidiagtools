
#include "mdtClUnit.h"

mdtClUnit::mdtClUnit(const QSqlDatabase & db) {
}

mdtClUnit::~mdtClUnit() {
}

QSqlError mdtClUnit::lastError() {
}

int mdtClUnit::toUnitRelatedArticleConnectionCount(const QVariant & unitId) {
}

QList<QVarian> mdtClUnit::toUnitRelatedArticleConnectionIds(const QVariant & unitId) {
}

bool mdtClUnit::unitConnectionIsRelatedToArticleConnection(const QVariant & unitConnectionId) {
}

QVariant mdtClUnit::toUnitConnectionRelatedArticleConnectionData(const QVariant & unitConnectionId, const QString & field) {
}

bool mdtClUnit::addUnitConnection(const mdtClUnitConnectionData & data) {
}

bool mdtClUnit::removeUnitConnection(const QVariant & unitConnectionId) {
}

bool mdtClUnit::editUnitConnection(const mdtClUnitConnectionData & data) {
}

mdtClUnitConnectionData mdtClUnit::getUnitConnectionData(const QVariant & unitConnectionId) {
}

QSqlQueryModel mdtClUnit::unitConnectionRangesModel(const QVariant & unitConnectionId) {
}

bool mdtClUnit::addUnitConnectionRange(const QVariant & baseUnitConnectionId, const mdtClUnitConnectionData & rangeData) {
}

bool mdtClUnit::updateUnitConnectionRanges(const QVariant & baseUnitConnectionId, const QStandardItemModel & ranges) {
}

bool mdtClUnit::removeUnitConnectionRange(const QVariant & unitConnectionId) {
}

