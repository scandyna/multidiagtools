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
#include "mdtTtLogicalTestCableDutWidget.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include "mdtClConnectableConnectorDialog.h"
#include "mdtClLink.h"
#include "mdtClUnit.h"
#include "mdtError.h"
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QMessageBox>
#include <QSqlRecord>
#include <QCheckBox>

mdtTtLogicalTestCableDutWidget::mdtTtLogicalTestCableDutWidget(QWidget* parent, QSqlDatabase db)
 : QGroupBox(parent),
   pvDatabase(db)
{
  setupUi(this);
  connect(pbSelectDut, SIGNAL(clicked()), this, SLOT(selectDut()));
  connect(pbSelectCn, SIGNAL(clicked()), this, SLOT(selectCn()));
  lbSchemaPosition->clear();
  lbAlias->clear();
  lbCnName->clear();
}

void mdtTtLogicalTestCableDutWidget::setTestCableConnector(const QVariant& connectorId, const QString& name)
{
  pvTestCableCnId = connectorId;
  ///pvCnType = Connector;
  pvCnType = mdtTtLogicalTestCableDialog::Connector;
  lbTestCableCnLabel->setText(tr("Test cable connector:"));
  lbTestCableCn->setText(name);
  lbCnLabel->setText(tr("Connector:"));
}

void mdtTtLogicalTestCableDutWidget::setTestCableConnection(const QVariant& connectionId, const QString& name)
{
  pvTestCableCnId = connectionId;
  ///pvCnType = Connection;
  pvCnType = mdtTtLogicalTestCableDialog::Connection;
  lbTestCableCnLabel->setText(tr("Test cable connection:"));
  lbTestCableCn->setText(name);
  lbCnLabel->setText(tr("Connection:"));
}

void mdtTtLogicalTestCableDutWidget::setDutUnit(const QVariant& unitId)
{
  mdtClUnit unit(0, pvDatabase);
  QList<QSqlRecord> dataList;
  QString sql;
  bool ok;

  pvUnitId = unitId;
  if(pvUnitId.isNull()){
    lbSchemaPosition->clear();
    lbAlias->clear();
  }else{
    sql = "SELECT SchemaPosition, Alias FROM Unit_tbl WHERE Id_PK = " + pvUnitId.toString();
    dataList = unit.getDataList<QSqlRecord>(sql, ok);
    if(ok && (!dataList.isEmpty())){
      Q_ASSERT(dataList.size() == 1);
      lbSchemaPosition->setText(dataList.at(0).value("SchemaPosition").toString());
      lbAlias->setText(dataList.at(0).value("Alias").toString());
    }else{
      lbSchemaPosition->setText("<Error>");
      lbAlias->setText("<Error>");
    }
  }
  // Update CN
  pvCnId.clear();
  lbCnName->clear();
}

