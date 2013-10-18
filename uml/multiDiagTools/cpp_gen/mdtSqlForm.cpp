
#include "mdtSqlForm.h"

mdtSqlForm::mdtSqlForm(const QObject & parent, const  & ) {
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

