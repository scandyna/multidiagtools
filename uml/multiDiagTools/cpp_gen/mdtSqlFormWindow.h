#ifndef _MDTSQLFORMWINDOW_H
#define _MDTSQLFORMWINDOW_H


#include "mdtSqlWindow.h"
#include "mdtSqlForm.h"

class mdtSqlFormWindow : public mdtSqlWindow, public mdtSqlForm {
  public:
    mdtSqlFormWindow(const QWidget & parent);

    ~mdtSqlFormWindow();

};
#endif
