#ifndef _MDTTTTESTNODEUNITDIALOG_H
#define _MDTTTTESTNODEUNITDIALOG_H


#include "mdtTtTestNodeUnitData.h"

class mdtTtTestNodeUnitDialog : public Ui::mdtTtTestNodeUnitDialog, public QDialog
{
  private:
    mdtTtTestNodeUnitData pvData;

    QSqlQueryModel pvTypeModel;


  public:
  mdtTtTestNodeUnitDialog(const QWidget & parent, const QSqlDatabase & db);

  void setData(const mdtTtTestNodeUnitData & data);

  mdtTtTestNodeUnitData data();


  private:
  void selectBaseUnit();

  void selectTestConnection();

    virtual void accept();

    virtual void reject();

  void setupBusComboBox();

  void setupTypeCpmboBox();

};

#endif // #ifndef MDTTTTESTNODEUNITDIALOG_H
