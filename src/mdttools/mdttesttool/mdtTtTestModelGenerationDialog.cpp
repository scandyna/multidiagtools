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
#include "mdtTtTestModelGenerationDialog.h"
#include "mdtTtTestModel.h"
#include <QMessageBox>
#include <QComboBox>
#include <QList>

#include <QDebug>

mdtTtTestModelGenerationDialog::mdtTtTestModelGenerationDialog(QSqlDatabase db, QWidget* parent)
 : QDialog(parent)
{
  pvDatabase = db;
  setupUi(this);
  
  populateTestTypeComboBox();
  connect(cbTestNode, SIGNAL(currentIndexChanged(int)), this, SLOT(populateTestCableComboBox(int)));
  connect(cbTestNode, SIGNAL(currentIndexChanged(int)), this, SLOT(populateMeasureConnectionAComboBox(int)));
  connect(cbTestNode, SIGNAL(currentIndexChanged(int)), this, SLOT(populateMeasureConnectionBComboBox(int)));
}

void mdtTtTestModelGenerationDialog::setTestModelId(const QVariant & testModelId)
{
  populateTestNodeComboBox(testModelId);
}

mdtTtTestModelGenerationParameter mdtTtTestModelGenerationDialog::selectedParameters() const
{
  return pvParameters;
}

mdtTtTestModelGenerationDialog::testType_t mdtTtTestModelGenerationDialog::selectedTestModelType() const
{
  int index;

  index = cbTestModelType->currentIndex();
  if(index < 0){
    return None;
  }

  return static_cast<testType_t>(cbTestModelType->itemData(index).toInt());
}

QVariant mdtTtTestModelGenerationDialog::selectedTestNodeId() const
{
  int index;

  index = cbTestNode->currentIndex();
  if(index < 0){
    return QVariant();
  }

  return cbTestNode->itemData(index);
}

QVariant mdtTtTestModelGenerationDialog::selectedTestCableId() const
{
  int index;

  index = cbTestCable->currentIndex();
  if(index < 0){
    return QVariant();
  }

  return cbTestCable->itemData(index);
}

QVariant mdtTtTestModelGenerationDialog::selectedMeasureConnectionA() const
{
  int index;

  index = cbMeasureConnectionA->currentIndex();
  if(index < 0){
    return QVariant();
  }

  return cbMeasureConnectionA->itemData(index);
}

QVariant mdtTtTestModelGenerationDialog::selectedMeasureConnectionB() const
{
  int index;

  index = cbMeasureConnectionB->currentIndex();
  if(index < 0){
    return QVariant();
  }

  return cbMeasureConnectionB->itemData(index);
}

void mdtTtTestModelGenerationDialog::populateTestCableComboBox(int index)
{
  mdtTtTestModel tm(0, pvDatabase);
  QVariant testNodeId;
  QList<QSqlRecord> dataList;
  QSqlRecord data;
  QString sql;
  QString str;
  bool ok;
  int i;

  cbTestCable->clear();
  if(index < 0){
    return;
  }
  testNodeId = cbTestNode->itemData(index);
  Q_ASSERT(!testNodeId.isNull());
  sql = "SELECT DISTINCT TestCable_Id_FK, Identification, Key FROM TestNode_TestCable_view WHERE TestNode_Id_FK = " + testNodeId.toString();
  sql += " ORDER BY Identification ASC";
  dataList = tm.getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    displayError(tm.lastError());
    return;
  }
  for(i = 0; i < dataList.size(); ++i){
    data = dataList.at(i);
    str = data.value("Identification").toString();
    if(!data.value("Key").isNull()){
      str += " (" + data.value("Key").toString() + ")";
    }
    cbTestCable->addItem(str, data.value("TestCable_Id_FK"));
  }
  // We get measure connection data from here, because this slot is called 1x when a new test node was selected
  getMeasureConnectionData(testNodeId);
}

void mdtTtTestModelGenerationDialog::populateMeasureConnectionAComboBox(int index)
{
  int i;
  QSqlRecord data;

  cbMeasureConnectionA->clear();
  if(index < 0){
    return;
  }
  for(i = 0; i < pvMeasureConnectionDataList.size(); ++i){
    data = pvMeasureConnectionDataList.at(i);
    cbMeasureConnectionA->addItem(data.value("UnitContactName").toString(), data.value("Id_PK"));
  }
}

