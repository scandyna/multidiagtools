/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtDatabaseTest.h"
#include "mdtApplication.h"

#include "mdtSqlTableModel.h"
///#include "mdtSqlDataWidgetMapper.h"
#include "mdtApplication.h"
#include "mdtSqlRelation.h"
#include "mdtSqlFieldHandler.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlTableWidget.h"
#include "mdtSortFilterProxyModel.h"
#include "mdtSqlWindow.h"
#include "mdtSqlDialog.h"
#include "ui_mdtSqlFormWidgetTestForm.h"
#include "mdtSqlForm.h"
#include "mdtSqlFormWindow.h"
#include "mdtSqlFormDialog.h"
#include "mdtSqlSelectionDialog.h"
#include <QTemporaryFile>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDateTimeEdit>
#include <QAbstractButton>
#include <QComboBox>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QFile>
#include <QVariant>
#include <QTimer>
#include <QAbstractButton>
#include <QModelIndex>

#include <QTableView>
#include <QItemSelectionModel>

#include <QDebug>

void mdtDatabaseTest::initTestCase()
{
  QTemporaryFile tempFile;
  ///QSqlDatabase db;
  QSqlField fld;
  QString sql;

  // Create a temp file that will be used for database
  QVERIFY(tempFile.open());
  tempFile.close();
  pvDbFileInfo.setFile(tempFile.fileName() + ".db");
  qDebug() << "Init, DB file: " << pvDbFileInfo.filePath();

  // Open database
  pvDb = QSqlDatabase::addDatabase("QSQLITE");
  pvDb.setDatabaseName(pvDbFileInfo.filePath());
  QVERIFY(pvDb.open());

  // QSqlQuery must be created after db.open() was called.
  QSqlQuery q(pvDb);

  /*
   * Enable foreing keys support
   */
  sql = "PRAGMA foreign_keys = ON";
  QVERIFY(q.exec(sql));

  /*
   * Create Client table
   */
  sql = "CREATE TABLE 'Client' (";
  sql += "'id_PK' INTEGER PRIMARY KEY AUTOINCREMENT, ";
  sql += "'first_name' VARCHAR(50) NOT NULL, ";
  sql += "'remarks' VARCHAR(500) )";
  QVERIFY(q.exec(sql));
  // Verify some attributes
  fld = pvDb.record("Client").field("id_PK");
  QVERIFY(fld.isAutoValue());
  fld = pvDb.record("Client").field("first_name");
  QVERIFY(fld.requiredStatus() == QSqlField::Required);
  // Inert some data
  QVERIFY(q.exec("INSERT INTO 'Client' ('first_name') VALUES('Andy')"));
  QVERIFY(q.exec("INSERT INTO 'Client' ('first_name', 'remarks') VALUES('Bety', 'Remark on Bety')"));
  QVERIFY(q.exec("SELECT * FROM 'Client'"));
  while(q.next()){
    QVERIFY(!q.record().isEmpty());
  }

  /*
   * Create Addresses table
   */
  sql = "CREATE TABLE 'Address' (";
  sql += "'id_PK' INTEGER PRIMARY KEY AUTOINCREMENT, ";
  sql += "'street_name' VARCHAR(50) NOT NULL, ";
  sql += "'street_number' INTEGER NOT NULL, ";
  sql += "'id_client_FK' INTEGER NOT NULL, ";
  sql += "FOREIGN KEY(id_client_FK) REFERENCES Client(id_PK) ";
  sql += "ON UPDATE RESTRICT ";
  sql += "ON DELETE RESTRICT ";
  sql += ");";
  QVERIFY(q.exec(sql));
  // Verify some attributes
  fld = pvDb.record("Address").field("id_PK");
  QVERIFY(fld.isAutoValue());
  fld = pvDb.record("Address").field("street_name");
  QVERIFY(fld.requiredStatus() == QSqlField::Required);
  fld = pvDb.record("Address").field("street_number");
  QVERIFY(fld.requiredStatus() == QSqlField::Required);
  fld = pvDb.record("Address").field("id_client_FK");
  QVERIFY(fld.requiredStatus() == QSqlField::Required);

  // Inert some data
  QVERIFY(q.exec("INSERT INTO 'Address' ('street_name', 'street_number', 'id_client_FK') VALUES('Andy street 1', 56 , 1)"));
  QVERIFY(q.exec("INSERT INTO 'Address' ('street_name', 'street_number', 'id_client_FK') VALUES('Andy street 2', 87 , 1)"));
  QVERIFY(q.exec("INSERT INTO 'Address' ('street_name', 'street_number', 'id_client_FK') VALUES('Bety street 1', 45 , 2)"));
  QVERIFY(q.exec("SELECT * FROM 'Address'"));
  while(q.next()){
    QVERIFY(!q.record().isEmpty());
  }

}

