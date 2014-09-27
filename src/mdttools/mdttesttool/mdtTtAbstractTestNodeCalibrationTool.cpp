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
///#include "mdtSqlFormWidget.h"
#include "mdtSqlRelation.h"
#include <QSqlError>
#include <QModelIndex>
#include <QDateTime>

#include <QDebug>

mdtTtAbstractTestNodeCalibrationTool::mdtTtAbstractTestNodeCalibrationTool(QSqlDatabase db, QObject* parent)
 : QObject(parent),
   pvDatabase(db),
   pvTestNodeManager(new mdtTtTestNodeManager(0, db)),
   pvTest(new mdtTtTest(0, db)),
   pvTestNodeTableModel(new QSqlTableModel(0, db)),
   /**pvTestNodeForm(new mdtSqlFormWidget),*/
   pvTestNodeUnitViewTableModel(new QSqlTableModel(0, db)),
   pvTestNodeUnitViewRelation(new mdtSqlRelation),
   pvTestNodeUnitTableModel(new QSqlTableModel(0, db)),
   pvTestNodeUnitRelation(new mdtSqlRelation)
{
  pvParentWidget = 0;
  /**
  pvTestNodeForm->setAskUserBeforRevert(false);
  pvTestNodeForm->setModel(pvTest->testTableModel().get());
  */
}

