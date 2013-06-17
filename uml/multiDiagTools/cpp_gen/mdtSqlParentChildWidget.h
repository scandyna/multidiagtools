#ifndef _MDTSQLPARENTCHILDWIDGET_H
#define _MDTSQLPARENTCHILDWIDGET_H


class mdtAbstractSqlWidget;
class mdtSqlRelation;

class mdtSqlParentChildWidget : public QWidget {
  public:
    mdtSqlParentChildWidget(const QWidget & parent);

    ~mdtSqlParentChildWidget();


  private:
    mdtAbstractSqlWidget * pvParentWidget;

    <mdtAbstractSqlWidget *> pvChildWidgets;


  public:
    void setParentWidget(const mdtAbstractSqlWidget & widget);


  private:
    <mdtSqlRelation *> pvRelations;


  public:
    void addChildWidget(const mdtAbstractSqlWidget & widget, const mdtSqlRelation & relation);

};
#endif
