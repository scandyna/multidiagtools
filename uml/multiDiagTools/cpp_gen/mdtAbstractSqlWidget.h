#ifndef _MDTABSTRACTSQLWIDGET_H
#define _MDTABSTRACTSQLWIDGET_H


class mdtSqlDataValidator;
class mdtSqlRelation;

class mdtAbstractSqlWidget : public QWidget {
  public:
    mdtAbstractSqlWidget(const QWidget & parent);

    ~mdtAbstractSqlWidget();

    virtual void setModel(const QSqlTableModel & model) = 0;

    void addDataValidator(const mdtSqlDataValidator & validator, bool putAsTopPriority);

    void addChildWidget(const mdtAbstractSqlWidget & widget, const mdtSqlRelation & relation);

    QList<mdtAbstractSqlWidget> sqlWidgets();

    virtual void submit();

    virtual void revert();

    virtual void insert();

    virtual void remove();

    virtual void toFirst() = 0;

    virtual void toLast() = 0;

    virtual void toPrevious() = 0;

    virtual void toNext() = 0;

    void submitTriggered();

    void revertTriggered();

    void insertTriggered();

    void removeTriggered();

    void errorOccured();

    void operationSucceed();

    void dataEdited();

    void currentIndexChanged(int index);


  protected:
    virtual bool doSubmit() = 0;

    virtual bool doRevert() = 0;

    virtual bool doInsert() = 0;

    virtual bool doSubmitNewRow() = 0;

    virtual bool doRevertNewRow() = 0;

    virtual bool doRemove() = 0;


  private:
    void onStateVisualizingEntered();

    void onStateEditingEntered();

    void onStateSubmittingEntered();

    void onStateRevertingEntered();

    void onStateInsertingEntered();

    void onStateSubmittingNewRowEntered();

    void onStateRevertingNewRowEntered();

    void onStateRemovingEntered();

    <mdtSqlRelation *> pvRelations;

    <mdtAbstractSqlWidget *> pvChildWidgets;

    <mdtSqlDataValidator *> pvDataValidators;

};
#endif
