#ifndef _MDTCLUNITEDITOR_H
#define _MDTCLUNITEDITOR_H


#include "mdtSqlRelation.h"
#include "mdtSqlFormWindow.h"

class mdtSqlWindow;

class mdtClUnitEditor : public QObject {
  private:
    Ui::mdtClUnitEditor pvUiUnitWidget;

    mdtSqlRelation pvRelation;


  public:
    mdtClUnitEditor(const QSqlDatabase & db);

    ~mdtClUnitEditor();

    bool setupTables(bool includeConnections);

    void setupUi(const mdtSqlWindow & window);


  private:
    QSqlDatabase * pvDatabase;

    mdtSqlFormWindow pvForm;

};
#endif
