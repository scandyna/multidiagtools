#ifndef _MDTSQLFORMWIDGET_H
#define _MDTSQLFORMWIDGET_H


#include "mdtAbstractSqlWidget.h"

class mdtSqlFieldHandler;

class mdtSqlFormWidget : public mdtAbstractSqlWidget {
  public:
    mdtSqlFormWidget(const QWidget & parent);

    ~mdtSqlFormWidget();

    virtual void setModel(const QSqlTableModel & model);


  private:
    <mdtSqlFieldHandler> pvFieldHandlers;


  public:
    virtual void toFirst();

    virtual void toLast();

    virtual void toPrevious();

    virtual void toNext();


  private:
    virtual bool doSubmit();

    virtual bool doRevert();

    virtual bool doInsert();

    virtual bool doSubmitNewRow();

    virtual bool doRevertNewRow();

    virtual bool doRemove();

    QDataWidgetMapper pvWidgetMapper;

};
#endif
