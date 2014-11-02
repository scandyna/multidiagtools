/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "mdtTtLogicalTestCableTsWidget.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include "mdtClConnectableConnectorDialog.h"
#include "mdtClLink.h"
#include "mdtClUnit.h"
#include "mdtError.h"
#include <QStringList>
#include <QVariant>
#include <QMessageBox>
#include <QList>
#include <QSqlRecord>

mdtTtLogicalTestCableTsWidget::mdtTtLogicalTestCableTsWidget(QWidget* parent, QSqlDatabase db)
 : QGroupBox(parent),
   pvDatabase(db)
{
  setupUi(this);
  connect(pbSelectSystem, SIGNAL(clicked()), this, SLOT(selectTestSystem()));
  connect(pbSelectCn, SIGNAL(clicked()), this, SLOT(selectCn()));
  lbSystem->clear();
  lbNode->clear();
  lbCnName->clear();
}

void mdtTtLogicalTestCableTsWidget::setTestCableConnector(const QVariant& connectorId, const QString& name)
{
  pvTestCableCnId = connectorId;
  pvCnType = mdtTtLogicalTestCableDialog::Connector;
  lbTestCableCnLabel->setText(tr("Test cable connector:"));
  lbTestCableCn->setText(name);
  lbCnLabel->setText(tr("Connector:"));
}

void mdtTtLogicalTestCableTsWidget::setTestCableConnection(const QVariant& connectionId, const QString& name)
{
  pvTestCableCnId = connectionId;
  pvCnType = mdtTtLogicalTestCableDialog::Connection;
  lbTestCableCnLabel->setText(tr("Test cable connection:"));
  lbTestCableCn->setText(name);
  lbCnLabel->setText(tr("Connection:"));
}

void mdtTtLogicalTestCableTsWidget::setTestSystem(const QVariant& tsVtId)
{
  mdtClUnit unit(0, pvDatabase);
  QList<QSqlRecord> dataList;
  QString sql;
  bool ok;

  pvTestSystemId = tsVtId;
  if(pvTestSystemId.isNull()){
    lbSystem->clear();
    lbNode->clear();
  }else{
    sql = "SELECT Type, SubType FROM TestNode_view WHERE VehicleType_Id_FK_PK = " + pvTestSystemId.toString();
    dataList = unit.getDataList<QSqlRecord>(sql, ok);
    if(ok && (!dataList.isEmpty())){
      Q_ASSERT(dataList.size() == 1);
      lbSystem->setText(dataList.at(0).value("Type").toString());
      lbNode->setText(dataList.at(0).value("SubType").toString());
    }else{
      lbSystem->setText("<Error>");
      lbNode->setText("<Error>");
    }
  }
  // Update CN
  pvCnId.clear();
  lbCnName->clear();
}

QList< QVariant > mdtTtLogicalTestCableTsWidget::getAffectedTsConnections(bool& ok)
{
  mdtClUnit unit(0, pvDatabase);
  QList<QVariant> cnIdList;
  QString sql;

  // If type is a connection, simply return it
  if(pvCnType == mdtTtLogicalTestCableDialog::Connection){
    cnIdList.append(pvCnId);
    return cnIdList;
  }
  // Here we have a connector
  Q_ASSERT(pvCnType == mdtTtLogicalTestCableDialog::Connector);
  sql = "SELECT Id_PK FROM UnitConnection_tbl WHERE UnitConnector_Id_FK = " + pvCnId.toString();
  cnIdList = unit.getDataList<QVariant>(sql, ok);
  if(!ok){
    displayError(unit.lastError());
  }

  return cnIdList;
}

bool mdtTtLogicalTestCableTsWidget::containsTestCableConnection(const QVariant& testCableConnectionId, bool& ok)
{
  mdtClUnit unit(0, pvDatabase);
  QList<QVariant> tcConnectionIdList;
  QString sql;
  int i;

  // If type is a connection, simply check on it
  if(pvCnType == mdtTtLogicalTestCableDialog::Connection){
    return (pvCnId == testCableConnectionId);
  }
  // Here we have a connector
  Q_ASSERT(pvCnType == mdtTtLogicalTestCableDialog::Connector);
  sql = "SELECT Id_PK FROM UnitConnection_tbl WHERE UnitConnector_Id_FK = " + pvTestCableCnId.toString();
  tcConnectionIdList = unit.getDataList<QVariant>(sql, ok);
  if(!ok){
    displayError(unit.lastError());
    return false;
  }
  // Search...
  for(i = 0; i < tcConnectionIdList.size(); ++i){
    if(tcConnectionIdList.at(i) == testCableConnectionId){
      return true;
    }
  }

  return false;
}