/// \todo Make data checks - Nothing is finished !!
/// \todo Check about ' bugs
void mdtDatabaseTest::relationsTest()
{
  QSqlTableModel parentModel;
  QSqlTableModel childModel;
  QModelIndex index;
  mdtSqlRelation *relation;
  mdtSqlFormWidget *clientWidget;
  mdtSqlTableWidget *addressWidget;
  Ui::mdtSqlFormWidgetTestForm form;
  mdtSqlWindow window;
  QSqlQuery q(pvDb);

  // We start with a empty table
  QVERIFY(q.exec("DELETE FROM 'Address'"));
  QVERIFY(q.exec("DELETE FROM 'Client'"));
  // Setup parent model
  parentModel.setTable("Client");
  parentModel.select();
  // Setup child model
  childModel.setTable("Address");
  childModel.select();
  // Setup relation
  relation = new mdtSqlRelation;
  relation->setParentModel(&parentModel);
  relation->setChildModel(&childModel);
  QVERIFY(relation->addRelation("id_PK", "id_client_FK", true));

  // Check data of each model
  

  // Setup parent form widget
  clientWidget = new mdtSqlFormWidget;
  clientWidget->setModel(&parentModel);
  form.setupUi(clientWidget);
  clientWidget->mapFormWidgets("fld_first_name");
  // Setup child table widget
  addressWidget = new mdtSqlTableWidget;
  addressWidget->setModel(&childModel);
  // Add addressWidget as child of clientWidget
  clientWidget->addChildWidget(addressWidget, relation);

  // Setup window
  window.setSqlWidget(clientWidget);
  window.addChildWidget(addressWidget, "Addresses");
  window.enableNavigation();
  window.enableEdition();
  window.resize(700, 400);
  window.show();

  // Add a row into client table
  QVERIFY(q.exec("INSERT INTO 'Client' ('id_PK', 'first_name') VALUES(1, 'Client 01')"));
  QVERIFY(parentModel.select());
  QCOMPARE(parentModel.rowCount(), 1);
  index = parentModel.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(parentModel.data(index), QVariant("Client 01"));
  QCOMPARE(childModel.rowCount(), 0);
  // Add a address related to client
  QVERIFY(q.exec("INSERT INTO 'Address' ('street_name', 'street_number', 'id_client_FK') VALUES('Street of client 01', 1, 1)"));
  QVERIFY(parentModel.select());
  QCOMPARE(childModel.rowCount(), 1);
  index = childModel.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(childModel.data(index), QVariant("Street of client 01"));
  index = childModel.index(0, 2);
  QVERIFY(index.isValid());
  QCOMPARE(childModel.data(index), QVariant(1));
  // Add a row into client table
  QVERIFY(q.exec("INSERT INTO 'Client' ('id_PK', 'first_name') VALUES(2, 'Client 02')"));
  QVERIFY(parentModel.select());
  relation->setParentCurrentIndex(1);
  QCOMPARE(parentModel.rowCount(), 2);
  index = parentModel.index(1, 1);
  QVERIFY(index.isValid());
  QCOMPARE(parentModel.data(index), QVariant("Client 02"));
  QCOMPARE(childModel.rowCount(), 0);
  // Add a address related to client
  QVERIFY(q.exec("INSERT INTO 'Address' ('street_name', 'street_number', 'id_client_FK') VALUES('Street of client 02', 1, 2)"));
  relation->setParentCurrentIndex(1);
  QCOMPARE(childModel.rowCount(), 1);
  index = childModel.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(childModel.data(index), QVariant("Street of client 02"));
  index = childModel.index(0, 2);
  QVERIFY(index.isValid());
  QCOMPARE(childModel.data(index), QVariant(1));

  // Play...
  /**
  while(window.isVisible()){
    QTest::qWait(1000);
  }
  */
}

void mdtDatabaseTest::sqlFieldHandlerTest()
{
  mdtSqlFieldHandler fh;
  QSqlField field;

  // Check inital flags
  QVERIFY(fh.isNull());
  QVERIFY(!fh.dataWasEdited());
  QVERIFY(!fh.isReadOnly());

  // Setup field
  field.setAutoValue(false);
  field.setLength(10);
  field.setRequiredStatus(QSqlField::Required);
  field.setReadOnly(false);
  fh.setField(field);

  /*
   * Tests with QLineEdit
   */
  QLineEdit le;
  fh.setDataWidget(&le);
  le.show();
  // Check field handler flags
  QVERIFY(fh.isNull());
  QVERIFY(!fh.dataWasEdited());
  QVERIFY(!fh.isReadOnly());
  // Check widget flags
  QVERIFY(le.isEnabled());
  QVERIFY(le.text().isEmpty());
  QVERIFY(!le.isReadOnly());
  // User edit ...
  QTest::keyClicks(&le, "ABCD");
  // Check field handler flags
  QVERIFY(!fh.isNull());
  QVERIFY(fh.dataWasEdited());
  // User edit ...
  QTest::keyClick(&le, Qt::Key_Backspace);
  QTest::keyClick(&le, Qt::Key_Backspace);
  QTest::keyClick(&le, Qt::Key_Backspace);
  QTest::keyClick(&le, Qt::Key_Backspace);
  // Check field handler flags
  QVERIFY(fh.isNull());
  QVERIFY(fh.dataWasEdited());
  // User edit ...
  QTest::keyClicks(&le, "123");
  // Check field handler flags
  QVERIFY(!fh.isNull());
  QVERIFY(fh.dataWasEdited());
  // User edit ...
  le.clear();
  QTest::keyClicks(&le, " ");
  // Check field handler flags
  QVERIFY(fh.isNull());
  QVERIFY(fh.dataWasEdited());
  // Field is mandatory, so check must fail
  QVERIFY(!fh.checkBeforeSubmit());
  // Line edit must be empty now
  QVERIFY(le.text().isEmpty());
  // User edit ...
  le.clear();
  QTest::keyClicks(&le, "ZA");
  // Check field handler flags
  QVERIFY(!fh.isNull());
  QVERIFY(fh.dataWasEdited());
  // Check must be OK now
  QVERIFY(fh.checkBeforeSubmit());
  // User edit to much
  le.clear();
  QTest::keyClicks(&le, "123456789ABC");
  // Check field handler flags
  QVERIFY(!fh.isNull());
  QCOMPARE(le.text(), QString("123456789A"));
  // Set read only
  fh.setReadOnly(true);
  QVERIFY(fh.isReadOnly());
  QVERIFY(le.isReadOnly());
  // Set editable again
  fh.setReadOnly(false);
  QVERIFY(!fh.isReadOnly());
  QVERIFY(!le.isReadOnly());
  // Now we change the row
  le.setText("Row 2 from DB");
  fh.updateFlags();
  // Check field handler flags
  QVERIFY(!fh.isNull());
  QVERIFY(!fh.dataWasEdited());
  // User edit ...
  le.clear();
  QTest::keyClicks(&le, " ");
  // Check field handler flags
  QVERIFY(fh.isNull());
  QVERIFY(fh.dataWasEdited());
  // We change the row
  le.setText("Row 3 from DB");
  fh.updateFlags();
  // Check field handler flags
  QVERIFY(!fh.isNull());
  QVERIFY(!fh.dataWasEdited());
  // We change the row
  le.setText("");
  fh.updateFlags();
  // Check field handler flags
  QVERIFY(fh.isNull());
  QVERIFY(!fh.dataWasEdited());

  
  // Setup field as read only
  field.setAutoValue(false);
  field.setLength(10);
  field.setRequiredStatus(QSqlField::Required);
  field.setReadOnly(true);
  fh.setField(field);
  // Check field handler flags
  ///QVERIFY(fh.isReadOnly());
  // Check widget flags
  QVERIFY(le.isEnabled());
  QVERIFY(le.isReadOnly());


  
  
  /*
   * Play
   */
  /*
  while(le.isVisible()){
    QTest::qWait(1000);
  }
  */

}

