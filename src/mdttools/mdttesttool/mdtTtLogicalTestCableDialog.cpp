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
#include "mdtTtLogicalTestCableTsWidget.h"
#include "mdtClUnit.h"
#include "mdtClLink.h"
#include "mdtTtLogicalTestCable.h"
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

QVariant mdtTtLogicalTestCableDialog::getDutConnectionId(const QVariant& testCableConnectionId, bool& ok)
{
  mdtClLink lnk(0, pvDatabase);
  QList<QVariant> dutConnectionIdList;
  mdtClConnectableCriteria c;
  int i, k;

  // Setup connectability check criteria
  c.checkContactType = true;
  // Find affectation that cocerns given testCableConnectionId
  for(i = 0; i < pvDutSideAffectationWidgets.size(); ++i){
    Q_ASSERT(pvDutSideAffectationWidgets.at(i) != 0);
    if(pvDutSideAffectationWidgets.at(i)->cnType() == Connector){
      c.checkContactName = true;
    }else{
      c.checkContactName = false;
    }
    if(pvDutSideAffectationWidgets.at(i)->containsTestCableConnection(testCableConnectionId, ok)){
      dutConnectionIdList = pvDutSideAffectationWidgets.at(i)->getAffectedDutConnections(ok);
      if(!ok){
        return QVariant();
      }
      // Search about DUT connection
      /// \todo Use mdtClAutoConnection
      /**
      for(k = 0; k < dutConnectionIdList.size(); ++k){
        
        if(lnk.canConnectConnections(testCableConnectionId, dutConnectionIdList.at(k), c, ok)){
          return dutConnectionIdList.at(k);
        }else{
          if(!ok){
            return QVariant();
          }
        }
      }
      */
    }else{
      if(!ok){
        return QVariant();
      }
    }
  }

  return QVariant();
}

QVariant mdtTtLogicalTestCableDialog::getTsConnectionId(const QVariant& testCableConnectionId, bool& ok)
{
  mdtClLink lnk(0, pvDatabase);
  QList<QVariant> tsConnectionIdList;
  mdtClConnectableCriteria c;
  int i, k;

  // Setup connectability check criteria
  c.checkContactType = true;
  c.checkContactName = true;
  // Find affectation that conerns given testCableConnectionId
  for(i = 0; i < pvTsSideAffectationWidgets.size(); ++i){
    Q_ASSERT(pvTsSideAffectationWidgets.at(i) != 0);
    if(pvTsSideAffectationWidgets.at(i)->containsTestCableConnection(testCableConnectionId, ok)){
      tsConnectionIdList = pvTsSideAffectationWidgets.at(i)->getAffectedTsConnections(ok);
      if(!ok){
        return QVariant();
      }
      // Search about DUT connection
      /// \todo Use mdtClAutoConnection
      /**
      for(k = 0; k < tsConnectionIdList.size(); ++k){
        if(lnk.canConnectConnections(testCableConnectionId, tsConnectionIdList.at(k), c, ok)){
          return tsConnectionIdList.at(k);
        }else{
          if(!ok){
            return QVariant();
          }
        }
      }
      */
    }else{
      if(!ok){
        return QVariant();
      }
    }
  }

  return QVariant();
}

void mdtTtLogicalTestCableDialog::addAffectation(mdtTtLogicalTestCableDutWidget* w)
{
  Q_ASSERT(w != 0);

  pvAffectationLayout->addWidget(w);
  pvDutSideAffectationWidgets.append(w);
}

void mdtTtLogicalTestCableDialog::addAffectation(mdtTtLogicalTestCableTsWidget* w)
{
  Q_ASSERT(w != 0);

  pvAffectationLayout->addWidget(w);
  pvTsSideAffectationWidgets.append(w);
}

void mdtTtLogicalTestCableDialog::accept()
{
  mdtTtLogicalTestCable ltc(0, pvDatabase);
  int i;
  bool ok;
  bool exists;

  // Check that user enterred a Key
  if(leLogicalCableKey->text().trimmed().isEmpty()){
    mdtError e(tr("Please enter a Key"), mdtError::Warning);
    displayError(e);
    return;
  }
  // Check that key does not allready exist
  exists = ltc.cableExistsByKey(leLogicalCableKey->text().trimmed(), ok);
  if(!ok){
    displayError(ltc.lastError());
    return;
  }
  if(exists){
    mdtError e(tr("A logical test cable with this key allready exists."), mdtError::Warning);
    displayError(e);
    return;
  }
  // Check that user selected a connector or connection for each DUT affectation widget at DUT side
  for(i = 0; i < pvDutSideAffectationWidgets.size(); ++i){
    Q_ASSERT(pvDutSideAffectationWidgets.at(i) != 0);
    if(pvDutSideAffectationWidgets.at(i)->cnId().isNull()){
      mdtError e(tr("Please affect something to ") + pvDutSideAffectationWidgets.at(i)->testCableCnName(), mdtError::Warning);
      displayError(e);
      return;
    }
  }
  // Check that user selected a connector or connection for each test system affectation widget at DUT side
  for(i = 0; i < pvTsSideAffectationWidgets.size(); ++i){
    Q_ASSERT(pvTsSideAffectationWidgets.at(i) != 0);
    if(pvTsSideAffectationWidgets.at(i)->cnId().isNull()){
      mdtError e(tr("Please affect something to ") + pvTsSideAffectationWidgets.at(i)->testCableCnName(), mdtError::Warning);
      displayError(e);
      return;
    }
  }

  QDialog::accept();
}

void mdtTtLogicalTestCableDialog::displayError(const mdtError& error)
{
  QMessageBox msgBox(this);

  msgBox.setText(error.text());
  msgBox.setInformativeText(error.informativeText());
  msgBox.setDetailedText(error.systemText());
  msgBox.setIcon(error.levelIcon());
  msgBox.exec();
}
