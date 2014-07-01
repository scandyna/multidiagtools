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
#include "mdtTtBasicTester.h"
#include "mdtTtTest.h"
#include "mdtTtTestModelData.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include <QString>
#include <QVariant>
#include <QMessageBox>

#include <QDebug>

mdtTtBasicTester::mdtTtBasicTester(QWidget *parent, QSqlDatabase db)
 : QMainWindow(parent)
{
  pvDatabase = db;
  setupUi(this);
  connectActions();
}

bool mdtTtBasicTester::setupTables()
{
  if(!pvTestData.setup(pvDatabase, true)){
    pvLastError = pvTestData.lastError();
    return false;
  }
  return true;
}

void mdtTtBasicTester::setTestModel()
{
  mdtSqlSelectionDialog dialog(this);
  mdtSqlTableSelection s;
  QString sql;

  /// \todo Check save, create new
  
  /// \todo Lorsqu'un test a déjà été créé, décider selon état (juste créé, déjà exécuté, .....)
  
  //

  // Setup and show dialog to select test model
  sql = "SELECT * FROM TestModel_tbl";
  dialog.setMessage(tr("Please select a test model:"));
  dialog.setQuery(sql, pvDatabase, false);
  
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Set test data
  s = dialog.selection("Id_PK");
  if(s.isEmpty()){
    return;
  }
  Q_ASSERT(s.rowCount() == 1);
  pvTestData.setValue("TestModel_Id_FK", s.data(0, "Id_PK"));
  
  qDebug() << "Test data: " << pvTestData;
  
  // Save data and update GUI
  if(pvTestData.value("Id_PK").isNull()){
    createNewTest();
  }else{
    saveTestData();
  }
}

bool mdtTtBasicTester::createNewTest()
{
  mdtTtTest test(0, pvDatabase);
  QVariant testId;

  // Set test date
  
  // Add test to DB
  testId = test.addTest(pvTestData);
  if(testId.isNull()){
    pvLastError = test.lastError();
    displayLastError();
    return false;
  }
  pvTestData.setValue("Id_PK", testId);
  displayTestData(true);

  return true;
}

bool mdtTtBasicTester::saveTestData()
{
  mdtTtTest test(0, pvDatabase);

  if(!test.updateTest(pvTestData.value("Id_PK"), pvTestData)){
    pvLastError = test.lastError();
    displayLastError();
    return false;
  }
  displayTestData(false);

  return true;
}

void mdtTtBasicTester::displayTestData(bool getFromDatabase)
{

}

void mdtTtBasicTester::displayLastError()
{
  QMessageBox msgBox;

  msgBox.setText(pvLastError.text());
  msgBox.setDetailedText(pvLastError.systemText());
  msgBox.setIcon(pvLastError.levelIcon());
  msgBox.exec();
}

void mdtTtBasicTester::connectActions()
{
  connect(actTestSetType, SIGNAL(triggered()), this, SLOT(setTestModel()));
}