void mdtDatabaseTest::sqlFormWidgetTest()
{
  mdtSqlFormWidget *sqlFormWidget;
  mdtSqlWindow window;
  Ui::mdtSqlFormWidgetTestForm form;
  QSqlTableModel model;
  QLineEdit *leFirstName = 0;
  QLineEdit *leRemarks = 0;
  QWidget *w;
  int rowCount;
  int row;
  QVariant data;
  QString sql;
  QSqlQuery q;

  // For this test, we wont foreign_keys support
  sql = "PRAGMA foreign_keys = OFF";
  QVERIFY(q.exec(sql));

  // Setup model + form view
  model.setTable("Client");
  model.select();
  sqlFormWidget = new mdtSqlFormWidget;
  QTest::qWait(50);
  sqlFormWidget->setModel(&model);
  form.setupUi(sqlFormWidget);
  sqlFormWidget->mapFormWidgets("fld_first_name");
  // Setup window
  window.setSqlWidget(sqlFormWidget);
  window.enableNavigation();
  window.enableEdition();
  // We need access to form's line edits
  QVERIFY(sqlFormWidget->layout() != 0);
  for(int i=0; i<sqlFormWidget->layout()->count(); i++){
    QVERIFY(sqlFormWidget->layout()->itemAt(i) != 0);
    w = sqlFormWidget->layout()->itemAt(i)->widget();
    QVERIFY(w != 0);
    if(w->objectName() == "fld_first_name"){
      leFirstName = dynamic_cast<QLineEdit*>(w);
    }else if(w->objectName() == "fld_remarks"){
      leRemarks = dynamic_cast<QLineEdit*>(w);
    }
  }
  QVERIFY(leFirstName != 0);
  QVERIFY(leRemarks != 0);
  
  window.show();
  
  /*
   * Check insertion
   *  - Insert 2 records
   *  - Check in model
   *  - Check in form
   *
   * Note:
   *  Because of mdtSqlFormWidget's internall state machine,
   *  witch runs asynchronousliy, we must wait between each action.
   */
  rowCount = model.rowCount();
  // Insert a record
  QTest::qWait(50);
  sqlFormWidget->insert();
  QTest::qWait(50);
  QVERIFY(leFirstName->isEnabled());
  QVERIFY(leFirstName->text().isEmpty());
  QVERIFY(leRemarks->isEnabled());
  QVERIFY(leRemarks->text().isEmpty());
  leFirstName->clear();
  QTest::keyClicks(leFirstName, "New name 1");
  leRemarks->clear();
  QTest::keyClicks(leRemarks, "New remark 1");
  sqlFormWidget->submit();
  QTest::qWait(50);
  // Check that model was updated
  QVERIFY(model.rowCount() > rowCount);
  rowCount = model.rowCount();
  row = sqlFormWidget->currentRow();
  data = model.data(model.index(row, model.fieldIndex("first_name")));
  QCOMPARE(data.toString(), QString("New name 1"));
  data = model.data(model.index(row, model.fieldIndex("remarks")));
  QCOMPARE(data.toString(), QString("New remark 1"));
  // Check that currentData() works
  QCOMPARE(sqlFormWidget->currentData("first_name"), QVariant("New name 1"));
  QCOMPARE(sqlFormWidget->currentData("remarks"), QVariant("New remark 1"));
  // Check that widget displays the correct row
  QCOMPARE(leFirstName->text(), QString("New name 1"));
  QCOMPARE(leRemarks->text(), QString("New remark 1"));
  // Insert a record
  sqlFormWidget->insert();
  QTest::qWait(50);
  QVERIFY(leFirstName->isEnabled());
  QVERIFY(leFirstName->text().isEmpty());
  QVERIFY(leRemarks->isEnabled());
  QVERIFY(leRemarks->text().isEmpty());
  leFirstName->clear();
  QTest::keyClicks(leFirstName, "New name 2");
  leRemarks->clear();
  QTest::keyClicks(leRemarks, "New remark 2");
  sqlFormWidget->submit();
  QTest::qWait(50);
  // Check that model was updated
  QVERIFY(model.rowCount() > rowCount);
  rowCount = model.rowCount();
  row = sqlFormWidget->currentRow();
  data = model.data(model.index(row, model.fieldIndex("first_name")));
  QCOMPARE(data.toString(), QString("New name 2"));
  data = model.data(model.index(row, model.fieldIndex("remarks")));
  QCOMPARE(data.toString(), QString("New remark 2"));
  // Check that widget displays the correct row
  QCOMPARE(leFirstName->text(), QString("New name 2"));
  QCOMPARE(leRemarks->text(), QString("New remark 2"));
  // Try to insert a record with no name - must fail
  sqlFormWidget->insert();
  QTest::qWait(50);
  QCOMPARE(leFirstName->text(), QString(""));
  QCOMPARE(leRemarks->text(), QString(""));
  leRemarks->clear();
  QTest::keyClicks(leRemarks, "New remark ...");
  // Catch and accept the message box that will pop-up
  QTimer::singleShot(50, this, SLOT(acceptModalDialog()));
  sqlFormWidget->submit();
  QTest::qWait(100);
  /*
   * We cannot check now, beacuse new row was inserted in model
   * We will revert, then check that new inserted row is suppressed
   */
  // Catch the message box that will pop-up and click Yes button
  QTimer::singleShot(50, this, SLOT(clickMessageBoxButtonYes()));
  sqlFormWidget->revert();
  QTest::qWait(100);
  QVERIFY(model.rowCount() == rowCount);
  /*
   * Check edition
   *  - Edit current row
   *  - Check in model
   *  - Check in form
   */
  // Edit in form and submit
  leFirstName->clear();
  QTest::keyClicks(leFirstName, "Edit name A");
  leRemarks->clear();
  QTest::keyClicks(leRemarks, "Edit remark A");
  sqlFormWidget->submit();
  QTest::qWait(50);
  // Check that model was updated
  QVERIFY(model.rowCount() == rowCount);
  row = sqlFormWidget->currentRow();
  data = model.data(model.index(row, model.fieldIndex("first_name")));
  QCOMPARE(data.toString(), QString("Edit name A"));
  data = model.data(model.index(row, model.fieldIndex("remarks")));
  QCOMPARE(data.toString(), QString("Edit remark A"));
  // Check that widget displays the correct row
  QCOMPARE(leFirstName->text(), QString("Edit name A"));
  QCOMPARE(leRemarks->text(), QString("Edit remark A"));
  /*
   * Check delete:
   *  - One time on accepting warning message box
   *  - One time rejecting warning message box
   */
  // Catch the message box that will pop-up and click No button
  QTimer::singleShot(50, this, SLOT(clickMessageBoxButtonNo()));
  sqlFormWidget->remove();
  QTest::qWait(100);
  // Check that nothing was removed and that index did not change
  QVERIFY(model.rowCount() == rowCount);
  row = sqlFormWidget->currentRow();
  data = model.data(model.index(row, model.fieldIndex("first_name")));
  QCOMPARE(data.toString(), QString("Edit name A"));
  data = model.data(model.index(row, model.fieldIndex("remarks")));
  QCOMPARE(data.toString(), QString("Edit remark A"));
  // Check that widget displays the correct row
  QCOMPARE(leFirstName->text(), QString("Edit name A"));
  QCOMPARE(leRemarks->text(), QString("Edit remark A"));
  // Catch the message box that will pop-up and click Yes button
  QTimer::singleShot(50, this, SLOT(clickMessageBoxButtonYes()));
  sqlFormWidget->remove();
  QTest::qWait(100);
  // Check that one row was removed and that widget displays something different than before
  QCOMPARE(model.rowCount(), (rowCount-1));
  rowCount = model.rowCount();
  QVERIFY(leFirstName->text() != QString("Edit name A"));
  QVERIFY(leRemarks->text() != QString("Edit remark A"));
  /*
   * Remove first row
   */
  sqlFormWidget->toFirst();
  QTest::qWait(50);
  row = sqlFormWidget->currentRow();
  QCOMPARE(row, 0);
  data = model.data(model.index(row, model.fieldIndex("first_name")));
  // Catch the message box that will pop-up and click Yes button
  QTimer::singleShot(50, this, SLOT(clickMessageBoxButtonYes()));
  sqlFormWidget->remove();
  QTest::qWait(100);
  // Check that one row was removed and that widget displays something different than before
  QCOMPARE(model.rowCount(), (rowCount-1));
  rowCount = model.rowCount();
  QVERIFY(leFirstName->text() != data.toString());
  // Remove all rows - prepare for next tests
  while(model.rowCount() > 0){
    // Catch the message box that will pop-up and click Yes button
    QTimer::singleShot(50, this, SLOT(clickMessageBoxButtonYes()));
    sqlFormWidget->remove();
    QTest::qWait(100);
  }
  // Check that widgets are empty and disabled
  QVERIFY(leFirstName->text().isEmpty());
  QVERIFY(!leFirstName->isEnabled());
  QVERIFY(leRemarks->text().isEmpty());
  QVERIFY(!leRemarks->isEnabled());
  /*
   * Check index changing on unsaved data
   *  - One time on accepting warning message box (-> submit, ok, ...)
   *  - One time rejecting warning message box
   */
  // Insert a record
  QTest::qWait(50);
  sqlFormWidget->insert();
  QTest::qWait(50);
  QVERIFY(leFirstName->isEnabled());
  QVERIFY(leFirstName->text().isEmpty());
  QVERIFY(leRemarks->isEnabled());
  QVERIFY(leRemarks->text().isEmpty());
  leFirstName->clear();
  QTest::keyClicks(leFirstName, "ABCD");
  leRemarks->clear();
  QTest::keyClicks(leRemarks, "1234");
  sqlFormWidget->submit();
  QTest::qWait(50);
  // Insert a record
  QTest::qWait(50);
  sqlFormWidget->insert();
  QTest::qWait(50);
  QVERIFY(leFirstName->isEnabled());
  QVERIFY(leFirstName->text().isEmpty());
  QVERIFY(leRemarks->isEnabled());
  QVERIFY(leRemarks->text().isEmpty());
  leFirstName->clear();
  QTest::keyClicks(leFirstName, "EFGH");
  leRemarks->clear();
  QTest::keyClicks(leRemarks, "5678");
  // Try to go to previous row - Catch message box and click Ok
  QTimer::singleShot(50, this, SLOT(clickMessageBoxButtonOk()));
  sqlFormWidget->toPrevious();
  QTest::qWait(100);
  // Check that we are allways in second row and save data
  QCOMPARE(leFirstName->text(), QString("EFGH"));
  QCOMPARE(leRemarks->text(), QString("5678"));
  sqlFormWidget->submit();
  QTest::qWait(50);
  // Go back and check that we are back to first row
  sqlFormWidget->toPrevious();
  QTest::qWait(50);
  QCOMPARE(leFirstName->text(), QString("ABCD"));
  QCOMPARE(leRemarks->text(), QString("1234"));
  // Go forward and check that second row was saved
  sqlFormWidget->toNext();
  QTest::qWait(50);
  QCOMPARE(leFirstName->text(), QString("EFGH"));
  QCOMPARE(leRemarks->text(), QString("5678"));

  // Re-enable foreign_keys support
  // For this test, we wont foreign_keys support
  sql = "PRAGMA foreign_keys = ON";
  QVERIFY(q.exec(sql));

  /*
   * Play
   */
  /*
  while(window.isVisible()){
    QTest::qWait(1000);
  }
  */
}

