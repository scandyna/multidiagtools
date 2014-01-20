#ifndef _MDTSQLTABLEWIDGET_H
#define _MDTSQLTABLEWIDGET_H


#include "mdtAbstractSqlWidget.h"

class mdtSqlTableWidget : public mdtAbstractSqlWidget {
  public:
    mdtSqlTableWidget(const QWidget & parent);

    ~mdtSqlTableWidget();


  private:
    QTableView pvTableView;


  public:
    virtual void doSetModel(const QSqlTableModel & model);


  private:
    virtual bool doSubmit();

    virtual bool doRevert();

    virtual bool doInsert();

    virtual bool doSubmitNewRow();

    virtual bool doRevertNewRow();

    virtual bool doRemove();

};
#endif
