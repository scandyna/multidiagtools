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
#include "mdtTtAbstractTestNodeCalibrationTool.h"
#include "mdtSqlRelationInfo.h"
#include "mdtUiMessageHandler.h"
#include "mdtSqlRecord.h"
#include <QSqlError>
#include <QModelIndex>
#include <QDateTime>

#include <QDebug>

mdtTtAbstractTestNodeCalibrationTool::mdtTtAbstractTestNodeCalibrationTool(QSqlDatabase db, QObject* parent)
 : QObject(parent),
   pvDatabase(db),
   pvTestNodeManager(new mdtTtTestNodeManager(0, db)),
   pvTest(new mdtTtTest(0, db)),
   pvTestNodeTableController(new mdtSqlDataWidgetController)
{
  pvParentWidget = 0;
}

bool mdtTtAbstractTestNodeCalibrationTool::setup(QWidget *testNodeFormWidget)
{
  Q_ASSERT(testNodeFormWidget != 0);

  QSqlError sqlError;
  mdtSqlRelationInfo relationInfo;

  // Setup test node table
  pvTestNodeTableController->setCanWriteToDatabase(false);   // We act on a view - controller can only submit data to model itself, not to database tables
  pvTestNodeTableController->setTableName("TestNode_view", pvDatabase, tr("Test node"));
  ///pvTestNodeTableController->setMessageHandler(std::shared_ptr<mdtUiMessageHandler>(new mdtUiMessageHandler(testNodeFormWidget)));
  if(!pvTestNodeTableController->mapFormWidgets(testNodeFormWidget)){
    pvLastError = pvTestNodeTableController->lastError();
    return false;
  }
  // Setup test node unit table
  relationInfo.setChildTableName("TestNodeUnit_view");
  relationInfo.addRelation("VehicleType_Id_FK_PK", "TestNode_Id_FK", false);
  if(!pvTestNodeTableController->addChildController<mdtSqlTableViewController>(relationInfo, tr("Test node units"))){
    pvLastError = pvTestNodeTableController->lastError();
    return false;
  }
  pvTestNodeUnitTableController = pvTestNodeTableController->childController<mdtSqlTableViewController>("TestNodeUnit_view");
  Q_ASSERT(pvTestNodeUnitTableController);
  pvTestNodeUnitTableController->setCanWriteToDatabase(false);
  pvTestNodeUnitTableController->addColumnToSortOrder("IoPosition", Qt::AscendingOrder);
  // Start controllers
  pvTestNodeTableController->start();
  if(!pvTestNodeTableController->select()){
    pvLastError = pvTestNodeTableController->lastError();
    return false;
  }
  pvTestNodeUnitTableController->sort();

  /** \toso Idée:
   *  - Travailler tous sur un controlleur pointant sur TestNodeUnit_view (y compris affichage des résultats en live)
   *  - Créer une méthode de sauvegarde reprenant les données depuis ce contrôleur, mais correspondant aux champs
   *     à sauvegarder dans TestNodeUnit_tbl, créer une liste de mdtSqlRecord et sauvegarder d'une transaction.
   */

  pvParentWidget = testNodeFormWidget;

  return true;
}

bool mdtTtAbstractTestNodeCalibrationTool::setTestNode(const QVariant& testNodeId)
{
  if(!pvTestNodeTableController->setFilter("VehicleType_Id_FK_PK", testNodeId)){
    pvLastError = pvTestNodeTableController->lastError();
    displayLastError();
    return false;
  }
  return true;
}


bool mdtTtAbstractTestNodeCalibrationTool::setTestNodeUnitCalibrationOffset(int testNodeUnitId, double offset)
{
  QVariant data;

  // Set calibration data
  data = QDateTime::currentDateTime().toString(Qt::ISODate);
  if(!setTestNodeUnitData(testNodeUnitId, "CalibrationDate", data)){
    return false;
  }
  // Set calibration offset
  if(!setTestNodeUnitData(testNodeUnitId, "CalibrationOffset", offset)){
    setTestNodeUnitData(testNodeUnitId, "CalibrationDate", QVariant());
    return false;
  }

  return true;
}

bool mdtTtAbstractTestNodeCalibrationTool::setTestNodeUnitCalibrationOffset(const QString& schemaPosition, double offset)
{
  QVariant data;

  // Set calibration data
  data = QDateTime::currentDateTime().toString(Qt::ISODate);
  if(!setTestNodeUnitData(schemaPosition, "CalibrationDate", data)){
    return false;
  }
  // Set calibration offset
  if(!setTestNodeUnitData(schemaPosition, "CalibrationOffset", offset)){
    setTestNodeUnitData(schemaPosition, "CalibrationDate", QVariant());
    return false;
  }

  return true;
}