void mdtDatabaseTest::sqlTableWidgetTest()
{
  mdtSqlTableWidget *sqlTableWidget;
  mdtSqlWindow window;
  QSqlTableModel model;
  ///QWidget *w;
  ///int rowCount;
  ///int row;
  QVariant data;

  // Setup model + form view
  model.setTable("Client");
  model.select();
  sqlTableWidget = new mdtSqlTableWidget;
  ///QTest::qWait(50);
  sqlTableWidget->setModel(&model);
  // Setup window
  window.setSqlWidget(sqlTableWidget);
  window.enableNavigation();
  window.enableEdition();

  window.show();

  /*
   * Play
   */
  /*
  while(window.isVisible()){
    QTest::qWait(1000);
  }
  */
}

void mdtDatabaseTest::sortFilterProxyModelTest()
{
  QSqlTableModel model;
  mdtSortFilterProxyModel proxy;
  QTableView view;
  QSqlQuery q;
  QString sql;
  QModelIndex index;

  // Create a table
    /*
   * Create Addresses table
   */
  sql = "CREATE TABLE 'somedata' (";
  sql += "'id_PK' INTEGER PRIMARY KEY AUTOINCREMENT, ";
  sql += "'name' VARCHAR(50) , ";
  sql += "'remarks' VARCHAR(50) );";
  QVERIFY(q.exec(sql));

  // Insert some data in DB
  QVERIFY(q.exec("INSERT INTO 'somedata' ('name', 'remarks') VALUES('Andy', 'REM 1')"));
  QVERIFY(q.exec("INSERT INTO 'somedata' ('name', 'remarks') VALUES('Bety', 'REM 3')"));
  QVERIFY(q.exec("INSERT INTO 'somedata' ('name', 'remarks') VALUES('Andy', 'REM 10')"));
  QVERIFY(q.exec("INSERT INTO 'somedata' ('name', 'remarks') VALUES('Andy', 'REM 2')"));
  QVERIFY(q.exec("INSERT INTO 'somedata' ('name', 'remarks') VALUES('Bety', 'REM 1')"));
  QVERIFY(q.exec("INSERT INTO 'somedata' ('name', 'remarks') VALUES('Andy', 'REM 3')"));
  QVERIFY(q.exec("INSERT INTO 'somedata' ('name', 'remarks') VALUES('Andy', 'REM 15')"));
  QVERIFY(q.exec("INSERT INTO 'somedata' ('name', 'remarks') VALUES('Bety', 'REM 10')"));
  QVERIFY(q.exec("INSERT INTO 'somedata' ('name', 'remarks') VALUES('Bety', 'REM 2')"));
  QVERIFY(q.exec("INSERT INTO 'somedata' ('name', 'remarks') VALUES('Bety', 'REM 15')"));

  // Setup model and proxy
  model.setTable("somedata");
  proxy.setSourceModel(&model);
  proxy.addColumnToSortOrder(1);
  proxy.addColumnToSortOrder("remarks");
  view.setModel(&proxy);
  model.select();

  view.sortByColumn(0);
  view.show();

  // Check sorting
  QCOMPARE(proxy.rowCount(), 10);
  index = proxy.index(0, 1);
  QCOMPARE(proxy.data(index).toString(), QString("Andy"));
  index = proxy.index(1, 1);
  QCOMPARE(proxy.data(index).toString(), QString("Andy"));
  index = proxy.index(2, 1);
  QCOMPARE(proxy.data(index).toString(), QString("Andy"));
  index = proxy.index(3, 1);
  QCOMPARE(proxy.data(index).toString(), QString("Andy"));
  index = proxy.index(4, 1);
  QCOMPARE(proxy.data(index).toString(), QString("Andy"));
  index = proxy.index(0, 2);
  QCOMPARE(proxy.data(index).toString(), QString("REM 1"));
  index = proxy.index(1, 2);
  QCOMPARE(proxy.data(index).toString(), QString("REM 2"));
  index = proxy.index(2, 2);
  QCOMPARE(proxy.data(index).toString(), QString("REM 3"));
  index = proxy.index(3, 2);
  QCOMPARE(proxy.data(index).toString(), QString("REM 10"));
  index = proxy.index(4, 2);
  QCOMPARE(proxy.data(index).toString(), QString("REM 15"));
  index = proxy.index(5, 1);
  QCOMPARE(proxy.data(index).toString(), QString("Bety"));
  index = proxy.index(6, 1);
  QCOMPARE(proxy.data(index).toString(), QString("Bety"));
  index = proxy.index(7, 1);
  QCOMPARE(proxy.data(index).toString(), QString("Bety"));
  index = proxy.index(8, 1);
  QCOMPARE(proxy.data(index).toString(), QString("Bety"));
  index = proxy.index(9, 1);
  QCOMPARE(proxy.data(index).toString(), QString("Bety"));
  index = proxy.index(5, 2);
  QCOMPARE(proxy.data(index).toString(), QString("REM 1"));
  index = proxy.index(6, 2);
  QCOMPARE(proxy.data(index).toString(), QString("REM 2"));
  index = proxy.index(7, 2);
  QCOMPARE(proxy.data(index).toString(), QString("REM 3"));
  index = proxy.index(8, 2);
  QCOMPARE(proxy.data(index).toString(), QString("REM 10"));
  index = proxy.index(9, 2);
  QCOMPARE(proxy.data(index).toString(), QString("REM 15"));

  // delete created table
  QVERIFY(q.exec("DROP TABLE 'somedata'"));

  /*
   * Play
   */
  /*
  while(view.isVisible()){
    QTest::qWait(1000);
  }
  */
}