bool mdtTtAbstractTestNodeCalibrationTool::init()
{
  QSqlError sqlError;

  if(!pvTest->init()){
    pvLastError = pvTest->lastError();
    return false;
  }
  // Setup test node table
  pvTestNodeTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
  pvTestNodeTableModel->setTable("TestNode_view");
  ///pvTestNodeTableModel->setFilter("VehicleType_Id_FK_PK = -1");
  if(!pvTestNodeTableModel->select()){
    sqlError = pvTestNodeTableModel->lastError();
    pvLastError.setError(tr("Cannot select data from table 'TestNode_view'"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtAbstractTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }
  /**
  pvTestNodeForm->setAskUserBeforRevert(false);
  pvTestNodeForm->setModel(pvTestNodeTableModel.get());
  */
  ///pvTestNodeForm->setCurrentIndex(-1);
  // Setup test node unit table view
  pvTestNodeUnitViewTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
  pvTestNodeUnitViewTableModel->setTable("TestNodeUnit_view");
  pvTestNodeUnitViewTableModel->setFilter("Unit_Id_FK_PK = -1");
  if(!pvTestNodeUnitViewTableModel->select()){
    sqlError = pvTestNodeTableModel->lastError();
    pvLastError.setError(tr("Cannot select data from table 'TestNodeUnit_view'"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtAbstractTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }
  // Setup TestNode_view -> TestNodeUnit_view relation
  pvTestNodeUnitViewRelation->setParentModel(pvTestNodeTableModel.get());
  pvTestNodeUnitViewRelation->setChildModel(pvTestNodeUnitViewTableModel.get());
  if(!pvTestNodeUnitViewRelation->addRelation("VehicleType_Id_FK_PK", "TestNode_Id_FK", false)){
    pvLastError.setError(tr("Cannot add TestNode_view <-> TestNodeUnit_view relation"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtAbstractTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }
  ///connect(pvTestNodeForm.get(), SIGNAL(currentRowChanged(int)), pvTestNodeUnitViewRelation.get(), SLOT(setParentCurrentIndex(int)));

  // Setup test node unit table
  pvTestNodeUnitTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
  pvTestNodeUnitTableModel->setTable("TestNodeUnit_tbl");
  pvTestNodeUnitTableModel->setFilter("Unit_Id_FK_PK = -1");
  if(!pvTestNodeUnitTableModel->select()){
    sqlError = pvTestNodeTableModel->lastError();
    pvLastError.setError(tr("Cannot select data from table 'TestNodeUnit_tbl'"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtAbstractTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }
  // Setup TestNode_view -> TestNodeUnit_tbl relation
  pvTestNodeUnitRelation->setParentModel(pvTestNodeTableModel.get());
  pvTestNodeUnitRelation->setChildModel(pvTestNodeUnitTableModel.get());
  if(!pvTestNodeUnitRelation->addRelation("VehicleType_Id_FK_PK", "TestNode_Id_FK", false)){
    pvLastError.setError(tr("Cannot add TestNode_view <-> TestNodeUnit_tbl relation"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtAbstractTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }
  ///connect(pvTestNodeForm.get(), SIGNAL(currentRowChanged(int)), pvTestNodeUnitRelation.get(), SLOT(setParentCurrentIndex(int)));

  
  ///pvTestNodeForm->toFirst();

  return true;
}

void mdtTtAbstractTestNodeCalibrationTool::setTestNodeUiWidget(QWidget* widget)
{
  Q_ASSERT(widget != 0);

  pvParentWidget = widget;
  /**
  pvTestNodeForm->mapFormWidgets(widget);
  pvTestNodeForm->setCurrentIndex(-1);
  pvTestNodeForm->start();
  */
}

QVariant mdtTtAbstractTestNodeCalibrationTool::currentTestNodeData(const QString& fieldName)
{
  ///return pvTestNodeForm->currentData(fieldName);
}

QVariant mdtTtAbstractTestNodeCalibrationTool::testNodeUnitData(int testNodeUnitId, const QString& fieldName)
{
  int row, col;
  QModelIndex index;

  if(pvTestNodeUnitViewTableModel->rowCount() < 1){
    return QVariant();
  }
  // Find column of requested field name
  col = pvTestNodeUnitViewTableModel->fieldIndex(fieldName);
  Q_ASSERT(col >= 0);
  // Search row
  row = testNodeUnitViewTableModelRow(testNodeUnitId);
  Q_ASSERT(row >= 0);
  index = pvTestNodeUnitViewTableModel->index(row, col);

  return pvTestNodeUnitViewTableModel->data(index);
}

QVariant mdtTtAbstractTestNodeCalibrationTool::testNodeUnitData(const QString& schemaPosition, const QString& fieldName)
{
  int row, col;
  QModelIndex index;

  if(pvTestNodeUnitViewTableModel->rowCount() < 1){
    return QVariant();
  }
  // Find column of requested field name
  col = pvTestNodeUnitViewTableModel->fieldIndex(fieldName);
  Q_ASSERT(col >= 0);
  // Search row
  row = testNodeUnitViewTableModelRow(schemaPosition);
  Q_ASSERT(row >= 0);
  index = pvTestNodeUnitViewTableModel->index(row, col);

  return pvTestNodeUnitViewTableModel->data(index);
}

bool mdtTtAbstractTestNodeCalibrationTool::setTestNodeUnitData(int testNodeUnitId, const QString& fieldName, const QVariant& data)
{
  int row, col;
  QModelIndex testNodeUnitViewIndex;
  QModelIndex testNodeUnitTableIndex;

  // Find index in TestNodeUnit_view
  col = pvTestNodeUnitViewTableModel->fieldIndex(fieldName);
  Q_ASSERT(col >= 0);
  row = testNodeUnitViewTableModelRow(testNodeUnitId);
  Q_ASSERT(row >= 0);
  testNodeUnitViewIndex = pvTestNodeUnitViewTableModel->index(row, col);
  Q_ASSERT(testNodeUnitViewIndex.isValid());
  // Find index in TestNodeUnit_tbl
  col = pvTestNodeUnitTableModel->fieldIndex(fieldName);
  Q_ASSERT(col >= 0);
  row = testNodeUnitTableModelRow(testNodeUnitId);
  Q_ASSERT(row >= 0);
  testNodeUnitTableIndex = pvTestNodeUnitViewTableModel->index(row, col);
  Q_ASSERT(testNodeUnitTableIndex.isValid());

  // Set data
  pvTestNodeUnitViewTableModel->setData(testNodeUnitViewIndex, data);
  pvTestNodeUnitTableModel->setData(testNodeUnitTableIndex, data);

  return true;
}

bool mdtTtAbstractTestNodeCalibrationTool::setTestNodeUnitData(const QString& schemaPosition, const QString& fieldName, const QVariant& data)
{
  QVariant testNodeUnitId;

  testNodeUnitId = testNodeUnitData(schemaPosition, "Unit_Id_FK_PK");
  Q_ASSERT(!testNodeUnitId.isNull());

  return setTestNodeUnitData(testNodeUnitId.toInt(), fieldName, data);
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

int mdtTtAbstractTestNodeCalibrationTool::testNodeUnitViewTableModelRow(int testNodeUnitId)
{
  int row, col;
  QModelIndex index;

  if(pvTestNodeUnitViewTableModel->rowCount() < 1){
    return -1;
  }
  // Find column of Unit_Id_FK_PK
  col = pvTestNodeUnitViewTableModel->fieldIndex("Unit_Id_FK_PK");
  Q_ASSERT(col >= 0);
  // Search row
  row = 0;
  while(true){
    index = pvTestNodeUnitViewTableModel->index(row, col);
    if(pvTestNodeUnitViewTableModel->data(index).toInt() == testNodeUnitId){
      return row;
    }
    if(row == (pvTestNodeUnitViewTableModel->rowCount()-1)){
      if(!pvTestNodeUnitViewTableModel->canFetchMore()){
        return -1;
      }
      pvTestNodeUnitViewTableModel->fetchMore();
    }
    ++row;
  }

  return -1;
}

int mdtTtAbstractTestNodeCalibrationTool::testNodeUnitViewTableModelRow(const QString& schemaPosition)
{
  int row, col;
  QModelIndex index;

  if(pvTestNodeUnitViewTableModel->rowCount() < 1){
    return -1;
  }
  // Find column of SchemaPosition
  col = pvTestNodeUnitViewTableModel->fieldIndex("SchemaPosition");
  Q_ASSERT(col >= 0);
  // Search row
  row = 0;
  while(true){
    index = pvTestNodeUnitViewTableModel->index(row, col);
    if(pvTestNodeUnitViewTableModel->data(index).toString() == schemaPosition){
      return row;
    }
    if(row == (pvTestNodeUnitViewTableModel->rowCount()-1)){
      if(!pvTestNodeUnitViewTableModel->canFetchMore()){
        return -1;
      }
      pvTestNodeUnitViewTableModel->fetchMore();
    }
    ++row;
  }

  return -1;
}

int mdtTtAbstractTestNodeCalibrationTool::testNodeUnitTableModelRow(int testNodeUnitId)
{
  int row, col;
  QModelIndex index;

  if(pvTestNodeUnitTableModel->rowCount() < 1){
    return -1;
  }
  // Find column of Unit_Id_FK_PK
  col = pvTestNodeUnitTableModel->fieldIndex("Unit_Id_FK_PK");
  Q_ASSERT(col >= 0);
  // Search row
  row = 0;
  while(true){
    index = pvTestNodeUnitTableModel->index(row, col);
    if(pvTestNodeUnitTableModel->data(index).toInt() == testNodeUnitId){
      return row;
    }
    if(row == (pvTestNodeUnitTableModel->rowCount()-1)){
      if(!pvTestNodeUnitTableModel->canFetchMore()){
        return -1;
      }
      pvTestNodeUnitTableModel->fetchMore();
    }
    ++row;
  }

  return -1;
}
