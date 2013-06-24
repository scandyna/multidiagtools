#ifndef _MDTCLUNITEDITOR_H
#define _MDTCLUNITEDITOR_H


#include "mdtSqlFormWidget.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlRelation.h"

class mdtSqlWindow;

class mdtClUnitEditor {
  private:
    Ui::mdtClUnitEditor pvUiUnitWidget;

    mdtSqlFormWidget pvUintWidget;

    mdtSqlTableWidget pvConnectionWidget;

    mdtSqlRelation pvRelation;


  public:
    mdtClUnitEditor(const QSqlDatabase & db);

    ~mdtClUnitEditor();

    bool setupTables(bool includeConnections);

    void setupUi(const mdtSqlWindow & window);


  private:
    QSqlDatabase * pvDatabase;

};
#endif
