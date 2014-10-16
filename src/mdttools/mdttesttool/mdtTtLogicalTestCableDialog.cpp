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
#include "mdtTtLogicalTestCableDialog.h"
#include "mdtTtLogicalTestCableDutWidget.h"
///#include "mdtTtLogicalTestCableTcWidget.h"
#include "mdtTtLogicalTestCableTsWidget.h"
#include "mdtClUnit.h"
#include <QVBoxLayout>
#include <QString>
#include <QSqlRecord>
#include <QMessageBox>

#include <QDebug>

mdtTtLogicalTestCableDialog::mdtTtLogicalTestCableDialog(QWidget* parent, QSqlDatabase db)
 : QDialog(parent),
   pvDatabase(db)
{
  QWidget *wAffectationWidget;

  setupUi(this);
  pvAffectationLayout = new QVBoxLayout;
  wAffectationWidget = scAffectationWidget->widget();
  if(wAffectationWidget == 0){
    wAffectationWidget = new QWidget;
    scAffectationWidget->setWidget(wAffectationWidget);
  }
  wAffectationWidget->setLayout(pvAffectationLayout);
}
///|StartUnitConnectorName|StartUnitContactName||EndUnitConnectorName|EndUnitContactName|||UnitConnectionStart_Id_FK|UnitConnectionEnd_Id_FK|StartUnit_Id_FK|EndUnit_Id_FK|LinkType_Code_FK|
void mdtTtLogicalTestCableDialog::setTestCable(const QVariant& testCableId)
{
  mdtClUnit unit(0, pvDatabase);
  QString sql;
  QList<QVariant> dataList;
  QList<QSqlRecord> recList;
  bool ok;
  int i;

  // Get test cable identification
  sql = "SELECT Identification FROM TestCable_tbl WHERE Unit_Id_FK_PK = " + testCableId.toString();
  dataList = unit.getDataList<QVariant>(sql, ok);
  if(!ok){
    displayError(unit.lastError());
    reject();
    return;
  }
  Q_ASSERT(dataList.size() == 1);
  lbTestCableIdentification->setText(dataList.at(0).toString());
  // Get start (=DUT) connectors list for given test cable (Id_PK and Name)
  sql = "SELECT DISTINCT StartUnitConnector_Id_FK, StartUnitConnectorName FROM UnitLink_view";
  sql += " WHERE StartUnit_Id_FK = " + testCableId.toString();
  sql += " AND EndUnit_Id_FK = " + testCableId.toString();
  sql += " AND StartUnitConnector_Id_FK NOT NULL";
  recList = unit.getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    displayError(unit.lastError());
    reject();
    return;
  }
  for(i = 0; i < recList.size(); ++i){
    mdtTtLogicalTestCableDutWidget *w = new mdtTtLogicalTestCableDutWidget(0, pvDatabase);
    w->setTestCableConnector(recList.at(i).value("StartUnitConnector_Id_FK"), recList.at(i).value("StartUnitConnectorName").toString());
    addAffectation(w);
  }
  // Get start (=DUT) connections list for given test cable (Id_PK and UnitContactName) that are not part of a connector
  sql = "SELECT DISTINCT UnitConnectionStart_Id_FK, StartUnitContactName FROM UnitLink_view";
  sql += " WHERE StartUnit_Id_FK = " + testCableId.toString();
  sql += " AND EndUnit_Id_FK = " + testCableId.toString();
  sql += " AND StartUnitConnector_Id_FK IS NULL";
  recList = unit.getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    displayError(unit.lastError());
    reject();
    return;
  }
  for(i = 0; i < recList.size(); ++i){
    mdtTtLogicalTestCableDutWidget *w = new mdtTtLogicalTestCableDutWidget(0, pvDatabase);
    w->setTestCableConnection(recList.at(i).value("UnitConnectionStart_Id_FK"), recList.at(i).value("StartUnitContactName").toString());
    addAffectation(w);
  }

  // Get end (=Test system side) connectors list for given test cable (Id_PK and Name)
  sql = "SELECT DISTINCT EndUnitConnector_Id_FK, EndUnitConnectorName FROM UnitLink_view";
  sql += " WHERE StartUnit_Id_FK = " + testCableId.toString();
  sql += " AND EndUnit_Id_FK = " + testCableId.toString();
  sql += " AND EndUnitConnector_Id_FK NOT NULL";
  recList = unit.getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    displayError(unit.lastError());
    reject();
    return;
  }
  for(i = 0; i < recList.size(); ++i){
    mdtTtLogicalTestCableTsWidget *w = new mdtTtLogicalTestCableTsWidget(0, pvDatabase);
    w->setTestCableConnector(recList.at(i).value("EndUnitConnector_Id_FK"), recList.at(i).value("EndUnitConnectorName").toString());
    addAffectation(w);
  }
  // Get end (=Test system side) connections list for given test cable (Id_PK and Name)
  sql = "SELECT DISTINCT UnitConnectionEnd_Id_FK, EndUnitContactName FROM UnitLink_view";
  sql += " WHERE StartUnit_Id_FK = " + testCableId.toString();
  sql += " AND EndUnit_Id_FK = " + testCableId.toString();
  sql += " AND EndUnitConnector_Id_FK IS NULL";
  recList = unit.getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    displayError(unit.lastError());
    reject();
    return;
  }
  for(i = 0; i < recList.size(); ++i){
    mdtTtLogicalTestCableTsWidget *w = new mdtTtLogicalTestCableTsWidget(0, pvDatabase);
    w->setTestCableConnection(recList.at(i).value("UnitConnectionEnd_Id_FK"), recList.at(i).value("EndUnitContactName").toString());
    addAffectation(w);
  }


}

void mdtTtLogicalTestCableDialog::addAffectation(mdtTtLogicalTestCableDutWidget* w)
{
  Q_ASSERT(w != 0);

  pvAffectationLayout->addWidget(w);
  pvDutSideAffectations.append(w);
}

void mdtTtLogicalTestCableDialog::addAffectation(mdtTtLogicalTestCableTsWidget* w)
{
  Q_ASSERT(w != 0);

  pvAffectationLayout->addWidget(w);
  pvTsSideAffectations.append(w);
}


/**
void mdtTtLogicalTestCableDialog::addAffectation(mdtTtLogicalTestCableDutWidget* dutWidget, mdtTtLogicalTestCableTcWidget* testCableWidget, mdtTtLogicalTestCableTsWidget* testSystemWidget)
{
  Q_ASSERT(dutWidget != 0);
  Q_ASSERT(testCableWidget != 0);
  Q_ASSERT(testSystemWidget != 0);

  mdtTtLogicalTestCableDialogAffectation a;
  int row = pvAffectationLayout->rowCount();

  // set widgets to container struct
  a.dutWidget = dutWidget;
  a.testCableWidget = testCableWidget;
  a.testSystemWidget = testSystemWidget;
  // Add widgets to layout
  pvAffectationLayout->addWidget(a.dutWidget, row, 0);
  pvAffectationLayout->addWidget(a.testCableWidget, row, 1);
  pvAffectationLayout->addWidget(a.testSystemWidget, row, 2);
  // Add container struct to container
  pvAffectations.append(a);
}
*/

void mdtTtLogicalTestCableDialog::displayError(const mdtError& error)
{
  QMessageBox msgBox(this);

  msgBox.setText(error.text());
  msgBox.setInformativeText(error.informativeText());
  msgBox.setDetailedText(error.systemText());
  msgBox.setIcon(error.levelIcon());
  msgBox.exec();
}
