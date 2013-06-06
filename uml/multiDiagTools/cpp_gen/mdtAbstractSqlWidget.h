#ifndef _MDTABSTRACTSQLWIDGET_H
#define _MDTABSTRACTSQLWIDGET_H


class mdtAbstractSqlWidget : public QWidget {
  public:
    mdtAbstractSqlWidget(const QWidget & parent);

    ~mdtAbstractSqlWidget();

    virtual void setModel(const QSqlTableModel & model) = 0;

    virtual void submit() = 0;

    virtual void revert() = 0;

    virtual void insert() = 0;

    virtual void delete() = 0;

};
#endif
