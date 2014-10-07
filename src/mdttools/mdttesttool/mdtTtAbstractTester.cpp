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
#include "mdtTtAbstractTester.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include "mdtSqlRelationInfo.h"
#include "mdtSqlRecord.h"
#include <QSqlError>
#include <QModelIndex>
#include <QMessageBox>
#include <QList>
#include <QStringList>

#include <QDebug>

using namespace std;

mdtTtAbstractTester::mdtTtAbstractTester(QSqlDatabase db, QObject* parent)
 : QObject(parent),
   pvDatabase(db),
   pvTestViewController(new mdtSqlDataWidgetController),
   pvTestNodeManager(new mdtTtTestNodeManager(0, db)),
   pvTest(new mdtTtTest(0, db))
{
  pvParentWidget = 0;
  /**
  pvTestFormWidget->setAskUserBeforRevert(false);
  pvTestFormWidget->setModel(pvTest->testTableModel().get());
  */
  ///connect(pvTest.get(), SIGNAL(testDataChanged(const QSqlRecord&)), this, SIGNAL(testDataChanged(const QSqlRecord&)));
}

bool mdtTtAbstractTester::setup()
{
  mdtSqlRelationInfo relationInfo;

  // Setup Test_view controller
  pvTestViewController->setCanWriteToDatabase(false); // We act on a view - controller can only submit data to model itself, not to database tables
  pvTestViewController->setTableName("Test_view", pvDatabase, tr("Test"));
  // Setup TestItem_view controller
  relationInfo.setChildTableName("TestItem_view");
  relationInfo.addRelation("Id_PK", "Test_Id_FK", false);
  if(!pvTestViewController->addChildController<mdtSqlTableViewController>(relationInfo, tr("Test items"))){
    pvLastError = pvTestViewController->lastError();
    return false;
  }
  pvTestItemViewController = pvTestViewController->childController<mdtSqlTableViewController>("TestItem_view");
  Q_ASSERT(pvTestItemViewController);
  pvTestItemViewController->setCanWriteToDatabase(false);

  return true;
  /**
  if(!pvTest->setup()){
    pvLastError = pvTest->lastError();
    displayLastError();
    return false;
  }
  return true;
  */
}

bool mdtTtAbstractTester::setTestUiWidget(QWidget* widget)
{
  Q_ASSERT(widget != 0);

  pvParentWidget = widget;
  if(!pvTestViewController->mapFormWidgets(widget)){
    pvLastError = pvTestViewController->lastError();
    displayLastError();
    return false;
  }

  return true;
  /**
  pvTestFormWidget->mapFormWidgets(widget);
  pvTestFormWidget->setCurrentIndex(-1);
  pvTestFormWidget->start();
  */
}

bool mdtTtAbstractTester::start()
{
  Q_ASSERT(pvTestItemViewController);

  pvTestViewController->start();
  if(!pvTestViewController->setFilter("Id_PK=-1")){
    pvLastError = pvTestViewController->lastError();
    return false;
  }
  if(!pvTestViewController->select()){
    pvLastError = pvTestViewController->lastError();
    return false;
  }

  return true;

  /**
  if(!pvTestViewController->start()){
    pvLastError = pvTestViewController->lastError();
    displayLastError();
    return false;
  }

  return true;
  */
}

bool mdtTtAbstractTester::testIsEmpty() const
{
  QStringList fields;
  mdtSqlRecord record;
  int i;
  int row, rowCount;

  // Check test data in Test_tbl
  if(pvTestViewController->rowCount(false) < 1){
    return true;
  }
  fields << "DutName" << "DutSerialNumber";
  record = pvTestViewController->currentRecord(fields);
  for(i = 0; i < record.count(); ++i){
    if(!record.value(i).isNull()){
      return false;
    }
  }
  // Check in TestItem_tbl
  fields.clear();
  fields << "Date" << "MeasuredValue" << "ResultValue" << "Result" << "Remark";
  rowCount = pvTestItemViewController->rowCount(true);
  for(row = 0; row < rowCount; ++row){
    record = pvTestItemViewController->record(row, fields);
    for(i = 0; i < record.count(); ++i){
      if(!record.value(i).isNull()){
        return false;
      }
    }
  }

  return true;
}

