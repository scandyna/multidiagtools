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

  /**
  if(!pvTest->setup()){
    pvLastError = pvTest->lastError();
    return false;
  }
  */
  // Setup test node table
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