void mdtTtTestModelGenerationDialog::populateMeasureConnectionBComboBox(int index)
{
  int i;
  QSqlRecord data;

  cbMeasureConnectionB->clear();
  if(index < 0){
    return;
  }
  for(i = 0; i < pvMeasureConnectionDataList.size(); ++i){
    data = pvMeasureConnectionDataList.at(i);
    cbMeasureConnectionB->addItem(data.value("UnitContactName").toString(), data.value("Id_PK"));
  }
}

void mdtTtTestModelGenerationDialog::accept()
{
  int index;

  // Set test model type
  /// \todo Implement
  
  // Set test node ID
  index = cbTestNode->currentIndex();
  if(index < 0){
    displayErrorAboutMissingParameter(tr("No test node was selected."));
    return;
  }
  pvParameters.testNodeId = cbTestNode->itemData(index);
  // Set test cable ID
  index = cbTestCable->currentIndex();
  if(index < 0){
    displayErrorAboutMissingParameter(tr("No test cable was selected."));
    return;
  }
  pvParameters.testCableId = cbTestCable->itemData(index);
  // Set measure connection A
  index = cbMeasureConnectionA->currentIndex();
  if(index < 0){
    displayErrorAboutMissingParameter(tr("Measure connection A was not selected."));
    return;
  }
  pvParameters.measureConnexionIdA = cbMeasureConnectionA->itemData(index);
  // Set measure connection B
  index = cbMeasureConnectionB->currentIndex();
  if(index < 0){
    displayErrorAboutMissingParameter(tr("Measure connection B was not selected."));
    return;
  }
  pvParameters.measureConnexionIdB = cbMeasureConnectionB->itemData(index);
  /// Values etc.. \todo Implement
  pvParameters.continuityExpectedValue = 0.0;
  pvParameters.isolationExpectedValue = 1e9;
  pvParameters.generateForNonLinkedConnections = false;

  QDialog::accept();
}

void mdtTtTestModelGenerationDialog::populateTestTypeComboBox()
{
  cbTestModelType->clear();
  cbTestModelType->addItem(tr("Continuity test (2 wire)"), ContinuityTest);
  cbTestModelType->addItem(tr("Short circuit detection test"), ShortDetectionTest);
}

void mdtTtTestModelGenerationDialog::populateTestNodeComboBox(const QVariant & testModelId)
{
  mdtTtTestModel tm(0, pvDatabase);
  QList<QSqlRecord> dataList;
  QSqlRecord data;
  QString text;
  bool ok;
  int i;

  cbTestNode->clear();
  dataList = tm.getTestNodeDataList(testModelId, ok);
  if(!ok){
    displayError(tm.lastError());
    return;
  }
  for(i = 0; i < dataList.size(); ++i){
    data = dataList.at(i);
    text = "ID: " + data.value("NodeIdentification").toString();
    text += " , " + data.value("Type").toString();
    text += " , " + data.value("SubType").toString();
    cbTestNode->addItem(text, data.value("VehicleType_Id_FK_PK"));
  }
}

void mdtTtTestModelGenerationDialog::getMeasureConnectionData(const QVariant & testNodeId)
{
  mdtTtTestModel tm(0, pvDatabase);
  QString sql;
  bool ok;

  pvMeasureConnectionDataList.clear();
  sql = "SELECT UCNX.Id_PK, UCNX.UnitContactName";
  sql += " FROM UnitConnection_tbl UCNX JOIN TestNodeUnitConnection_tbl TNUCNX ON TNUCNX.UnitConnection_Id_FK_PK = UCNX.Id_PK";
  sql += " JOIN TestNodeUnit_tbl TNU ON TNU.Unit_Id_FK_PK = TNUCNX.TestNodeUnit_Id_FK";
  sql += " WHERE TNU.TestNode_Id_FK = " + testNodeId.toString();
  sql += " AND TNU.Type_Code_FK = 'MEASCONNECTOR'";
  pvMeasureConnectionDataList = tm.getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    pvMeasureConnectionDataList.clear();
    displayError(tm.lastError());
    return;
  }
}

void mdtTtTestModelGenerationDialog::displayError(const mdtError & error)
{
  QMessageBox msgBox(this);

  msgBox.setText(error.text());
  msgBox.setInformativeText(error.informativeText());
  msgBox.setDetailedText(error.systemText());
  msgBox.setIcon(error.levelIcon());
  msgBox.exec();
}

void mdtTtTestModelGenerationDialog::displayErrorAboutMissingParameter(const QString & missingParameterText)
{
  QMessageBox msgBox(this);

  msgBox.setText(tr("Some parameters are missing. Please complete them and try again."));
  msgBox.setInformativeText(missingParameterText);
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.exec();
}
