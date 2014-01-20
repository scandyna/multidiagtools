#ifndef _MDTCLEDITOR_H
#define _MDTCLEDITOR_H


#include "mdtSqlForm.h"

class mdtSqlFormWindow;
class mdtSqlFormDialog;

class mdtClEditor : public QObject
{
  public:
  mdtClEditor(const QObject & parent, const QSqlDatabase & db);

  virtual ~mdtClEditor();

  void setupAsWindow(const QWidget & parent);

  void setupAsDialog(const QWidget & parent);


  private:
    mdtSqlForm pvForm;

    <mdtSqlFormWindow> pvFormWindow;

    <mdtSqlFormDialog> pvFormDialog;


  public:
  void show();

  int exec();


  protected:
  virtual bool setupTables() = 0;

};

#endif // #ifndef MDTCLEDITOR_H
