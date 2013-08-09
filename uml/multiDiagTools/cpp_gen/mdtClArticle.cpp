
#include "mdtClArticle.h"

mdtClArticle::mdtClArticle(const QSqlDatabase & db) {
}

mdtClArticle::~mdtClArticle() {
}

QSqlError mdtClArticle::lastError() {
}

bool mdtClArticle::addLink(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, double value, const QVariant & directionCode, const QVariant & typeCode) {
}

bool mdtClArticle::removeLink(const QVariant & linkId) {
}

bool mdtClArticle::removeLinks(const QVariant & linkIdList) {
}

bool mdtClArticle::removeLinks(const QModelIndexList & indexListOfSelectedRows) {
}

bool mdtClArticle::addResistor(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, double value) {
}

bool mdtClArticle::addDiode(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, double Vf, const QVariant & directionCode) {
}

bool mdtClArticle::addBridge(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId,  ) {
}