bool mdtTtLogicalTestCableDutWidget::containsTestCableConnection(const QVariant& testCableConnectionId, bool& ok)
{
  mdtClUnit unit(0, pvDatabase);
  QList<QVariant> tcConnectionIdList;
  QString sql;
  int i;

  // If type is a connection, simply check on it
  if(pvCnType == mdtTtLogicalTestCableDialog::Connection){
    return (pvTestCableCnId == testCableConnectionId);
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

QList< QVariant > mdtTtLogicalTestCableDutWidget::getAffectedDutConnections(bool& ok)
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

void mdtTtLogicalTestCableDutWidget::selectDut()
{
  mdtSqlSelectionDialog selectionDialog(this);
  mdtSqlTableSelection s;
  QString sql;

  // Build SQL query
  sql = "SELECT * FROM Unit_view";
  // Setup and show dialog
  selectionDialog.setMessage(tr("Select DUT unit:"));
  selectionDialog.setQuery(sql, pvDatabase, false);
  selectionDialog.setColumnHidden("Unit_Id_PK", true);
  selectionDialog.setHeaderData("SchemaPosition", tr("Schema\nposition"));
  selectionDialog.addColumnToSortOrder("Type", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("SubType", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("SchemaPosition", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.setWindowTitle(tr("DUT selection"));
  selectionDialog.resize(700, 500);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  // Store selected DUT unit ID
  s = selectionDialog.selection("Unit_Id_PK");
  if(s.isEmpty()){
    return;
  }
  Q_ASSERT(s.rowCount() == 1);
  setDutUnit(s.data(0, "Unit_Id_PK"));
}

void mdtTtLogicalTestCableDutWidget::selectCn()
{
  // Check that DUT unit was selected
  if(pvUnitId.isNull()){
    return;
  }
  // Check that test cable CN was set
  if(pvTestCableCnId.isNull()){
    return;
  }
  // Call selection method regarding CN type
  switch(pvCnType){
    case mdtTtLogicalTestCableDialog::Connector:
      selectDutConnector();
      break;
    case mdtTtLogicalTestCableDialog::Connection:
      selectDutConnection();
      break;
  }
}

void mdtTtLogicalTestCableDutWidget::selectDutConnector()
{
  mdtClConnectableConnectorDialog dialog(this, pvDatabase);

  dialog.setStartConnectorLabel(tr("Test cable connector"));
  dialog.setEndConnectorLabel(tr("DUT connector"));
  dialog.setStartConnector(pvTestCableCnId);
  dialog.setEndUnitId(pvUnitId);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  pvCnId = dialog.endUnitConnectorId();
  lbCnName->setText(dialog.endUnitConnectorName());
  /**
  mdtClLink lnk(0, pvDatabase);
  mdtSqlSelectionDialog selectionDialog(this);
  mdtSqlTableSelection s;
  mdtClConnectableCriteria c;
  QString sql;
  QStringList fields;
  bool ok;

  // Setup connectability check criteria
  c.checkContactCount = cbCheckContactCount->isChecked();
  c.checkContactType = cbCheckContactType->isChecked();
  c.checkForm = cbCheckForm->isChecked();
  c.checkGenderAreOpposite = cbCheckGenderAreOpposite->isChecked();
  c.checkInsert = cbCheckInsert->isChecked();
  c.checkInsertRotation = cbCheckInsertRotation->isChecked();
  // Get SQL query
  sql = lnk.sqlForConnectableUnitConnectorsSelection(pvTestCableCnId, pvUnitId, c, ok);
  if(!ok){
    displayError(lnk.lastError());
    return;
  }
  // Setup and show dialog
  selectionDialog.setMessage(tr("Select DUT connector:"));
  selectionDialog.setQuery(sql, pvDatabase, false);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("Unit_Id_FK", true);
  selectionDialog.setColumnHidden("Connector_Id_FK", true);
  selectionDialog.setColumnHidden("ArticleConnector_Id_FK", true);
  selectionDialog.setColumnHidden("ArticleConnectorName", true);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setHeaderData("UnitConnectorName", tr("Connector"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  // Store selected DUT connector ID
  fields << "Id_PK" << "UnitConnectorName";
  s = selectionDialog.selection(fields);
  if(s.isEmpty()){
    return;
  }
  Q_ASSERT(s.rowCount() == 1);
  pvCnId = s.data(0, "Id_PK");
  lbCn->setText(s.data(0, "UnitConnectorName").toString());
  */
}

void mdtTtLogicalTestCableDutWidget::selectDutConnection()
{
  mdtClLink lnk(0, pvDatabase);
  mdtSqlSelectionDialog selectionDialog(this);
  mdtSqlTableSelection s;
  mdtClConnectableCriteria c;
  QString sql;
  QStringList fields;

  // Setup connectability check criteria
  c.checkContactType = true;
  c.checkContactName = false;
  // Get SQL query
  sql = lnk.sqlForConnectableUnitConnectionsSelection(pvTestCableCnId, pvUnitId, c);
  if(sql.isEmpty()){
    displayError(lnk.lastError());
    return;
  }
  // Setup and show dialog
  selectionDialog.setMessage(tr("Select DUT connection:"));
  selectionDialog.setQuery(sql, pvDatabase, false);
  selectionDialog.setColumnHidden("UnitConnection_Id_PK", true);
  selectionDialog.setColumnHidden("Unit_Id_FK", true);
  selectionDialog.setColumnHidden("UnitConnector_Id_FK", true);
  selectionDialog.setColumnHidden("UnitConnectorName", true);
  selectionDialog.setColumnHidden("ConnectionType_Code_FK", true);
  selectionDialog.setColumnHidden("ArticleConnection_Id_FK", true);
  selectionDialog.setHeaderData("UnitContactName", tr("Connection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  // Store selected DUT connector ID
  fields << "UnitConnection_Id_PK" << "UnitContactName";
  s = selectionDialog.selection(fields);
  if(s.isEmpty()){
    return;
  }
  Q_ASSERT(s.rowCount() == 1);
  pvCnId = s.data(0, "UnitConnection_Id_PK");
  lbCnName->setText(s.data(0, "UnitContactName").toString());
}

void mdtTtLogicalTestCableDutWidget::displayError(const mdtError& error)
{
  QMessageBox msgBox(this);

  msgBox.setText(error.text());
  msgBox.setInformativeText(error.informativeText());
  msgBox.setDetailedText(error.systemText());
  msgBox.setIcon(error.levelIcon());
  msgBox.exec();
}