/**
bool mdtTtAbstractTester::testIsSaved() const
{
  int row, col;
  QModelIndex index;

  // Check test data in Test_tbl
  if(pvTestTableModel->rowCount() < 1){
    return true;
  }
  Q_ASSERT(pvTestTableModel->rowCount() == 1);
  for(col = 0; col < pvTestTableModel->columnCount(); ++col){
    // Test_tbl model if filtered on current Id_PK, it contains only 1 row
    index = pvTestTableModel->index(0, col);
    qDebug() << "-- Checking in Test_tbl, data: " << pvTestTableModel->data(index);
    if(pvTestTableModel->isDirty(index)){
      return false;
    }
  }
  // Check test items
  for(row = 0; row < pvTestItemTableModel->rowCount(); ++row){
    for(col = 0; col < pvTestItemTableModel->columnCount(); ++col){
      index = pvTestItemTableModel->index(row, col);
      if(pvTestItemTableModel->isDirty(index)){
        return false;
      }
    }
  }

  return true;
}
*/

void mdtTtAbstractTester::createTest()
{
  mdtSqlSelectionDialog selectionDialog(pvParentWidget);
  mdtSqlTableSelection s;
  QString sql;
  QVariant testModelId;
  QVariant testId;

  // Remove test if it is empty
  if(!removeTestIfEmpty()){
    displayLastError();
    return;
  }
  // Check if test is saved
  if(!testIsSaved()){
    QMessageBox msgBox(pvParentWidget);
    msgBox.setText(tr("Current test was not saved."));
    msgBox.setInformativeText(tr("If you continue, current test will be lost. Do you want to continue ?"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::No);
    if(msgBox.exec() != QMessageBox::Yes){
      return;
    }
    // Remove current test
    if(!removeCurrentTestPv()){
      displayLastError();
      return;
    }
  }
  // Let the user choose a test model
  sql = "SELECT TM.Id_PK, TM.DesignationEN";
  sql += " FROM TestModel_tbl TM";
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setMessage(tr("Select a test type to use:"));
  selectionDialog.setWindowTitle(tr("Test selection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  s = selectionDialog.selection("Id_PK");
  if(s.isEmpty()){
    return;
  }
  Q_ASSERT(s.rowCount() == 1);
  testModelId = s.data(0, "Id_PK");
  // Create the new test
  testId = pvTest->createTest(testModelId);
  if(testId.isNull()){
    pvLastError = pvTest->lastError();
    displayLastError();
    return;
  }
  // Go to created test
  if(!pvTestViewController->setFilter("Id_PK", testId)){
    pvLastError = pvTestViewController->lastError();
    displayLastError();
    return;
  }
}

void mdtTtAbstractTester::openTest()
{
  mdtSqlSelectionDialog selectionDialog(pvParentWidget);
  mdtSqlTableSelection s;
  QString sql;
  QVariant testId;

  // Remove test if it is empty
  if(!removeTestIfEmpty()){
    displayLastError();
    return;
  }
  // Check if test is saved
  if(!testIsSaved()){
    QMessageBox msgBox(pvParentWidget);
    msgBox.setText(tr("Current test was not saved."));
    msgBox.setInformativeText(tr("If you continue, current test will be lost. Do you want to continue ?"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::No);
    if(msgBox.exec() != QMessageBox::Yes){
      return;
    }
    // Remove current test
    if(!removeCurrentTestPv()){
      displayLastError();
      return;
    }
  }
  // Get current test id
  testId = pvTestViewController->currentData("Id_PK");
  ///testId = pvTest->testData().value("Id_PK");
  // Let the user choose a test
  /**
  sql = "SELECT T.Id_PK, T.Date, TM.DesignationEN, T.DutSerialNumber";
  sql += " FROM Test_tbl T JOIN TestModel_tbl TM ON TM.Id_PK = T.TestModel_Id_FK";
  if(!testId.isNull()){
    sql += " WHERE T.Id_PK <> " + testId.toString();
  }
  */
  sql = "SELECT * FROM Test_view";
  if(!testId.isNull()){
    sql += " WHERE Id_PK <> " + testId.toString();
  }
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setMessage(tr("Select test to view:"));
  selectionDialog.setWindowTitle(tr("Test selection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  s = selectionDialog.selection("Id_PK");
  if(s.isEmpty()){
    return;
  }
  Q_ASSERT(s.rowCount() == 1);
  testId = s.data(0, "Id_PK");
  // Change to selected test
  if(!pvTestViewController->setFilter("Id_PK", testId)){
    pvLastError = pvTestViewController->lastError();
    displayLastError();
    return;
  }
  ///pvTest->setCurrentTest(testId);
  ///pvTestFormWidget->toFirst();
}

void mdtTtAbstractTester::saveTest()
{
  Q_ASSERT(pvTestItemViewController);

  mdtSqlRecord testRecord;
  QList<mdtSqlRecord> testItemRecords;
  QStringList fields;
  bool ok;
  int row;
  int rowCount;

  // Test part was edited by user, let controller submit data to model
  if(!pvTestViewController->submitAndWait()){
    pvLastError = pvTestViewController->lastError();
    displayLastError();
    return;
  }
  // Get record for part that we have to save to Test_tbl
  fields << "Id_PK" << /**"TestModel_Id_FK" <<*/ "Date" << "DutName" << "DutSerialNumber";
  testRecord = pvTestViewController->currentRecord(fields, ok);
  if(!ok){
    pvLastError = pvTestViewController->lastError();
    displayLastError();
    return;
  }
  // Get part for TestItem_tbl
  fields.clear();
  fields << "Id_PK" << "Test_Id_FK" << "TestModelItem_Id_FK" << "Date" << "MeasuredValue" << "InstrumentRangeMin" << "InstrumentRangeMax"\
         << "ResultValue" << "Result" << "Remark";
  rowCount = pvTestItemViewController->rowCount(false);
  for(row = 0; row < rowCount; ++row){
    testItemRecords << pvTestItemViewController->record(row, fields, ok);
    if(!ok){
      pvLastError = pvTestItemViewController->lastError();
      displayLastError();
      return;
    }
  }
  // Save test
  if(!pvTest->saveTest(testRecord, testItemRecords)){
    pvLastError = pvTest->lastError();
    displayLastError();
    return;
  }

  if(!pvTestViewController->setFilter("Id_PK", testRecord.value("Id_PK"))){
    pvLastError = pvTestViewController->lastError();
    return;
  }

  // At first, submit data from form to DB - Error message will be shown by form widget
  /**
  if(!pvTestFormWidget->submitAndWait()){
    return;
  }
  */
  // Call mdtTtTest's save method, will also save test items
  /**
  if(!pvTest->saveCurrentTest()){
    pvLastError = pvTest->lastError();
    displayLastError();
    return;
  }
  */

}

bool mdtTtAbstractTester::removeTestIfEmpty()
{
  QVariant testId;

  testId = pvTestViewController->currentData("Id_PK");
  if(testId.isNull()){
    return true;
  }
  if(!testIsEmpty()){
    return true;
  }
  pvTestViewController->revert();
  if(!pvTest->removeTest(testId)){
    pvLastError = pvTest->lastError();
    return false;
  }
  if(!pvTestViewController->setFilter("Id_PK=-1")){
    pvLastError = pvTestViewController->lastError();
    return false;
  }

  return true;
}

bool mdtTtAbstractTester::removeCurrentTestPv()
{
  QVariant testId;

  if(pvTestViewController->rowCount(false) < 1){
    return true;
  }
  testId = pvTestViewController->currentData("Id_PK");
  if(!testId.isNull()){
    pvTestViewController->revert();
    if(!pvTest->removeTest(testId)){
      pvLastError = pvTest->lastError();
      return false;
    }
  }
  if(!pvTestViewController->setFilter("Id_PK=-1")){
    pvLastError = pvTestViewController->lastError();
    return false;
  }

  return true;
}

void mdtTtAbstractTester::displayLastError()
{
  QMessageBox msgBox(pvParentWidget);

  msgBox.setText(pvLastError.text());
  msgBox.setDetailedText(pvLastError.systemText());
  msgBox.setIcon(pvLastError.levelIcon());
  msgBox.exec();
}
