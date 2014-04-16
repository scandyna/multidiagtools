#ifndef _MDTCLLINKEDUNITCONNECTIONINFODIALOG_H
#define _MDTCLLINKEDUNITCONNECTIONINFODIALOG_H


#include "mdtSortFilterProxyModel.h"

class mdtClLinkedUnitConnectionInfoDialog : public QDialog, public Ui::mdtClLinkedUnitConnectionInfoDialog
{
  public:
  mdtClLinkedUnitConnectionInfoDialog(const QWidget & parent, const QSqlDatabase & db);


  private:
    mdtSortFilterProxyModel pvProxyModel;

    QSqlQueryModel pvModel;


  public:
  void setConnections(const QVariant & unitConnectionId, const QVariant & linkedConnectionIdList);


  private:
  void displayConnection(const QVariant & connectionId);

  void displayUnit(const QVariant & unitId);

  void displayLinkedConnections(const QVariant & connectionIdList);

  QString sqlForLinkedConnections(const QVariant & connectionIdList);

};

#endif // #ifndef MDTCLLINKEDUNITCONNECTIONINFODIALOG_H
