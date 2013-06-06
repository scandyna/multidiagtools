#ifndef _MDTSQLFORMWIDGET_H
#define _MDTSQLFORMWIDGET_H


#include "mdtAbstractSqlWidget.h"

class mdtSqlFormWidget : public mdtAbstractSqlWidget {
  public:
    mdtSqlFormWidget(const QWidget & parent);

    ~mdtSqlFormWidget();

    virtual void setModel(const QSqlTableModel & model);

    void setForm(const QWidget & form);

    virtual void submit();

    virtual void revert();

    virtual void insert();

    virtual void delete();


  private:
    mdtSqlDataWidgetMapper pvWidgetMapper;

};
#endif