bool mdtTtAbstractTestNodeCalibrationTool::saveTestNodeUnitCalibrationOffsets()
{
  Q_ASSERT(pvTestNodeUnitTableController);

  mdtSqlRecord record;
  int N;
  int i;
  bool ok;
  QVariant tnuId;
  QVariant var;

  // Setup record with fields to update
  if(!record.addField("CalibrationOffset", "TestNodeUnit_tbl", pvDatabase)){
    pvLastError = record.lastError();
    return false;
  }
  if(!record.addField("CalibrationDate", "TestNodeUnit_tbl", pvDatabase)){
    pvLastError = record.lastError();
    return false;
  }
  // Beginn a transaction
  if(!beginTransaction()){
    return false;
  }
  // Save each value
  N = pvTestNodeUnitTableController->rowCount(true);
  for(i = 0; i < N; ++i){
    record.clearValues();
    // Get test node unit ID
    tnuId = pvTestNodeUnitTableController->data(i, "Unit_Id_FK_PK", ok);
    if(!ok){
      pvLastError = pvTestNodeUnitTableController->lastError();
      rollbackTransaction();
      return false;
    }
    // Get offset value
    var = pvTestNodeUnitTableController->data(i, "CalibrationOffset", ok);
    if(!ok){
      pvLastError = pvTestNodeUnitTableController->lastError();
      rollbackTransaction();
      return false;
    }
    record.setValue("CalibrationOffset", var);
    // Get offset date
    var = pvTestNodeUnitTableController->data(i, "CalibrationDate", ok);
    if(!ok){
      pvLastError = pvTestNodeUnitTableController->lastError();
      rollbackTransaction();
      return false;
    }
    record.setValue("CalibrationDate", var);
    // Save to TestNodeUnit_tbl
    if(!pvTest->updateRecord("TestNodeUnit_tbl", record, "Unit_Id_FK_PK", tnuId)){
      pvLastError = pvTest->lastError();
      rollbackTransaction();
      return false;
    }
  }
  // Commit trasaction
  if(!commitTransaction()){
    return false;
  }

  return true;
}

void mdtTtAbstractTestNodeCalibrationTool::displayLastError()
{
  QMessageBox msgBox(pvParentWidget);

  msgBox.setText(pvLastError.text());
  msgBox.setInformativeText(pvLastError.informativeText());
  msgBox.setDetailedText(pvLastError.systemText());
  msgBox.setIcon(pvLastError.levelIcon());
  msgBox.exec();
}

void mdtTtAbstractTestNodeCalibrationTool::displayMessage(const QString& text, const QString& informativeText, QMessageBox::Icon icon)
{
  QMessageBox msgBox(pvParentWidget);

  msgBox.setText(text);
  msgBox.setInformativeText(informativeText);
  msgBox.setIcon(icon);
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.exec();
}

bool mdtTtAbstractTestNodeCalibrationTool::promptUser(const QString& text, const QString& informativeText, QMessageBox::Icon icon, QMessageBox::StandardButtons buttons)
{
  return askUser(text, informativeText, icon, buttons);
}

bool mdtTtAbstractTestNodeCalibrationTool::askUser(const QString& text, const QString& informativeText, QMessageBox::Icon icon, QMessageBox::StandardButtons buttons)
{
  QMessageBox msgBox(pvParentWidget);

  msgBox.setText(text);
  msgBox.setInformativeText(informativeText);
  msgBox.setIcon(icon);
  msgBox.setStandardButtons(buttons);
  switch(msgBox.exec()){
    case QMessageBox::Ok:
    case QMessageBox::Open:
    case QMessageBox::Save:
    case QMessageBox::Apply:
    case QMessageBox::SaveAll:
    case QMessageBox::Yes:
    case QMessageBox::YesToAll:
      return true;
    default:
      return false;
  }
}

bool mdtTtAbstractTestNodeCalibrationTool::isInRange(const mdtValue & value, double min, double max)
{
  double x;

  // Get double value
  if(value.isMinusOl()){
    x = -std::numeric_limits<double>::max();
  }else if(value.isPlusOl()){
    x = std::numeric_limits<double>::max();
  }else{
    x = value.valueDouble();
  }
  // Check range
  if(!mdtTtTest::isInRange(x, min, max)){
    QString msg;
    pvLastError.setError(tr("Range checking failed for given value."), mdtError::Error);
    msg  = tr("Value") + " " + QString::number(x) + " " + tr("is out of range.") + " ";
    msg += tr("Allowed range is: ") + "[" + QString::number(min) + ";" + QString::number(max) + "]";
    pvLastError.setInformativeText(msg);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtTtAbstractTestNodeCalibrationTool::beginTransaction() 
{
  if(!pvDatabase.transaction()){
    QSqlError sqlError = pvDatabase.lastError();
    pvLastError.setError(tr("Cannot beginn transaction (database: '") + pvDatabase.databaseName() + tr("')."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtAbstractTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }
  return true;
}

bool mdtTtAbstractTestNodeCalibrationTool::rollbackTransaction() 
{
  if(!pvDatabase.rollback()){
    QSqlError sqlError = pvDatabase.lastError();
    pvLastError.setError(tr("Cannot beginn rollback (database: '") + pvDatabase.databaseName() + tr("')."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtAbstractTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }
  return true;
}

bool mdtTtAbstractTestNodeCalibrationTool::commitTransaction() 
{
  if(!pvDatabase.commit()){
    QSqlError sqlError = pvDatabase.lastError();
    pvLastError.setError(tr("Cannot beginn commit (database: '") + pvDatabase.databaseName() + tr("')."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtAbstractTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }
  return true;
}
