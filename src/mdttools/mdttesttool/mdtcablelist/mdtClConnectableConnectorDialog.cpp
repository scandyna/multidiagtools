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
#include "mdtClConnectableConnectorDialog.h"
#include "mdtError.h"
#include "mdtClLink.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include <QMessageBox>
#include <QLabel>
#include <QGroupBox>
#include <QSqlRecord>

mdtClConnectableConnectorDialog::mdtClConnectableConnectorDialog(QWidget* parent, QSqlDatabase db)
 : QDialog(parent),
   pvDatabase(db)
{
  setupUi(this);
  connect(pbSelectEndConnector, SIGNAL(clicked()), this, SLOT(selectEndConnector()));
  clearStartConnectorWidgets();
  clearEndConnectorWidgets();
  mdtClLink lnk(0, pvDatabase);
}

void mdtClConnectableConnectorDialog::setStartConnector(const QVariant& unitConnectorId)
{
  pvStartUnitConnectorId = unitConnectorId;
  displayStartConnector();
}

void mdtClConnectableConnectorDialog::setStartConnectorLabel(const QString & text)
{
  gbStartConnector->setTitle(text);
}

void mdtClConnectableConnectorDialog::setEndUnitId(const QVariant& unitId)
{
  pvEndUnitIdList.clear();
  pvEndUnitIdList.append(unitId);
}

void mdtClConnectableConnectorDialog::setEndUnitIdList(const QList< QVariant >& unitIdList)
{
  pvEndUnitIdList = unitIdList;
}

void mdtClConnectableConnectorDialog::setEndConnectorLabel(const QString & text)
{
  gbEndConnector->setTitle(text);
}

QString mdtClConnectableConnectorDialog::endUnitConnectorName() const
{
  return lbEndConnectorName->text();
}

void mdtClConnectableConnectorDialog::setConnectableCriteria(const mdtClConnectableCriteria & c)
{
  cbCheckContactCount->setChecked(c.checkContactCount);
  cbCheckContactName->setChecked(c.checkContactName);
  cbCheckContactType->setChecked(c.checkContactType);
  cbCheckForm->setChecked(c.checkForm);
  cbCheckGenderAreOpposite->setChecked(c.checkGenderAreOpposite);
  cbCheckInsert->setChecked(c.checkInsert);
  cbCheckInsertRotation->setChecked(c.checkInsertRotation);
}

mdtClConnectableCriteria mdtClConnectableConnectorDialog::getConnectableCriteria() const
{
  mdtClConnectableCriteria c;

  c.checkContactCount = cbCheckContactCount->isChecked();
  c.checkContactName = cbCheckContactName->isChecked();
  c.checkContactType = cbCheckContactType->isChecked();
  c.checkForm = cbCheckForm->isChecked();
  c.checkGenderAreOpposite = cbCheckGenderAreOpposite->isChecked();
  c.checkInsert = cbCheckInsert->isChecked();
  c.checkInsertRotation = cbCheckInsertRotation->isChecked();

  return c;
}

void mdtClConnectableConnectorDialog::selectEndConnector()
{
  mdtClLink lnk(0, pvDatabase);
  mdtSqlSelectionDialog selectionDialog(this);
  mdtSqlTableSelection s;
  mdtClConnectableCriteria c;
  QString sql;
  bool ok;

  // Get connectability check criteria
  c = getConnectableCriteria();
  // Get SQL query
  sql = lnk.sqlForConnectableUnitConnectorsSelection(pvStartUnitConnectorId, pvEndUnitIdList, c, ok);
  if(!ok){
    displayError(lnk.lastError());
    return;
  }
  // Setup and show dialog
  selectionDialog.setMessage(tr("Select a connector:"));
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
  // Store selected connector ID
  s = selectionDialog.selection("Id_PK");
  if(s.isEmpty()){
    return;
  }
  Q_ASSERT(s.rowCount() == 1);
  pvEndUnitConnectorId = s.data(0, "Id_PK");
  displayEndConnector();
}

void mdtClConnectableConnectorDialog::clearStartConnectorWidgets()
{
  lbStartConnectorName->clear();
  lbStartConnectorGender->clear();
  lbStartConnectorForm->clear();
  lbStartConnectorInsert->clear();
  lbStartConnectorInsertRotation->clear();
  pbSelectEndConnector->setEnabled(false);
}

void mdtClConnectableConnectorDialog::displayStartConnector()
{
  mdtClLink lnk(0, pvDatabase);
  QString sql;
  QList<QSqlRecord> dataList;
  QSqlRecord data;
  bool ok;

  // Clear previously displayed connector
  clearStartConnectorWidgets();
  if(pvStartUnitConnectorId.isNull()){
    return;
  }
  // Get connector data
  sql = "SELECT * FROM UnitConnector_view WHERE Id_PK = " + pvStartUnitConnectorId.toString();
  dataList = lnk.getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    displayError(lnk.lastError());
    return;
  }
  if(dataList.isEmpty()){
    return;
  }
  Q_ASSERT(dataList.size() == 1);
  data = dataList.at(0);
  // Update widgets
  lbStartConnectorName->setText(data.value("UnitConnectorName").toString());
  lbStartConnectorGender->setText(data.value("Gender").toString());
  lbStartConnectorForm->setText(data.value("Form").toString());
  lbStartConnectorInsert->setText(data.value("Insert").toString());
  lbStartConnectorInsertRotation->setText(data.value("InsertRotation").toString());
  pbSelectEndConnector->setEnabled(true);
}

void mdtClConnectableConnectorDialog::clearEndConnectorWidgets()
{
  lbEndConnectorName->clear();
  lbEndConnectorGender->clear();
  lbEndConnectorForm->clear();
  lbEndConnectorInsert->clear();
  lbEndConnectorInsertRotation->clear();
}

void mdtClConnectableConnectorDialog::displayEndConnector()
{
  mdtClLink lnk(0, pvDatabase);
  QString sql;
  QList<QSqlRecord> dataList;
  QSqlRecord data;
  bool ok;

  // Clear previously displayed connector
  clearEndConnectorWidgets();
  if(pvEndUnitConnectorId.isNull()){
    return;
  }
  // Get connector data
  sql = "SELECT * FROM UnitConnector_view WHERE Id_PK = " + pvEndUnitConnectorId.toString();
  dataList = lnk.getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    displayError(lnk.lastError());
    return;
  }
  if(dataList.isEmpty()){
    return;
  }
  Q_ASSERT(dataList.size() == 1);
  data = dataList.at(0);
  // Update widgets
  lbEndConnectorName->setText(data.value("UnitConnectorName").toString());
  lbEndConnectorGender->setText(data.value("Gender").toString());
  lbEndConnectorForm->setText(data.value("Form").toString());
  lbEndConnectorInsert->setText(data.value("Insert").toString());
  lbEndConnectorInsertRotation->setText(data.value("InsertRotation").toString());
}

void mdtClConnectableConnectorDialog::displayError(const mdtError& error)
{
  QMessageBox msgBox(this);

  msgBox.setText(error.text());
  msgBox.setInformativeText(error.informativeText());
  msgBox.setDetailedText(error.systemText());
  msgBox.setIcon(error.levelIcon());
  msgBox.exec();
}