void mdtDatabaseTest::sqlDialogTest()
{
  mdtSqlDialog dialog;
  QSqlTableModel parentModel;
  mdtSqlFormWidget *sqlFormWidget;
  Ui::mdtSqlFormWidgetTestForm form;
  QSqlTableModel childModel;
  mdtSqlRelation *relation;
  mdtSqlTableWidget *addressWidget;

  // Setup parent model + widget
  parentModel.setTable("Client");
  QVERIFY(parentModel.select());
  sqlFormWidget = new mdtSqlFormWidget;
  sqlFormWidget->setModel(&parentModel);
  form.setupUi(sqlFormWidget);
  sqlFormWidget->mapFormWidgets("fld_first_name");
  // Setup child model
  childModel.setTable("Address");
  childModel.select();
  // Setup relation
  relation = new mdtSqlRelation;
  relation->setParentModel(&parentModel);
  relation->setChildModel(&childModel);
  QVERIFY(relation->addRelation("id_PK", "id_client_FK", true));
  // Setup child table widget
  addressWidget = new mdtSqlTableWidget;
  addressWidget->setModel(&childModel);
  // Add addressWidget as child of sqlFormWidget
  sqlFormWidget->addChildWidget(addressWidget, relation);
  // Setup dialog
  dialog.setSqlWidget(sqlFormWidget);
  dialog.addChildWidget(addressWidget, "Address");
  dialog.setCurrentRow(0);
  dialog.setCurrentRow("Id_PK", 2);
  QTimer::singleShot(50, &dialog, SLOT(accept()));
  dialog.exec();

  /*
   * Play
   */
  /*
  while(dialog.isVisible()){
    QTest::qWait(1000);
  }
  */
}

