#ifndef _MDTSQLFORM_H
#define _MDTSQLFORM_H


#include "mdtAbstractSqlWidget.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlTableWidget.h"

class mdtSqlForm : public QObject {
  private:
    mdtAbstractSqlWidget pvSqlWidget;


  public:
    mdtSqlForm(const QObject & parent, const  & );

    virtual ~mdtSqlForm();

    virtual bool setTable(const QString & tableName, const QSqlDatabase & db, const QWidget & uiWidget) = 0;

    virtual bool addChildTable(const QString & tableName, const QSqlDatabase & db) = 0;

    bool addRelation(const QString & parentFieldName, const QString & childTableName, const QString & childFieldName, const  & );

    bool addRelation(const QString & parentTableName, const QString & parentFieldName, const QString & childTableName, const QString & childFieldName);

    mdtAbstractSqlWidget sqlWidget(const QString & tableName);

    mdtSqlFormWidget sqlFormWidget(const QString & tableName);

    mdtSqlTableWidget sqlTableWidget(const QString & tableName);

};
#endif
