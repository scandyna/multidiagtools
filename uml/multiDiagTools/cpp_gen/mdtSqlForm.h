#ifndef _MDTSQLFORM_H
#define _MDTSQLFORM_H


#include "mdtAbstractSqlWidget.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlTableWidget.h"

class mdtSqlForm : public QWidget {
  private:
    mdtAbstractSqlWidget pvSqlWidget;


  public:
    mdtSqlForm(const QWidget & parent, const QSqlDatabase & db);

    virtual ~mdtSqlForm();

    bool setTable(const QString & tableName, const QWidget & uiWidget);

    bool addChildTable(const QString & tableName);

    bool addRelation(const QString & parentFieldName, const QString & childTableName, const QString & childFieldName, const  & );

    bool addRelation(const QString & parentTableName, const QString & parentFieldName, const QString & childTableName, const QString & childFieldName);

    mdtAbstractSqlWidget sqlWidget(const QString & tableName);

    mdtSqlFormWidget sqlFormWidget(const QString & tableName);

    mdtSqlTableWidget sqlTableWidget(const QString & tableName);

};
#endif