void mdtDatabaseTest::sqlFormWindowTest()
{
  mdtSqlFormWindow fw;
  Ui::mdtSqlFormWidgetTestForm *uif = new Ui::mdtSqlFormWidgetTestForm;

  // Setup Ui
  uif->setupUi(fw.mainSqlWidget());
  // Try some errors
  QVERIFY(!fw.setTable("ABCD"));
  QVERIFY(!fw.addChildTable("1234"));
  // Setup form
  QVERIFY(fw.setTable("Client"));
  QVERIFY(fw.addChildTable("Address", tr("Client's addresses")));
  QVERIFY(fw.addRelation("id_PK", "Address", "id_client_FK"));
  fw.sqlWindow()->enableEdition();
  fw.sqlWindow()->enableNavigation();
  fw.show();
  // Check that widgets can be found
  QVERIFY(fw.sqlWidget("HuHbzg") == 0);
  QVERIFY(fw.sqlWidget("Client") != 0);
  QVERIFY(fw.sqlWidget("Address") != 0);
  QVERIFY(fw.sqlFormWidget("Client") != 0);
  QVERIFY(fw.sqlTableWidget("Client") == 0);
  QVERIFY(fw.sqlFormWidget("Address") == 0);
  QVERIFY(fw.sqlTableWidget("Address") != 0);
  // Check that models can be found
  QVERIFY(fw.model("JKh k") == 0);
  QVERIFY(fw.model("Client") != 0);
  QVERIFY(fw.model("Address") != 0);
  // Check currentRow
  QCOMPARE(fw.currentRow("Client"), 0);
  QVERIFY(fw.currentRow("jhjkh") < 0);
  // Check currentValue
  QVERIFY(fw.currentData("Client", "id_PK").isValid());
  QVERIFY(!fw.currentData("AA", "id_PK").isValid());
  QVERIFY(!fw.currentData("Client", "JJ").isValid());

  /*
   * Play
   */
  /*
  while(fw.sqlWindow()->isVisible()){
    QTest::qWait(1000);
  }
  */

  delete uif;
}

