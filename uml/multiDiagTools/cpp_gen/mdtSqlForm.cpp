
#include "mdtSqlForm.h"

mdtSqlForm::mdtSqlForm(const QWidget & parent, const QSqlDatabase & db) {
}

bool mdtSqlForm::setTable(const QString & tableName, const QWidget & uiWidget) {
}

bool mdtSqlForm::addChildTable(const QString & tableName) {
}

bool mdtSqlForm::addRelation(const QString & parentFieldName, const QString & childTableName, const QString & childFieldName, const  & ) {
}

bool mdtSqlForm::addRelation(const QString & parentTableName, const QString & parentFieldName, const QString & childTableName, const QString & childFieldName) {
}

mdtAbstractSqlWidget mdtSqlForm::sqlWidget(const QString & tableName) {
}

mdtSqlFormWidget mdtSqlForm::sqlFormWidget(const QString & tableName) {
}

mdtSqlTableWidget mdtSqlForm::sqlTableWidget(const QString & tableName) {
}