void mdtTtLogicalTestCableTsWidget::selectTestSystem()
{
  mdtSqlSelectionDialog selectionDialog(this);
  mdtSqlTableSelection s;
  QString sql;

  // Build SQL query
  sql = "SELECT * FROM TestNode_view";
  // Setup and show dialog
  selectionDialog.setMessage(tr("Select test system:"));
  selectionDialog.setQuery(sql, pvDatabase, false);
  selectionDialog.setColumnHidden("VehicleType_Id_FK_PK", true);
  selectionDialog.setHeaderData("Type", tr("Test system"));
  selectionDialog.setHeaderData("SubType", tr("Node or group"));
  selectionDialog.addColumnToSortOrder("Type", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("SubType", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.setWindowTitle(tr("Test system selection"));
  selectionDialog.resize(750, 500);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  // Store selected test system ID
  s = selectionDialog.selection("VehicleType_Id_FK_PK");
  if(s.isEmpty()){
    return;
  }
  Q_ASSERT(s.rowCount() == 1);
  setTestSystem(s.data(0, "VehicleType_Id_FK_PK"));
}

void mdtTtLogicalTestCableTsWidget::selectCn()
{
  // Check that test system was selected
  if(pvTestSystemId.isNull()){
    return;
  }
  // Check that test cable CN was set
  if(pvTestCableCnId.isNull()){
    return;
  }
  // Call selection method regarding CN type
  switch(pvCnType){
    case mdtTtLogicalTestCableDialog::Connector:
      selectTsConnector();
      break;
    case mdtTtLogicalTestCableDialog::Connection:
      selectTsConnection();
      break;
  }
}

void mdtTtLogicalTestCableTsWidget::selectTsConnector()
{
  mdtClConnectableConnectorDialog dialog(this, pvDatabase);
  QList<QVariant> tsUnitIdList;
  bool ok;

  // Get list of units that are test connectors
  tsUnitIdList = getTestConnectorUnitIdList(ok);
  if(!ok){
    return;
  }
  // Setup and show connectable connectors dialog
  dialog.setStartConnectorLabel(tr("Test cable connector"));
  dialog.setEndConnectorLabel(tr("Test system connector"));
  dialog.setStartConnector(pvTestCableCnId);
  dialog.setEndUnitIdList(tsUnitIdList);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  pvCnId = dialog.endUnitConnectorId();
  lbCnName->setText(dialog.endUnitConnectorName());
}

void mdtTtLogicalTestCableTsWidget::selectTsConnection()
{
  QMessageBox msgBox(this);

  msgBox.setText(tr("This functionnality is not implemented yet."));
  msgBox.setIcon(QMessageBox::Critical);
  msgBox.exec();
}

QList< QVariant > mdtTtLogicalTestCableTsWidget::getTestConnectorUnitIdList(bool& ok)
{
  mdtClLink lnk(0, pvDatabase);
  QList<QVariant> tsUnitIdList;
  QString sql;

  // Get all unit IDs of pvTestSystemId that are a test connector
  sql = "SELECT Unit_Id_FK_PK FROM TestNodeUnit_view";
  sql += " WHERE TestNode_Id_FK = " + pvTestSystemId.toString();
  sql += " AND Type_Code_FK = 'TESTCONNECTOR'";
  tsUnitIdList = lnk.getDataList<QVariant>(sql, ok);
  if(!ok){
    displayError(lnk.lastError());
  }

  return tsUnitIdList;
}

void mdtTtLogicalTestCableTsWidget::displayError(const mdtError& error)
{
  QMessageBox msgBox(this);

  msgBox.setText(error.text());
  msgBox.setInformativeText(error.informativeText());
  msgBox.setDetailedText(error.systemText());
  msgBox.setIcon(error.levelIcon());
  msgBox.exec();
}