void mdtDatabaseTest::sqlFormDialogTest()
{
  mdtSqlFormDialog fd;
  Ui::mdtSqlFormWidgetTestForm *uif = new Ui::mdtSqlFormWidgetTestForm;

  // Setup Ui
  uif->setupUi(fd.mainSqlWidget());
  // Try some errors
  QVERIFY(!fd.setTable("ABCD"));
  QVERIFY(!fd.addChildTable("1234"));
  // Setup form
  QVERIFY(fd.setTable("Client"));
  QVERIFY(fd.addChildTable("Address", tr("Client's addresses")));
  QVERIFY(fd.addRelation("id_PK", "Address", "id_client_FK"));
  // Check that widgets can be found
  QVERIFY(fd.sqlWidget("HuHbzg") == 0);
  QVERIFY(fd.sqlWidget("Client") != 0);
  QVERIFY(fd.sqlWidget("Address") != 0);
  QVERIFY(fd.sqlFormWidget("Client") != 0);
  QVERIFY(fd.sqlTableWidget("Client") == 0);
  QVERIFY(fd.sqlFormWidget("Address") == 0);
  QVERIFY(fd.sqlTableWidget("Address") != 0);
  // Check that models can be found
  QVERIFY(fd.model("JKh k") == 0);
  QVERIFY(fd.model("Client") != 0);
  QVERIFY(fd.model("Address") != 0);
  // Check currentRow
  QCOMPARE(fd.currentRow("Client"), 0);
  QVERIFY(fd.currentRow("jhjkh") < 0);
  // Check currentValue
  QVERIFY(fd.currentData("Client", "id_PK").isValid());
  QVERIFY(!fd.currentData("AA", "id_PK").isValid());
  QVERIFY(!fd.currentData("Client", "JJ").isValid());

  QTimer::singleShot(50, fd.sqlDialog(), SLOT(accept()));
  fd.exec();

  /*
   * Play
   */
  /*
  while(fw.sqlWindow()->isVisible()){
    QTest::qWait(1000);
  }
  */

  delete uif;
}

