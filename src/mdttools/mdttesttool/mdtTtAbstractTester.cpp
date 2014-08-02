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
#include "mdtSqlFormWidget.h"
#include <QSqlError>
#include <QModelIndex>
#include <QMessageBox>

#include <QDebug>

using namespace std;

mdtTtAbstractTester::mdtTtAbstractTester(QSqlDatabase db, QObject* parent)
 : QObject(parent),
   pvDatabase(db),
   pvTestNodeManager(new mdtTtTestNodeManager(0, db)),
   pvTest(new mdtTtTest(0, db)),
   pvTestFormWidget(new mdtSqlFormWidget)
{
  pvParentWidget = 0;
  pvTestFormWidget->setAskUserBeforRevert(false);
  pvTestFormWidget->setModel(pvTest->testTableModel().get());
  connect(pvTest.get(), SIGNAL(testDataChanged(const QSqlRecord&)), this, SIGNAL(testDataChanged(const QSqlRecord&)));
}

bool mdtTtAbstractTester::init()
{
  if(!pvTest->init()){
    pvLastError = pvTest->lastError();
    return false;
  }
  pvTestFormWidget->setCurrentIndex(-1);

  return true;
}

void mdtTtAbstractTester::setTestUiWidget(QWidget* widget)
{
  Q_ASSERT(widget != 0);

  pvParentWidget = widget;
  pvTestFormWidget->mapFormWidgets(widget);
  pvTestFormWidget->setCurrentIndex(-1);
  pvTestFormWidget->start();
}

bool mdtTtAbstractTester::testIsEmpty() const
{
  if(!pvTestFormWidget->allDataAreSaved(false)){
    return false;
  }
  return pvTest->testIsEmpty();
}

bool mdtTtAbstractTester::testIsSaved() const
{
  if(!pvTestFormWidget->allDataAreSaved(false)){
    return false;
  }
  return pvTest->testIsSaved();
}

void mdtTtAbstractTester::createTest()
{
  mdtSqlSelectionDialog selectionDialog(pvParentWidget);
  mdtSqlTableSelection s;
  QString sql;
  QVariant testModelId;

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
    pvTestFormWidget->revert();
  }
  pvTestFormWidget->setCurrentIndex(-1);
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
  if(!pvTest->createTest(testModelId)){
    pvLastError = pvTest->lastError();
    displayLastError();
    return;
  }
  pvTestFormWidget->toFirst();
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
    pvTestFormWidget->revert();
  }
  // Get current test id
  testId = pvTest->testData().value("Id_PK");
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
    sql += " WHERE T.Id_PK <> " + testId.toString();
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
  pvTest->setCurrentTest(testId);
  pvTestFormWidget->toFirst();
}

void mdtTtAbstractTester::saveTest()
{
  // At first, submit data from form to DB - Error message will be shown by form widget
  if(!pvTestFormWidget->submitAndWait()){
    return;
  }
  // Call mdtTtTest's save method, will also save test items
  if(!pvTest->saveCurrentTest()){
    pvLastError = pvTest->lastError();
    displayLastError();
    return;
  }

}

bool mdtTtAbstractTester::removeTestIfEmpty()
{
  if(!testIsEmpty()){
    return true;
  }
  if(!pvTest->removeCurrentTest()){
    pvLastError = pvTest->lastError();
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
