#ifndef _MDTCLLINKEDUNITCONNECTORINFODIALOG_H
#define _MDTCLLINKEDUNITCONNECTORINFODIALOG_H


#include "mdtSqlTableWidget.h"

class mdtClLinkedUnitConnectorInfoDialog : public QDialog, private Ui::mdtClLinkedUnitConnectorInfoDialog
{
  public:
  mdtClLinkedUnitConnectorInfoDialog(const QWidget & parent, const QSqlDatabase & db);

  void setConnectors(const QVariant & unitConnectorId, const QVariant & linkedConnectionIdList);


  private:
    mdtSqlTableWidget pvLinkedConnectorsWidget;

    mdtSqlTableWidget pvLinkedConnectionsWidget;

  void displayUnit();

  void displayConnector();

  void displayLinkedConnections();

  void setupConnectorTable();

  void setupLinkedConnectorsTable();

  void displayError(const mdtError & error);

};

#endif // #ifndef MDTCLLINKEDUNITCONNECTORINFODIALOG_H