void mdtDatabaseTest::sqlSelectionDialogTest()
{
  mdtSqlSelectionDialog *dialog;
  QSqlQueryModel model;
  QSqlQuery q;
  QString sql;
  QModelIndex index;

  // Create a table
  sql = "CREATE TABLE 'somedata' (";
  sql += "'id_PK' INTEGER PRIMARY KEY AUTOINCREMENT, ";
  sql += "'name' VARCHAR(50) , ";
  sql += "'remarks' VARCHAR(50) );";
  QVERIFY(q.exec(sql));
  // Insert some data in DB
  QVERIFY(q.exec("INSERT INTO 'somedata' ('id_PK', 'name', 'remarks') VALUES(1, 'Andy', 'REM Andy')"));
  QVERIFY(q.exec("INSERT INTO 'somedata' ('id_PK', 'name', 'remarks') VALUES(2, 'Bety', 'REM Bety')"));

  /*
   * Check single selection with 1 field, at column 0
   */
  sql = "SELECT * FROM somedata";
  model.setQuery(sql);
  QVERIFY(!model.lastError().isValid());
  dialog = new mdtSqlSelectionDialog;
  dialog->setModel(&model);
  dialog->addSelectionResultColumn("id_PK");
  dialog->selectRows("name", "Andy");
  QTimer::singleShot(50, dialog, SLOT(accept()));
  QVERIFY(dialog->exec() == QDialog::Accepted);
  QCOMPARE(dialog->selectionResult().size(), 1);
  QCOMPARE(dialog->selectionResult().at(0), QVariant(1));
  delete dialog;
  /*
   * Check single selection with 1 field, at column 2
   */
  sql = "SELECT name, remarks, id_PK FROM somedata";
  model.setQuery(sql);
  QVERIFY(!model.lastError().isValid());
  dialog = new mdtSqlSelectionDialog;
  dialog->setModel(&model);
  dialog->addSelectionResultColumn("id_PK");
  dialog->selectRows("name", "Andy");
  QTimer::singleShot(50, dialog, SLOT(accept()));
  QVERIFY(dialog->exec() == QDialog::Accepted);
  QCOMPARE(dialog->selectionResult().size(), 1);
  QCOMPARE(dialog->selectionResult().at(0), QVariant(1));
  delete dialog;
  /*
   * Check single selection with 2 fields
   */
  sql = "SELECT * FROM somedata";
  model.setQuery(sql);
  QVERIFY(!model.lastError().isValid());
  dialog = new mdtSqlSelectionDialog;
  dialog->setModel(&model);
  dialog->addSelectionResultColumn("id_PK");
  dialog->addSelectionResultColumn("name");
  dialog->selectRows("id_PK", 2);
  QTimer::singleShot(50, dialog, SLOT(accept()));
  QVERIFY(dialog->exec() == QDialog::Accepted);
  QCOMPARE(dialog->selectionResult().size(), 2);
  QCOMPARE(dialog->selectionResult().at(0), QVariant(2));
  QCOMPARE(dialog->selectionResult().at(1), QVariant("Bety"));
  delete dialog;
  /*
   * Check multiple selection with 1 field
   */
  sql = "SELECT * FROM somedata";
  model.setQuery(sql);
  QVERIFY(!model.lastError().isValid());
  dialog = new mdtSqlSelectionDialog;
  dialog->setModel(&model, true);
  dialog->addSelectionResultColumn("id_PK");
  dialog->selectRows("id_PK", 1);
  dialog->selectRows("id_PK", 2);
  QTimer::singleShot(50, dialog, SLOT(accept()));
  QVERIFY(dialog->exec() == QDialog::Accepted);
  QCOMPARE(dialog->selectionResults().size(), 2);
  QCOMPARE(dialog->selectionResults().at(0).data(), QVariant(1));
  QCOMPARE(dialog->selectedData(0, "id_PK"), QVariant(1));
  QCOMPARE(dialog->selectionResults().at(1).data(), QVariant(2));
  QCOMPARE(dialog->selectedData(1, "id_PK"), QVariant(2));
  delete dialog;
  /*
   * Check multiple selection with 2 fields
   */
  sql = "SELECT * FROM somedata";
  model.setQuery(sql);
  QVERIFY(!model.lastError().isValid());
  dialog = new mdtSqlSelectionDialog;
  dialog->setModel(&model, true);
  dialog->addSelectionResultColumn("id_PK");
  dialog->addSelectionResultColumn("name");
  dialog->selectRows("id_PK", 1);
  dialog->selectRows("id_PK", 2);
  QTimer::singleShot(50, dialog, SLOT(accept()));
  QVERIFY(dialog->exec() == QDialog::Accepted);
  QCOMPARE(dialog->selectionResults().size(), 4);
  QCOMPARE(dialog->selectionResults().at(0).data(), QVariant(1));
  QCOMPARE(dialog->selectedData(0, "id_PK"), QVariant(1));
  QCOMPARE(dialog->selectionResults().at(1).data(), QVariant("Andy"));
  QCOMPARE(dialog->selectedData(0, "name"), QVariant("Andy"));
  QCOMPARE(dialog->selectionResults().at(2).data(), QVariant(2));
  QCOMPARE(dialog->selectedData(1, "id_PK"), QVariant(2));
  QCOMPARE(dialog->selectionResults().at(3).data(), QVariant("Bety"));
  QCOMPARE(dialog->selectedData(1, "name"), QVariant("Bety"));
  delete dialog;

  // delete created table
  QVERIFY(q.exec("DROP TABLE 'somedata'"));
}

void mdtDatabaseTest::clickMessageBoxButton(QMessageBox::StandardButton button)
{
  QMessageBox *mdsBox;
  QAbstractButton *btn;

  if(QApplication::activeModalWidget() != 0){
    mdsBox = dynamic_cast<QMessageBox*>(QApplication::activeModalWidget());
    if(mdsBox != 0){
      qDebug() << "Found a message box, name: " << mdsBox->objectName();
      // Get requested button
      btn = mdsBox->button(button);
      if(btn != 0){
        btn->click();
      }
    }
  }
}

void mdtDatabaseTest::clickMessageBoxButtonYes()
{
  clickMessageBoxButton(QMessageBox::Yes);
}

void mdtDatabaseTest::clickMessageBoxButtonNo()
{
  clickMessageBoxButton(QMessageBox::No);
}

void mdtDatabaseTest::clickMessageBoxButtonCancel()
{
  clickMessageBoxButton(QMessageBox::Cancel);
}

void mdtDatabaseTest::clickMessageBoxButtonOk()
{
  clickMessageBoxButton(QMessageBox::Ok);
}

void mdtDatabaseTest::acceptModalDialog()
{
  QDialog *dlg;

  if(QApplication::activeModalWidget() != 0){
    dlg = dynamic_cast<QDialog*>(QApplication::activeModalWidget());
    if(dlg != 0){
      qDebug() << "Found a dilaog, name: " << dlg->objectName();
      dlg->accept();
    }
  }
}

void mdtDatabaseTest::rejectModalDialog()
{
  QDialog *dlg;

  if(QApplication::activeModalWidget() != 0){
    dlg = dynamic_cast<QDialog*>(QApplication::activeModalWidget());
    if(dlg != 0){
      qDebug() << "Found a dilaog, name: " << dlg->objectName();
      dlg->reject ();
    }
  }
}

void mdtDatabaseTest::mdtSqlTableModelTest()
{
}

/**
void mdtDatabaseTest::mdtSqlDataWidgetMapperTest()
{
  mdtSqlDataWidgetMapper mapper;
  mdtSqlTableModel model;
  QLineEdit id, first_name, remarks;
  
  // Init model
  model.setTable("Client");
  model.select();

  // Init mapper
  mapper.setModel(&model);
  mapper.addMapping(&id, 0);
  mapper.addMapping(&first_name, 1);
  mapper.addMapping(&remarks, 2);
  mapper.toFirst();

  // Check that data are available in widgets
  QVERIFY(!id.text().isEmpty());
  QVERIFY(!first_name.text().isEmpty());
}
*/


void mdtDatabaseTest::cleanupTestCase()
{
  QFile::remove(pvDbFileInfo.filePath());
}

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtDatabaseTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
