#ifndef _MDTSQLFORMDIALOG_H
#define _MDTSQLFORMDIALOG_H


#include "mdtSqlForm.h"
#include "mdtSqlDialog.h"

class mdtAbstractSqlWidget;

class mdtSqlFormDialog : public mdtSqlForm {
  private:
    mdtSqlDialog pvDialog;


  public:
    mdtSqlFormDialog(const QObject & parent);

    ~mdtSqlFormDialog();

    mdtSqlDialog sqlDialog();

    void show();


  private:
     addChildWidget(const mdtAbstractSqlWidget & widget);

};
#endif
