
#include "mdtSqlRelation.h"
#include "mdtSqlRelationItem.h"

mdtSqlRelation::mdtSqlRelation(const QObject & parent) {
}

mdtSqlRelation::~mdtSqlRelation() {
}

void mdtSqlRelation::setParentModel(const QSqlTableModel & model) {
}

void mdtSqlRelation::setChildModel(const QSqlTableModel & model) {
}

void mdtSqlRelation::addRelation(const QString & parentFieldName, const QString & childFieldName) {
}

void mdtSqlRelation::clear() {
}

void mdtSqlRelation::setParentCurrentIndex(int index) {
}

void mdtSqlRelation::setParentCurrentIndex(const QModelIndex & index) {
}

void mdtSqlRelation::setParentCurrentIndex(const QModelIndex & current, const QModelIndex & previous) {
}

void mdtSqlRelation::generateChildModelRelationFilter() {
}

void mdtSqlRelation::generateChildModelFilter() {
}

