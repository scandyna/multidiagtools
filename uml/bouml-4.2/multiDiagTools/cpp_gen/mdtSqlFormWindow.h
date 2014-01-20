#ifndef _MDTSQLFORMWINDOW_H
#define _MDTSQLFORMWINDOW_H


#include "mdtSqlWindow.h"
#include "mdtSqlForm.h"

class mdtAbstractSqlWidget;

class mdtSqlFormWindow : public mdtSqlForm {
  public:
    mdtSqlFormWindow(const QObject & parent);

    ~mdtSqlFormWindow();

    mdtSqlWindow sqlWindow();


  private:
    mdtSqlWindow pvWindow;


  public:
    void show();


  private:
     addChildWidget(const mdtAbstractSqlWidget & widget);

};
#endif
