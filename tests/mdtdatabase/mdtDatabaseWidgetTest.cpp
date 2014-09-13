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
#include "mdtDatabaseWidgetTest.h"
#include "mdtSqlSchemaTable.h"
#include "mdtApplication.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlTableSelectionItem.h"
#include "mdtSqlTableSelectionRow.h"
#include "mdtSqlTableSelection.h"
#include "mdtSqlRelation.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlFieldHandler.h"
#include "mdtAbstractSqlTableController.h"
#include "mdtSqlDataWidgetController.h"
#include "mdtSqlTableViewController.h"
#include "mdtUiMessageHandler.h"
#include <QTemporaryFile>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDateTimeEdit>
#include <QAbstractButton>
#include <QComboBox>
#include <QString>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QFile>
#include <QVariant>
#include <QTimer>
#include <QAbstractButton>
#include <QModelIndex>
#include <QList>
#include <QFileInfo>
#include <QTableView>
#include <QItemSelectionModel>
#include <QVBoxLayout>

#include <QDebug>

/*
 * sqlDataWidgetControllerTest implementation
 */

sqlDataWidgetControllerTestWidget::sqlDataWidgetControllerTestWidget(QWidget *parent)
  :QWidget(parent)
{
  QVBoxLayout *l = new QVBoxLayout;
  fld_FirstName = new QLineEdit;
  fld_FirstName->setObjectName("fld_FirstName");
  fld_Remarks = new QLineEdit;
  fld_Remarks->setObjectName("fld_Remarks");
  l->addWidget(fld_FirstName);
  l->addWidget(fld_Remarks);
  setLayout(l);
}

void sqlDataWidgetControllerTestWidget::setToFirstEnableState(bool enable)
{
  toFirstEnabled = enable;
}

void sqlDataWidgetControllerTestWidget::setToLastEnableState(bool enable)
{
  toLastEnabled = enable;
}

void sqlDataWidgetControllerTestWidget::setToNextEnableState(bool enable)
{
  toNextEnabled = enable;
}

void sqlDataWidgetControllerTestWidget::setToPreviousEnableState(bool enable)
{
  toPreviousEnabled = enable;
}

void sqlDataWidgetControllerTestWidget::setInsertEnableState(bool enable)
{
  insertEnabled = enable;
}

void sqlDataWidgetControllerTestWidget::setRemoveEnableState(bool enable)
{

  removeEnabled = enable;
}

void sqlDataWidgetControllerTestWidget::setSubmitEnableState(bool enable)
{
  submitEnabled = enable;
}

void sqlDataWidgetControllerTestWidget::setRevertEnableState(bool enable)
{
  revertEnabled = enable;
}


/*
 * Test implementation
 */

void mdtDatabaseWidgetTest::initTestCase()
{
  createDatabaseSchema();
  QVERIFY(pvDatabaseManager.database().isOpen());
  /**
  populateTestDatabase();
  clearTestDatabaseData();
  */
}

void mdtDatabaseWidgetTest::cleanupTestCase()
{
  QFile::remove(pvDatabaseManager.database().databaseName());
}

void mdtDatabaseWidgetTest::sqlFieldHandlerTest()
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
  fh.setData("Row 2 from DB");
  ///le.setText("Row 2 from DB");
  ///fh.updateFlags();
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
  fh.setData("Row 3 from DB");
  ///le.setText("Row 3 from DB");
  ///fh.updateFlags();
  // Check field handler flags
  QVERIFY(!fh.isNull());
  QVERIFY(!fh.dataWasEdited());
  // We change the row
  fh.setData(QVariant());
  ///le.setText("");
  ///fh.updateFlags();
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

  // Setup field
  field.setAutoValue(false);
  field.setLength(10);
  field.setRequiredStatus(QSqlField::Required);
  field.setReadOnly(false);
  fh.setField(field);

  /*
   * Tests with non editable QComboBox
   */
  QComboBox cb;
  cb.show();
  cb.addItem("");
  cb.addItem("V");
  cb.addItem("A");
  fh.setDataWidget(&cb);
  // Check field handler flags and data
  QVERIFY(fh.isNull());
  QVERIFY(!fh.dataWasEdited());
  QVERIFY(!fh.isReadOnly());
  QVERIFY(fh.data().isNull());
  // Check widget flags
  QVERIFY(cb.isEnabled());
  // User selects
  cb.setCurrentIndex(1);
  // Check field handler flags and data
  QVERIFY(!fh.isNull());
  QVERIFY(fh.dataWasEdited());
  QCOMPARE(fh.data(), QVariant("V"));
  // User selects first (empty) item
  cb.setCurrentIndex(0);
  // Check field handler flags and data
  QVERIFY(fh.isNull());
  QVERIFY(fh.dataWasEdited());
  QVERIFY(fh.data().isNull());
  // User selects
  cb.setCurrentIndex(2);
  // Check field handler flags and data
  QVERIFY(!fh.isNull());
  QVERIFY(fh.dataWasEdited());
  QCOMPARE(fh.data(), QVariant("A"));
  // Set data from DB - Existing value
  fh.setData("V");
  QVERIFY(!fh.isNull());
  QVERIFY(!fh.dataWasEdited());
  QCOMPARE(fh.data(), QVariant("V"));
  // Set data from DB - Non existing value
  fh.setData("P");
  QVERIFY(fh.isNull());
  QVERIFY(!fh.dataWasEdited());
  QVERIFY(fh.data().isNull());

  // Setup field
  field.setAutoValue(false);
  field.setLength(10);
  field.setRequiredStatus(QSqlField::Required);
  field.setReadOnly(false);
  fh.setField(field);

  /*
   * Tests with QPlainTextEdit
   */
  QPlainTextEdit pte;
  fh.setDataWidget(&pte);
  pte.show();
  // Check field handler flags
  QVERIFY(fh.isNull());
  QVERIFY(!fh.dataWasEdited());
  QVERIFY(!fh.isReadOnly());
  // Check widget flags
  QVERIFY(pte.isEnabled());
  QVERIFY(pte.toPlainText().isEmpty());
  QVERIFY(!pte.isReadOnly());
  // User edit ...
  QTest::keyClicks(&pte, "ABCD");
  // Check field handler flags
  QVERIFY(!fh.isNull());
  QVERIFY(fh.dataWasEdited());
  // User edit ...
  QTest::keyClick(&pte, Qt::Key_Backspace);
  QTest::keyClick(&pte, Qt::Key_Backspace);
  QTest::keyClick(&pte, Qt::Key_Backspace);
  QTest::keyClick(&pte, Qt::Key_Backspace);
  // Check field handler flags
  QVERIFY(fh.isNull());
  QVERIFY(fh.dataWasEdited());
  // User edit ...
  QTest::keyClicks(&pte, "123");
  // Check field handler flags
  QVERIFY(!fh.isNull());
  QVERIFY(fh.dataWasEdited());
  // User edit ...
  pte.clear();
  QTest::keyClicks(&pte, " ");
  // Check field handler flags
  QVERIFY(fh.isNull());
  QVERIFY(fh.dataWasEdited());
  // Field is mandatory, so check must fail
  QVERIFY(!fh.checkBeforeSubmit());
  // Line edit must be empty now
  QVERIFY(pte.toPlainText().isEmpty());
  // User edit ...
  pte.clear();
  QTest::keyClicks(&pte, "ZA");
  // Check field handler flags
  QVERIFY(!fh.isNull());
  QVERIFY(fh.dataWasEdited());
  // Check must be OK now
  QVERIFY(fh.checkBeforeSubmit());
  // User edit to much
  /**
  pte.clear();
  QTest::keyClicks(&pte, "123456789ABC");
  // Check field handler flags
  QVERIFY(!fh.isNull());
  QCOMPARE(pte.toPlainText(), QString("123456789A"));
  */
  // Set read only
  fh.setReadOnly(true);
  QVERIFY(fh.isReadOnly());
  QVERIFY(pte.isReadOnly());
  // Set editable again
  fh.setReadOnly(false);
  QVERIFY(!fh.isReadOnly());
  QVERIFY(!pte.isReadOnly());
  // Now we change the row
  fh.setData("Row 2 from DB");
  // Check field handler flags
  QVERIFY(!fh.isNull());
  QVERIFY(!fh.dataWasEdited());
  // User edit ...
  pte.clear();
  QTest::keyClicks(&pte, " ");
  // Check field handler flags
  QVERIFY(fh.isNull());
  QVERIFY(fh.dataWasEdited());
  // We change the row
  fh.setData("Row 3 from DB");
  // Check field handler flags
  QVERIFY(!fh.isNull());
  QVERIFY(!fh.dataWasEdited());
  // We change the row
  fh.setData(QVariant());
  // Check field handler flags
  QVERIFY(fh.isNull());
  QVERIFY(!fh.dataWasEdited());
  // Setup field as read only
  field.setAutoValue(false);
  field.setLength(10);
  field.setRequiredStatus(QSqlField::Required);
  field.setReadOnly(true);
  fh.setField(field);
  // Check widget flags
  QVERIFY(pte.isEnabled());
  QVERIFY(pte.isReadOnly());

  
  /*
   * Play
   */
  /*
  while(cb.isVisible()){
    QTest::qWait(1000);
  }
  */
}

void mdtDatabaseWidgetTest::sqlDataWidgetControllerTest()
{
  QSqlQuery q(pvDatabaseManager.database());
  sqlDataWidgetControllerTestWidget w;
  ///std::shared_ptr<mdtUiMessageHandler> messageHandler(new mdtUiMessageHandler(&w));
  mdtSqlDataWidgetController wc;
  std::shared_ptr<QSqlTableModel> m1(new QSqlTableModel(0, pvDatabaseManager.database()));
  std::shared_ptr<QSqlTableModel> m2(new QSqlTableModel(0, pvDatabaseManager.database()));
  std::shared_ptr<QSqlTableModel> model;
  mdtSqlRelationInfo relationInfo;
  QVariant data;
  bool ok;

  // For this test, we wont foreign_keys support
  QVERIFY(q.exec("PRAGMA foreign_keys = OFF"));
  // Create test data
  populateTestDatabase();
  // Setup
  ///wc.setMessageHandler(messageHandler);
  ///connect(&wc, SIGNAL(globalWidgetEnableStateChanged(bool)), &w, SLOT(setEnabled(bool)));
  connect(&wc, SIGNAL(toFirstEnabledStateChanged(bool)), &w, SLOT(setToFirstEnableState(bool)));
  connect(&wc, SIGNAL(toLastEnabledStateChanged(bool)), &w, SLOT(setToLastEnableState(bool)));
  connect(&wc, SIGNAL(toNextEnabledStateChanged(bool)), &w, SLOT(setToNextEnableState(bool)));
  connect(&wc, SIGNAL(toPreviousEnabledStateChanged(bool)), &w, SLOT(setToPreviousEnableState(bool)));
  connect(&wc, SIGNAL(insertEnabledStateChanged(bool)), &w, SLOT(setInsertEnableState(bool)));
  connect(&wc, SIGNAL(removeEnabledStateChanged(bool)), &w, SLOT(setRemoveEnableState(bool)));
  connect(&wc, SIGNAL(submitEnabledStateChanged(bool)), &w, SLOT(setSubmitEnableState(bool)));
  connect(&wc, SIGNAL(revertEnabledStateChanged(bool)), &w, SLOT(setRevertEnableState(bool)));
  // Initial state
  w.show();
  QVERIFY(w.fld_FirstName->text().isEmpty());
  QVERIFY(w.fld_Remarks->text().isEmpty());
  QVERIFY(wc.model().get() == 0);
  QVERIFY(wc.userFriendlyTableName().isEmpty());
  QCOMPARE(wc.rowCount(), 0);
  QVERIFY(wc.currentRow() < 0);
  QVERIFY(wc.currentState() == mdtAbstractSqlTableController::Stopped);

  qDebug() << "m1 usage: " << m1.use_count();
  qDebug() << "model usage: " << wc.model().use_count();

  /*
   * Setup a model ourself and set it to controller
   */
  m1->setTable("Client_tbl");
  wc.setModel(m1);
  QVERIFY(wc.model() == m1);
  QCOMPARE(wc.model()->tableName(), QString("Client_tbl"));
  QCOMPARE(wc.userFriendlyTableName(), QString("Client_tbl"));
  QCOMPARE(wc.rowCount(), 0);
  QVERIFY(wc.currentRow() < 0);
  QVERIFY(!wc.setCurrentRow(-1));
  QVERIFY(!wc.setCurrentRow(0));
  QVERIFY(wc.currentState() == mdtAbstractSqlTableController::Stopped);
  /*
   * Setup a second model, set it to controller,
   *  and check that first model is not alterred.
   */
  m2->setTable("Address_tbl");
  wc.setModel(m2);
  QVERIFY(wc.model() == m2);
  QCOMPARE(wc.model()->tableName(), QString("Address_tbl"));
  QCOMPARE(wc.userFriendlyTableName(), QString("Address_tbl"));
  QVERIFY(m1 != m2);
  QCOMPARE(m1->tableName(), QString("Client_tbl"));
  QCOMPARE(m2->tableName(), QString("Address_tbl"));
  /*
   * Set table name, and check that m2 is intact
   */
  wc.setTableName("Client_tbl", pvDatabaseManager.database(), "Clients");
  QCOMPARE(wc.model()->tableName(), QString("Client_tbl"));
  QCOMPARE(wc.userFriendlyTableName(), QString("Clients"));
  QCOMPARE(m2->tableName(), QString("Address_tbl"));
  // Reset m1 and m2
  m1.reset();
  m2.reset();
  QVERIFY(wc.model().get() != 0);
  QCOMPARE(wc.model()->tableName(), QString("Client_tbl"));
  QCOMPARE(wc.rowCount(), 0);
  QVERIFY(wc.currentRow() < 0);
  QVERIFY(!wc.setCurrentRow(-1));
  QVERIFY(!wc.setCurrentRow(0));
  // For next test steps, we use model pointer
  model = wc.model();
  QVERIFY(model.get() != 0);
  /*
   * Map widgets
   */
  QVERIFY(wc.mapFormWidgets(&w, "fld_FirstName"));
  // Check fields - Because state machine is not running, all widgets must be diseabled
  QVERIFY(!w.fld_FirstName->isEnabled());
  QVERIFY(!w.fld_Remarks->isEnabled());
  QVERIFY(w.fld_FirstName->text().isEmpty());
  QVERIFY(w.fld_Remarks->text().isEmpty());
  // Check control buttons states
  QVERIFY(!w.toFirstEnabled);
  QVERIFY(!w.toLastEnabled);
  QVERIFY(!w.toNextEnabled);
  QVERIFY(!w.toPreviousEnabled);
  QVERIFY(!w.submitEnabled);
  QVERIFY(!w.revertEnabled);
  QVERIFY(!w.insertEnabled); /// \todo Define the correct behaviour when model was never selected
  QVERIFY(!w.removeEnabled); /// \todo Define the correct behaviour when model was never selected
  /*
   * Check start/stop of state machine
   */
  QVERIFY(wc.currentState() == mdtAbstractSqlTableController::Stopped);
  wc.start();
  QVERIFY(wc.currentState() == mdtAbstractSqlTableController::Visualizing);
  wc.stop();
  QVERIFY(wc.currentState() == mdtAbstractSqlTableController::Stopped);
  wc.start();
  QVERIFY(wc.currentState() == mdtAbstractSqlTableController::Visualizing);
  // Check fields - Must be all OFF, because model was never selected
  QVERIFY(!w.fld_FirstName->isEnabled());
  QVERIFY(!w.fld_Remarks->isEnabled());
  QVERIFY(w.fld_FirstName->text().isEmpty());
  QVERIFY(w.fld_Remarks->text().isEmpty());
  // Check control buttons states - Must be all OFF, because model was never selected
  /**
  QVERIFY(!w.toFirstEnabled);
  QVERIFY(!w.toLastEnabled);
  QVERIFY(!w.toNextEnabled);
  QVERIFY(!w.toPreviousEnabled);
  QVERIFY(!w.submitEnabled);
  QVERIFY(!w.revertEnabled);
  QVERIFY(!w.insertEnabled);
  QVERIFY(!w.removeEnabled);
  */
  /*
   * Select data and check
   */
  QVERIFY(wc.select());
  QCOMPARE(wc.rowCount(), 4);
  QCOMPARE(wc.currentRow(), 0);
  QVERIFY(w.fld_FirstName->isEnabled());
  QVERIFY(w.fld_Remarks->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Andy"));
  QVERIFY(w.fld_Remarks->text().isEmpty());
  // Check control buttons states
  QVERIFY(!w.toFirstEnabled);
  QVERIFY(w.toLastEnabled);
  QVERIFY(w.toNextEnabled);
  QVERIFY(!w.toPreviousEnabled);
  QVERIFY(!w.submitEnabled);
  QVERIFY(!w.revertEnabled);
  QVERIFY(w.insertEnabled);
  QVERIFY(w.removeEnabled);
  // Check set current row
  QVERIFY(wc.setCurrentRow(0));
  QVERIFY(wc.setCurrentRow(1));
  QCOMPARE(wc.currentRow(), 1);
  QVERIFY(w.fld_FirstName->isEnabled());
  QVERIFY(w.fld_Remarks->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Bety"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark on Bety"));
  // Check control buttons states
  QVERIFY(w.toFirstEnabled);
  QVERIFY(w.toLastEnabled);
  QVERIFY(w.toNextEnabled);
  QVERIFY(w.toPreviousEnabled);
  QVERIFY(!w.submitEnabled);
  QVERIFY(!w.revertEnabled);
  QVERIFY(w.insertEnabled);
  QVERIFY(w.removeEnabled);
  /*
   * Check navigation - No edition
   */
  wc.toFirst();
  QCOMPARE(wc.currentRow(), 0);
  QVERIFY(w.fld_FirstName->isEnabled());
  QVERIFY(w.fld_Remarks->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Andy"));
  QVERIFY(w.fld_Remarks->text().isEmpty());
  QVERIFY(!w.toFirstEnabled);
  QVERIFY(w.toLastEnabled);
  QVERIFY(w.toNextEnabled);
  QVERIFY(!w.toPreviousEnabled);
  wc.toNext();
  QCOMPARE(wc.currentRow(), 1);
  QVERIFY(w.fld_FirstName->isEnabled());
  QVERIFY(w.fld_Remarks->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Bety"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark on Bety"));
  QVERIFY(w.toFirstEnabled);
  QVERIFY(w.toLastEnabled);
  QVERIFY(w.toNextEnabled);
  QVERIFY(w.toPreviousEnabled);
  wc.toPrevious();
  QCOMPARE(wc.currentRow(), 0);
  QVERIFY(w.fld_FirstName->isEnabled());
  QVERIFY(w.fld_Remarks->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Andy"));
  QVERIFY(w.fld_Remarks->text().isEmpty());
  QVERIFY(!w.toFirstEnabled);
  QVERIFY(w.toLastEnabled);
  QVERIFY(w.toNextEnabled);
  QVERIFY(!w.toPreviousEnabled);
  wc.toLast();
  QCOMPARE(wc.currentRow(), 3);
  QVERIFY(w.fld_FirstName->isEnabled());
  QVERIFY(w.fld_Remarks->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Charly"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark on Charly"));
  QVERIFY(w.toFirstEnabled);
  QVERIFY(!w.toLastEnabled);
  QVERIFY(!w.toNextEnabled);
  QVERIFY(w.toPreviousEnabled);
  wc.toFirst();
  QCOMPARE(wc.currentRow(), 0);
  QVERIFY(w.fld_FirstName->isEnabled());
  QVERIFY(w.fld_Remarks->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Andy"));
  QVERIFY(w.fld_Remarks->text().isEmpty());
  QVERIFY(!w.toFirstEnabled);
  QVERIFY(w.toLastEnabled);
  QVERIFY(w.toNextEnabled);
  QVERIFY(!w.toPreviousEnabled);
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
  QCOMPARE(wc.rowCount(), 4);
  QCOMPARE(wc.currentRow(), 0);
  // Insert a record
  wc.insert();
  QTest::qWait(50);
  QCOMPARE(wc.rowCount(), 5);
  QCOMPARE(wc.currentRow(), 4);
  QVERIFY(w.fld_FirstName->isEnabled());
  QVERIFY(w.fld_Remarks->isEnabled());
  QVERIFY(w.fld_FirstName->text().isEmpty());
  QVERIFY(w.fld_Remarks->text().isEmpty());
  QTest::keyClicks(w.fld_FirstName, "New name 1");
  QTest::keyClicks(w.fld_Remarks, "New remark 1");
  // Submit with asynch version
  wc.submit();
  QTest::qWait(1000); // Writing in DB can be very slow, f.ex. with Sqlite on HDD
  // Check that model was updated
  QCOMPARE(wc.rowCount(), 5);
  QCOMPARE(wc.currentRow(), 4);
  data = model->data(model->index(4, model->fieldIndex("FirstName")));
  QCOMPARE(data.toString(), QString("New name 1"));
  data = model->data(model->index(4, model->fieldIndex("Remarks")));
  QCOMPARE(data.toString(), QString("New remark 1"));
  // Check that data() works
  QCOMPARE(wc.data(4, "FirstName", ok), QVariant("New name 1"));
  QVERIFY(ok);
  QVERIFY(wc.data(200, "FirstName", ok).isNull());
  QVERIFY(!ok);
  QCOMPARE(wc.data(4, "FirstName"), QVariant("New name 1"));
  // Check that currentData() works
  QCOMPARE(wc.currentData("FirstName", ok), QVariant("New name 1"));
  QVERIFY(ok);
  QCOMPARE(wc.currentData("Remarks", ok), QVariant("New remark 1"));
  QVERIFY(ok);
  QCOMPARE(wc.currentData("FirstName"), QVariant("New name 1"));
  QCOMPARE(wc.currentData("Remarks"), QVariant("New remark 1"));
  // Check that widget displays the correct row
  QCOMPARE(w.fld_FirstName->text(), QString("New name 1"));
  QCOMPARE(w.fld_Remarks->text(), QString("New remark 1"));
  // Insert a record
  wc.insert();
  QTest::qWait(50);
  QCOMPARE(wc.rowCount(), 6);
  QCOMPARE(wc.currentRow(), 5);
  QVERIFY(w.fld_FirstName->isEnabled());
  QVERIFY(w.fld_Remarks->isEnabled());
  QVERIFY(w.fld_FirstName->text().isEmpty());
  QVERIFY(w.fld_Remarks->text().isEmpty());
  QTest::keyClicks(w.fld_FirstName, "New name 2");
  QTest::keyClicks(w.fld_Remarks, "New remark 2");
  // Submit with synch version
  QVERIFY(wc.submitAndWait());
  // Check that model was updated
  QCOMPARE(model->rowCount(), 6);
  QCOMPARE(wc.rowCount(), 6);
  QCOMPARE(wc.currentRow(), 5);
  data = model->data(model->index(5, model->fieldIndex("FirstName")));
  QCOMPARE(data.toString(), QString("New name 2"));
  data = model->data(model->index(5, model->fieldIndex("Remarks")));
  QCOMPARE(data.toString(), QString("New remark 2"));
  // Check that widget displays the correct row
  QCOMPARE(w.fld_FirstName->text(), QString("New name 2"));
  QCOMPARE(w.fld_Remarks->text(), QString("New remark 2"));
  // Try to insert a record with no name - must fail
  wc.insert();
  QTest::qWait(50);
  QCOMPARE(wc.rowCount(), 7);
  QCOMPARE(wc.currentRow(), 6);
  QVERIFY(w.fld_FirstName->text().isEmpty());
  QVERIFY(w.fld_Remarks->text().isEmpty());
  QTest::keyClicks(w.fld_Remarks, "New remark ...");
  wc.submit();
  QTest::qWait(50);
  QCOMPARE(wc.rowCount(), 7);
  QCOMPARE(wc.currentRow(), 6);
  /*
   * We cannot check now, beacuse new row was inserted in model
   * We will revert, then check that new inserted row is suppressed
   */
  wc.revert();
  QTest::qWait(50);
  QCOMPARE(wc.rowCount(), 6);
  QCOMPARE(wc.currentRow(), 5);
  // Check that widget displays the correct row
  QCOMPARE(w.fld_FirstName->text(), QString("New name 2"));
  QCOMPARE(w.fld_Remarks->text(), QString("New remark 2"));
  /*
   * Check edition
   *  - Edit current row
   *  - Check in model
   *  - Check in widgets
   */
  // Check control buttons states - before edition
  QVERIFY(w.toFirstEnabled);
  QVERIFY(!w.toLastEnabled);
  QVERIFY(!w.toNextEnabled);
  QVERIFY(w.toPreviousEnabled);
  QVERIFY(!w.submitEnabled);
  QVERIFY(!w.revertEnabled);
  QVERIFY(w.insertEnabled);
  QVERIFY(w.removeEnabled);
  // Edit in widgets and submit
  w.fld_FirstName->clear();
  w.fld_Remarks->clear();
  QTest::keyClicks(w.fld_FirstName, "Edit name A");
  QTest::keyClicks(w.fld_Remarks, "Edit remark A");
  // Check control buttons states - after edition
  QVERIFY(!w.toFirstEnabled);
  QVERIFY(!w.toLastEnabled);
  QVERIFY(!w.toNextEnabled);
  QVERIFY(!w.toPreviousEnabled);
  QVERIFY(w.submitEnabled);
  QVERIFY(w.revertEnabled);
  QVERIFY(!w.insertEnabled);
  QVERIFY(!w.removeEnabled);
  // Submit
  QVERIFY(wc.submitAndWait());
  // Check that model was updated
  QVERIFY(model->rowCount() == 6);
  data = model->data(model->index(5, model->fieldIndex("FirstName")));
  QCOMPARE(data.toString(), QString("Edit name A"));
  data = model->data(model->index(5, model->fieldIndex("Remarks")));
  QCOMPARE(data.toString(), QString("Edit remark A"));
  // Check that controller returns correct row count and current row
  QCOMPARE(wc.rowCount(), 6);
  QCOMPARE(wc.currentRow(), 5);
  // Check that widget displays the correct row
  QCOMPARE(w.fld_FirstName->text(), QString("Edit name A"));
  QCOMPARE(w.fld_Remarks->text(), QString("Edit remark A"));
  // Check control buttons states - after submit
  QVERIFY(w.toFirstEnabled);
  QVERIFY(!w.toLastEnabled);
  QVERIFY(!w.toNextEnabled);
  QVERIFY(w.toPreviousEnabled);
  QVERIFY(!w.submitEnabled);
  QVERIFY(!w.revertEnabled);
  QVERIFY(w.insertEnabled);
  QVERIFY(w.removeEnabled);
  /*
   * Check delete
   */
  QCOMPARE(wc.rowCount(), 6);
  QCOMPARE(wc.currentRow(), 5);
  wc.remove();
  QTest::qWait(1000); // Writing in DB can be very slow, f.ex. with Sqlite on HDD
  // Check that model was updated
  QCOMPARE(model->rowCount(), 5);
  QCOMPARE(wc.rowCount(), 5);
  QCOMPARE(wc.currentRow(), 4);
  data = model->data(model->index(4, model->fieldIndex("FirstName")));
  QCOMPARE(data.toString(), QString("New name 1"));
  data = model->data(model->index(4, model->fieldIndex("Remarks")));
  QCOMPARE(data.toString(), QString("New remark 1"));
  // Check that widget displays the correct row
  QCOMPARE(w.fld_FirstName->text(), QString("New name 1"));
  QCOMPARE(w.fld_Remarks->text(), QString("New remark 1"));
  /*
   * Remove first row
   */
  wc.toFirst();
  QTest::qWait(50);
  QCOMPARE(wc.rowCount(), 5);
  QCOMPARE(wc.currentRow(), 0);
  wc.remove();
  QTest::qWait(1000); // Writing in DB can be very slow, f.ex. with Sqlite on HDD
  QCOMPARE(wc.rowCount(), 4);
  QCOMPARE(wc.currentRow(), 0);
  // Check that widget displays the correct row
  QCOMPARE(w.fld_FirstName->text(), QString("Bety"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark on Bety"));
  /*
   * Check index changing on unsaved data.
   * Because we have no message handler, it must fail.
   */
  // Edit in widgets
  w.fld_FirstName->clear();
  w.fld_Remarks->clear();
  QTest::keyClicks(w.fld_FirstName, "ABCD");
  QTest::keyClicks(w.fld_Remarks, "1234");
  // Try to go to last
  wc.toNext();
  QTest::qWait(50);
  // Check that nothing changed
  QCOMPARE(wc.currentRow(), 0);
  QCOMPARE(w.fld_FirstName->text(), QString("ABCD"));
  QCOMPARE(w.fld_Remarks->text(), QString("1234"));
  // Revert and check that model's data are displayed again
  wc.revert();
  QTest::qWait(50);
  QCOMPARE(wc.currentRow(), 0);
  QCOMPARE(w.fld_FirstName->text(), QString("Bety"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark on Bety"));
  // Go next (must work now)
  wc.toNext();
  QTest::qWait(50);
  QCOMPARE(wc.currentRow(), 1);
  QCOMPARE(w.fld_FirstName->text(), QString("Zeta"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark on Zeta"));
  // Edit in widgets
  w.fld_FirstName->clear();
  w.fld_Remarks->clear();
  QTest::keyClicks(w.fld_FirstName, "ABCD");
  QTest::keyClicks(w.fld_Remarks, "1234");
  // Try to go to last
  wc.toLast();
  QTest::qWait(50);
  // Check that nothing changed
  QCOMPARE(wc.currentRow(), 1);
  QCOMPARE(w.fld_FirstName->text(), QString("ABCD"));
  QCOMPARE(w.fld_Remarks->text(), QString("1234"));
  // Save and check
  QVERIFY(wc.submitAndWait());
  QCOMPARE(wc.currentRow(), 1);
  QCOMPARE(w.fld_FirstName->text(), QString("ABCD"));
  QCOMPARE(w.fld_Remarks->text(), QString("1234"));
  /*
   * Checks on empty table
   */
  clearTestDatabaseData();
  QVERIFY(wc.select());
  QCOMPARE(wc.rowCount(), 0);
  QCOMPARE(wc.currentRow(), -1);
  QVERIFY(!w.fld_FirstName->isEnabled());
  QVERIFY(!w.fld_Remarks->isEnabled());
  QVERIFY(w.fld_FirstName->text().isEmpty());
  QVERIFY(w.fld_Remarks->text().isEmpty());
  QVERIFY(!w.toFirstEnabled);
  QVERIFY(!w.toLastEnabled);
  QVERIFY(!w.toNextEnabled);
  QVERIFY(!w.toPreviousEnabled);
  QVERIFY(!w.submitEnabled);
  QVERIFY(!w.revertEnabled);
  QVERIFY(w.insertEnabled);
  QVERIFY(!w.removeEnabled);
  /*
   * Prepare table with 1000 elements
   * (will check fetch more and related behaviours)
   */
  // Check navigation
  populate1000Names();
  QVERIFY(wc.select());
  QCOMPARE(wc.rowCount(), 1000);
  QCOMPARE(wc.currentRow(), 0);
  QCOMPARE(w.fld_FirstName->text(), QString("Name 1"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark 1"));
  QVERIFY(wc.select());
  QVERIFY(wc.rowCount(false) < 1000);
  QCOMPARE(wc.currentRow(), 0);
  QCOMPARE(w.fld_FirstName->text(), QString("Name 1"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark 1"));
  wc.toNext();
  QTest::qWait(50);
  QCOMPARE(wc.currentRow(), 1);
  QCOMPARE(w.fld_FirstName->text(), QString("Name 2"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark 2"));
  wc.setCurrentRow(499);
  QCOMPARE(wc.currentRow(), 499);
  QCOMPARE(w.fld_FirstName->text(), QString("Name 500"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark 500"));
  QVERIFY(wc.rowCount(false) < 1000);
  wc.toLast();
  QTest::qWait(50);
  QCOMPARE(wc.currentRow(), 999);
  QCOMPARE(w.fld_FirstName->text(), QString("Name 1000"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark 1000"));
  // Check insertion
  QVERIFY(wc.select());
  QVERIFY(wc.rowCount(false) < 1000);
  QCOMPARE(wc.currentRow(), 0);
  // Insert a record
  wc.insert();
  QTest::qWait(50);
  QCOMPARE(wc.rowCount(false), 1001);
  QCOMPARE(wc.currentRow(), 1000);
  QVERIFY(w.fld_FirstName->isEnabled());
  QVERIFY(w.fld_Remarks->isEnabled());
  QVERIFY(w.fld_FirstName->text().isEmpty());
  QVERIFY(w.fld_Remarks->text().isEmpty());
  QTest::keyClicks(w.fld_FirstName, "New name 1001");
  QTest::keyClicks(w.fld_Remarks, "New remark 1001");
  // Submit with synch version
  QVERIFY(wc.submitAndWait());
  // Check that model was updated
  QCOMPARE(model->rowCount(), 1001);
  QCOMPARE(wc.rowCount(), 1001);
  QCOMPARE(wc.currentRow(), 1000);
  data = model->data(model->index(1000, model->fieldIndex("FirstName")));
  QCOMPARE(data.toString(), QString("New name 1001"));
  data = model->data(model->index(1000, model->fieldIndex("Remarks")));
  QCOMPARE(data.toString(), QString("New remark 1001"));
  // Check that widget displays the correct row
  QCOMPARE(w.fld_FirstName->text(), QString("New name 1001"));
  QCOMPARE(w.fld_Remarks->text(), QString("New remark 1001"));
  /*
   * Delete somewhere in the middle
   */
  QVERIFY(wc.select());
  QVERIFY(wc.rowCount(false) < 1000);
  QCOMPARE(wc.currentRow(), 0);
  QVERIFY(wc.setCurrentRow(399));
  QVERIFY(wc.rowCount(false) < 1000);
  QCOMPARE(wc.currentRow(), 399);
  // Check that widget displays the correct row
  QCOMPARE(w.fld_FirstName->text(), QString("Name 400"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark 400"));
  wc.remove();
  QTest::qWait(1000); // Writing in DB can be very slow, f.ex. with Sqlite on HDD
  // Check that model was updated
  QCOMPARE(wc.currentRow(), 399);
  // Check that widget displays the correct row
  QCOMPARE(w.fld_FirstName->text(), QString("Name 401"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark 401"));
  QCOMPARE(wc.rowCount(), 1000);
  /*
   * Check sorting
   */
  // Repopulate original test data
  clearTestDatabaseData();
  populateTestDatabase();
  // Setup sorting
  wc.addColumnToSortOrder("FirstName", Qt::AscendingOrder);
  wc.sort();
  // Select and check
  QVERIFY(wc.select());
  QCOMPARE(wc.rowCount(), 4);
  QCOMPARE(wc.currentRow(), 0);
  // Check that widget displays the correct data
  QCOMPARE(w.fld_FirstName->text(), QString("Andy"));
  QVERIFY(w.fld_Remarks->text().isEmpty());
  // Check that currentData() works
  QCOMPARE(wc.currentData("FirstName"), QVariant("Andy"));
  QCOMPARE(wc.currentData("Remarks"), QVariant(""));
  // Go to next row and check
  wc.toNext();
  QTest::qWait(50);
  QCOMPARE(w.fld_FirstName->text(), QString("Bety"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark on Bety"));
  QCOMPARE(wc.currentData("FirstName"), QVariant("Bety"));
  QCOMPARE(wc.currentData("Remarks"), QVariant("Remark on Bety"));
  // Go to next row and check
  wc.toNext();
  QTest::qWait(50);
  QCOMPARE(w.fld_FirstName->text(), QString("Charly"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark on Charly"));
  QCOMPARE(wc.currentData("FirstName"), QVariant("Charly"));
  QCOMPARE(wc.currentData("Remarks"), QVariant("Remark on Charly"));
  // Go to next row and check
  wc.toNext();
  QTest::qWait(50);
  QCOMPARE(w.fld_FirstName->text(), QString("Zeta"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark on Zeta"));
  QCOMPARE(wc.currentData("FirstName"), QVariant("Zeta"));
  QCOMPARE(wc.currentData("Remarks"), QVariant("Remark on Zeta"));
  // Go to first and check
  wc.toFirst();
  QTest::qWait(50);
  QCOMPARE(w.fld_FirstName->text(), QString("Andy"));
  QVERIFY(w.fld_Remarks->text().isEmpty());
  QCOMPARE(wc.currentData("FirstName"), QVariant("Andy"));
  QCOMPARE(wc.currentData("Remarks"), QVariant(""));
  // Insert a record
  wc.insert();
  QTest::qWait(50);
  QCOMPARE(wc.rowCount(), 5);
  QCOMPARE(wc.currentRow(), 4);
  QVERIFY(w.fld_FirstName->text().isEmpty());
  QVERIFY(w.fld_Remarks->text().isEmpty());
  QTest::keyClicks(w.fld_FirstName, "Laura");
  QTest::keyClicks(w.fld_Remarks, "Remark on Laura");
  QVERIFY(wc.submitAndWait());
  QCOMPARE(w.fld_FirstName->text(), QString("Laura"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark on Laura"));
  QCOMPARE(wc.currentData("FirstName"), QVariant("Laura"));
  QCOMPARE(wc.currentData("Remarks"), QVariant("Remark on Laura"));
  // Check all data again
  wc.sort();
  wc.toFirst();
  QTest::qWait(50);
  QCOMPARE(w.fld_FirstName->text(), QString("Andy"));
  QVERIFY(w.fld_Remarks->text().isEmpty());
  QCOMPARE(wc.currentData("FirstName"), QVariant("Andy"));
  QCOMPARE(wc.currentData("Remarks"), QVariant(""));
  // Go to next row and check
  wc.toNext();
  QTest::qWait(50);
  QCOMPARE(w.fld_FirstName->text(), QString("Bety"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark on Bety"));
  QCOMPARE(wc.currentData("FirstName"), QVariant("Bety"));
  QCOMPARE(wc.currentData("Remarks"), QVariant("Remark on Bety"));
  // Go to next row and check
  wc.toNext();
  QTest::qWait(50);
  QCOMPARE(w.fld_FirstName->text(), QString("Charly"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark on Charly"));
  QCOMPARE(wc.currentData("FirstName"), QVariant("Charly"));
  QCOMPARE(wc.currentData("Remarks"), QVariant("Remark on Charly"));
  // Go to next row and check
  wc.toNext();
  QTest::qWait(50);
  QCOMPARE(w.fld_FirstName->text(), QString("Laura"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark on Laura"));
  QCOMPARE(wc.currentData("FirstName"), QVariant("Laura"));
  QCOMPARE(wc.currentData("Remarks"), QVariant("Remark on Laura"));
  // Go to next row and check
  wc.toNext();
  QTest::qWait(50);
  QCOMPARE(w.fld_FirstName->text(), QString("Zeta"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark on Zeta"));
  QCOMPARE(wc.currentData("FirstName"), QVariant("Zeta"));
  QCOMPARE(wc.currentData("Remarks"), QVariant("Remark on Zeta"));
  /*
   * Check programmed edition - No submit
   */
  QVERIFY(wc.setCurrentData("Remarks", "Edited remark on Zeta", false));
  QCOMPARE(w.fld_FirstName->text(), QString("Zeta"));
  QCOMPARE(w.fld_Remarks->text(), QString("Edited remark on Zeta"));
  QCOMPARE(wc.currentData("FirstName"), QVariant("Zeta"));
  QCOMPARE(wc.currentData("Remarks"), QVariant("Edited remark on Zeta"));
  // Check that DB is intact
  QVERIFY(wc.select());
  wc.sort();
  wc.toLast();
  QTest::qWait(50);
  QCOMPARE(w.fld_FirstName->text(), QString("Zeta"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark on Zeta"));
  QCOMPARE(wc.currentData("FirstName"), QVariant("Zeta"));
  QCOMPARE(wc.currentData("Remarks"), QVariant("Remark on Zeta"));
  /*
   * Check programmed edition - With direct submit
   */
  QVERIFY(wc.setCurrentData("Remarks", "Edited remark on Zeta", true));
  QCOMPARE(w.fld_FirstName->text(), QString("Zeta"));
  QCOMPARE(w.fld_Remarks->text(), QString("Edited remark on Zeta"));
  QCOMPARE(wc.currentData("FirstName"), QVariant("Zeta"));
  QCOMPARE(wc.currentData("Remarks"), QVariant("Edited remark on Zeta"));
  QVERIFY(wc.select());
  wc.sort();
  wc.toLast();
  QTest::qWait(50);
  QCOMPARE(w.fld_FirstName->text(), QString("Zeta"));
  QCOMPARE(w.fld_Remarks->text(), QString("Edited remark on Zeta"));
  QCOMPARE(wc.currentData("FirstName"), QVariant("Zeta"));
  QCOMPARE(wc.currentData("Remarks"), QVariant("Edited remark on Zeta"));
  /*
   * Check programmed edition:
   *  - Edit many rows
   *  - Submit all at once
   */
  // Edit each row
  wc.toFirst();
  QTest::qWait(50);
  QVERIFY(wc.setCurrentData("Remarks", "Edited remark on Andy", false));
  wc.toNext();
  QTest::qWait(50);
  QVERIFY(wc.setCurrentData("Remarks", "Edited remark on Bety", false));
  wc.toNext();
  QTest::qWait(50);
  QVERIFY(wc.setCurrentData("Remarks", "Edited remark on Charly", false));
  wc.toNext();
  QTest::qWait(50);
  QVERIFY(wc.setCurrentData("Remarks", "Edited remark on Laura", false));
  wc.toNext();
  QTest::qWait(50);
  QVERIFY(wc.setCurrentData("Remarks", "Edited remark 2 on Zeta", false));
  // Submit all rows
  QVERIFY(wc.submitAndWait());
  // Check that database was updated
  QVERIFY(wc.select());
  wc.sort();
  wc.toFirst();
  QTest::qWait(50);
  QCOMPARE(w.fld_FirstName->text(), QString("Andy"));
  QCOMPARE(w.fld_Remarks->text(), QString("Edited remark on Andy"));
  QCOMPARE(wc.currentData("FirstName"), QVariant("Andy"));
  QCOMPARE(wc.currentData("Remarks"), QVariant("Edited remark on Andy"));
  // Go to next row and check
  wc.toNext();
  QTest::qWait(50);
  QCOMPARE(w.fld_FirstName->text(), QString("Bety"));
  QCOMPARE(w.fld_Remarks->text(), QString("Edited remark on Bety"));
  QCOMPARE(wc.currentData("FirstName"), QVariant("Bety"));
  QCOMPARE(wc.currentData("Remarks"), QVariant("Edited remark on Bety"));
  // Go to next row and check
  wc.toNext();
  QTest::qWait(50);
  QCOMPARE(w.fld_FirstName->text(), QString("Charly"));
  QCOMPARE(w.fld_Remarks->text(), QString("Edited remark on Charly"));
  QCOMPARE(wc.currentData("FirstName"), QVariant("Charly"));
  QCOMPARE(wc.currentData("Remarks"), QVariant("Edited remark on Charly"));
  // Go to next row and check
  wc.toNext();
  QTest::qWait(50);
  QCOMPARE(w.fld_FirstName->text(), QString("Laura"));
  QCOMPARE(w.fld_Remarks->text(), QString("Edited remark on Laura"));
  QCOMPARE(wc.currentData("FirstName"), QVariant("Laura"));
  QCOMPARE(wc.currentData("Remarks"), QVariant("Edited remark on Laura"));
  // Go to next row and check
  wc.toNext();
  QTest::qWait(50);
  QCOMPARE(w.fld_FirstName->text(), QString("Zeta"));
  QCOMPARE(w.fld_Remarks->text(), QString("Edited remark 2 on Zeta"));
  QCOMPARE(wc.currentData("FirstName"), QVariant("Zeta"));
  QCOMPARE(wc.currentData("Remarks"), QVariant("Edited remark 2 on Zeta"));

  // Clear test data
  clearTestDatabaseData();
  // Re-enable foreign_keys support
  QVERIFY(q.exec("PRAGMA foreign_keys = ON"));
}

void mdtDatabaseWidgetTest::sqlTableViewControllerTest()
{
  mdtSqlTableViewController tvc;
  QTableView tv;
  QSqlQuery q(pvDatabaseManager.database());

  // For this test, we wont foreign_keys support
  QVERIFY(q.exec("PRAGMA foreign_keys = OFF"));
  // Create test data
  populateTestDatabase();
  /*
   * Setup
   */
  tvc.setTableView(&tv);
  tvc.setTableName("Client_tbl", pvDatabaseManager.database(), "Clients");
  tv.show();
  tvc.start();
  /*
   * Select and check
   */
  QVERIFY(tvc.select());

  QTest::qWait(5000);

  // Clear test data
  clearTestDatabaseData();
  // Re-enable foreign_keys support
  QVERIFY(q.exec("PRAGMA foreign_keys = ON"));
}


void mdtDatabaseWidgetTest::sqlTableSelectionItemTest()
{
  QSqlTableModel model;
  QModelIndex index;

  // Populate database and setup model
  populateTestDatabase();
  model.setTable("Client_tbl");
  QVERIFY(model.select());
  // Get index of first row, field Id_PK
  index = model.index(0, model.fieldIndex("Id_PK"));
  QVERIFY(index.isValid());
  // Build table selection item and check
  mdtSqlTableSelectionItem item(index, "Id_PK");
  QVERIFY(item.index().isValid());
  QCOMPARE(item.index().row(), 0);
  QCOMPARE(item.fieldName(), QString("Id_PK"));
  // Clear database data
  clearTestDatabaseData();
}

void mdtDatabaseWidgetTest::sqlTableSelectionRowTest()
{
  QSqlTableModel model;
  QModelIndex index;
  mdtSqlTableSelectionRow row;

  // Populate database and setup model
  populateTestDatabase();
  model.setTable("Client_tbl");
  QVERIFY(model.select());

  // Initial state
  QCOMPARE(row.columnCount(), 0);
  QCOMPARE(row.index(""), QModelIndex());
  QCOMPARE(row.fields().size(), 0);
  // Put a index and check
  index = model.index(0, model.fieldIndex("Id_PK"));
  QVERIFY(index.isValid());
  row.addIndex(index, "Id_PK");
  QCOMPARE(row.columnCount(), 1);
  QCOMPARE(row.index(0), index);
  QCOMPARE(row.index("Id_PK"), index);
  QCOMPARE(row.fields().size(), 1);
  QCOMPARE(row.fields().at(0), QString("Id_PK"));
  // Put a index and check
  index = model.index(0, model.fieldIndex("Remarks"));
  QVERIFY(index.isValid());
  row.addIndex(index, "Remarks");
  QCOMPARE(row.columnCount(), 2);
  QCOMPARE(row.index(1), index);
  QCOMPARE(row.index("Remarks"), index);
  QCOMPARE(row.fields().size(), 2);
  QCOMPARE(row.fields().at(0), QString("Id_PK"));
  QCOMPARE(row.fields().at(1), QString("Remarks"));
  // Clear
  row.clear();
  QCOMPARE(row.columnCount(), 0);
  QCOMPARE(row.index(""), QModelIndex());
  QCOMPARE(row.index("Id_PK"), QModelIndex());
  QCOMPARE(row.index("Remarks"), QModelIndex());
  QCOMPARE(row.fields().size(), 0);

  // Clear database data
  clearTestDatabaseData();
}

void mdtDatabaseWidgetTest::sqlTableSelectionTest()
{
  QSqlTableModel model;
  QTableView view;
  QItemSelectionModel *selectionModel;
  QModelIndex index;
  mdtSqlTableSelection s;
  QStringList fieldList;
  QList<QVariant> expectedDataList;

  // Populate database and setup model + view
  populateTestDatabase();
  view.setModel(&model);
  model.setTable("Client_tbl");
  QVERIFY(model.select());
  view.resize(500, 200);
  view.show();
  selectionModel = view.selectionModel();
  QVERIFY(selectionModel != 0);
  // Field list we need
  fieldList << "Id_PK";

  // Initial state
  QCOMPARE(s.rowCount(), 0);
  QVERIFY(s.isEmpty());
  /*
   * Select Id_PK from row 0 in view
   */
  selectionModel->clear();
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  selectionModel->select(index, QItemSelectionModel::Select);
  // Set selection to table selection and check
  s.setIndexList(selectionModel->selectedIndexes(), fieldList, &model);
  QCOMPARE(s.rowCount(), 1);
  QVERIFY(!s.isEmpty());
  QCOMPARE(s.fields(0).size(), 1);
  QCOMPARE(s.fields(0).at(0), QString("Id_PK"));
  QCOMPARE(s.data(0, "Id_PK"), QVariant(1));
  expectedDataList.clear();
  expectedDataList << 1;
  QCOMPARE(s.dataList("Id_PK"), expectedDataList);
  /*
   * Select Id_PK from row 1 in view
   */
  selectionModel->clear();
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  selectionModel->select(index, QItemSelectionModel::Select);
  // Set selection to table selection and check
  s.setIndexList(selectionModel->selectedIndexes(), fieldList, &model);
  QCOMPARE(s.rowCount(), 1);
  QVERIFY(!s.isEmpty());
  QCOMPARE(s.fields(0).size(), 1);
  QCOMPARE(s.fields(0).at(0), QString("Id_PK"));
  QCOMPARE(s.data(0, "Id_PK"), QVariant(2));
  expectedDataList.clear();
  expectedDataList << 2;
  QCOMPARE(s.dataList("Id_PK"), expectedDataList);
  /*
   * Select Id_PK from row 0 and 1 in view
   */
  selectionModel->clear();
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  selectionModel->select(index, QItemSelectionModel::Select);
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  selectionModel->select(index, QItemSelectionModel::Select);
  // Set selection to table selection and check
  s.setIndexList(selectionModel->selectedIndexes(), fieldList, &model);
  QCOMPARE(s.rowCount(), 2);
  QVERIFY(!s.isEmpty());
  QCOMPARE(s.fields(0).size(), 1);
  QCOMPARE(s.fields(0).at(0), QString("Id_PK"));
  QCOMPARE(s.fields(1).size(), 1);
  QCOMPARE(s.fields(1).at(0), QString("Id_PK"));
  QCOMPARE(s.data(0, "Id_PK"), QVariant(1));
  QCOMPARE(s.data(1, "Id_PK"), QVariant(2));
  expectedDataList.clear();
  expectedDataList << 1 << 2;
  QCOMPARE(s.dataList("Id_PK"), expectedDataList);
  /*
   * Select FirstName from row 0
   */
  selectionModel->clear();
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  selectionModel->select(index, QItemSelectionModel::Select);
  // Set selection to table selection and check
  s.setIndexList(selectionModel->selectedIndexes(), fieldList, &model);
  QCOMPARE(s.rowCount(), 1);
  QVERIFY(!s.isEmpty());
  QCOMPARE(s.fields(0).size(), 1);
  QCOMPARE(s.fields(0).at(0), QString("Id_PK"));
  QCOMPARE(s.data(0, "Id_PK"), QVariant(1));
  expectedDataList.clear();
  expectedDataList << 1;
  QCOMPARE(s.dataList("Id_PK"), expectedDataList);
  /*
   * Select FirstName from row 1
   */
  selectionModel->clear();
  index = model.index(1, 1);
  QVERIFY(index.isValid());
  selectionModel->select(index, QItemSelectionModel::Select);
  // Set selection to table selection and check
  s.setIndexList(selectionModel->selectedIndexes(), fieldList, &model);
  QCOMPARE(s.rowCount(), 1);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(2));
  expectedDataList.clear();
  expectedDataList << 2;
  QCOMPARE(s.dataList("Id_PK"), expectedDataList);
  /*
   * Select FirstName from row 0 and Remarks from row 1
   */
  selectionModel->clear();
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  selectionModel->select(index, QItemSelectionModel::Select);
  index = model.index(1, 2);
  QVERIFY(index.isValid());
  selectionModel->select(index, QItemSelectionModel::Select);
  // Set selection to table selection and check
  s.setIndexList(selectionModel->selectedIndexes(), fieldList, &model);
  QCOMPARE(s.rowCount(), 2);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(1));
  QCOMPARE(s.data(1, "Id_PK"), QVariant(2));
  expectedDataList.clear();
  expectedDataList << 1 << 2;
  QCOMPARE(s.dataList("Id_PK"), expectedDataList);
  /*
   * Select complete rows 0 and 1
   */
  selectionModel->clear();
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  selectionModel->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  selectionModel->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
  // Set selection to table selection and check
  s.setIndexList(selectionModel->selectedIndexes(), fieldList, &model);
  QCOMPARE(s.rowCount(), 2);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(1));
  QCOMPARE(s.data(1, "Id_PK"), QVariant(2));
  expectedDataList.clear();
  expectedDataList << 1 << 2;
  QCOMPARE(s.dataList("Id_PK"), expectedDataList);

  // Clear
  s.clear();
  QCOMPARE(s.rowCount(), 0);
  QVERIFY(s.isEmpty());

  /*
   * Play
   */
  /*
  while(view.isVisible()){
    QTest::qWait(1000);
  }
  */

  // Clear database data
  clearTestDatabaseData();
}

void mdtDatabaseWidgetTest::sqlSelectionDialogTest()
{
  mdtSqlSelectionDialog *dialog;
  QSqlQuery q;
  QString sql;
  mdtSqlTableSelection s;
  QStringList fields;

  // Populate database
  populateTestDatabase();
  /*
   * Check single selection with 1 field, at column 0
   */
  sql = "SELECT * FROM Client_tbl";
  dialog = new mdtSqlSelectionDialog;
  dialog->setQuery(sql, pvDatabaseManager.database());
  dialog->selectRow(0);
  QTimer::singleShot(50, dialog, SLOT(accept()));
  QVERIFY(dialog->exec() == QDialog::Accepted);
  s = dialog->selection("Id_PK");
  QCOMPARE(s.rowCount(), 1);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(1));
  delete dialog;
  /*
   * Check single selection with 1 field, at column 0 + sorting ASC
   */
  sql = "SELECT * FROM Client_tbl";
  dialog = new mdtSqlSelectionDialog;
  dialog->setQuery(sql, pvDatabaseManager.database());
  dialog->addColumnToSortOrder("FirstName", Qt::AscendingOrder);
  dialog->sort();
  dialog->selectRow(0);
  QTimer::singleShot(50, dialog, SLOT(accept()));
  QVERIFY(dialog->exec() == QDialog::Accepted);
  s = dialog->selection("Id_PK");
  QCOMPARE(s.rowCount(), 1);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(1));
  delete dialog;
  /*
   * Check single selection with 1 field, at column 0 + sorting DESC
   */
  sql = "SELECT * FROM Client_tbl";
  dialog = new mdtSqlSelectionDialog;
  dialog->setQuery(sql, pvDatabaseManager.database());
  dialog->addColumnToSortOrder("FirstName", Qt::DescendingOrder);
  dialog->sort();
  dialog->selectRow(0);
  QTimer::singleShot(50, dialog, SLOT(accept()));
  QVERIFY(dialog->exec() == QDialog::Accepted);
  s = dialog->selection("Id_PK");
  QCOMPARE(s.rowCount(), 1);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(3));
  delete dialog;
  /*
   * Check single selection with 1 field, at column 1
   */
  sql = "SELECT * FROM Client_tbl";
  dialog = new mdtSqlSelectionDialog;
  dialog->setQuery(sql, pvDatabaseManager.database());
  dialog->selectRow(0);
  QTimer::singleShot(50, dialog, SLOT(accept()));
  QVERIFY(dialog->exec() == QDialog::Accepted);
  s = dialog->selection("FirstName");
  QCOMPARE(s.rowCount(), 1);
  QCOMPARE(s.data(0, "FirstName"), QVariant("Andy"));
  delete dialog;
  /*
   * Check single selection with 2 fields
   */
  sql = "SELECT * FROM Client_tbl";
  dialog = new mdtSqlSelectionDialog;
  dialog->setQuery(sql, pvDatabaseManager.database());
  dialog->selectRow(0);
  QTimer::singleShot(50, dialog, SLOT(accept()));
  QVERIFY(dialog->exec() == QDialog::Accepted);
  fields.clear();
  fields << "Id_PK" << "FirstName";
  s = dialog->selection(fields);
  QCOMPARE(s.rowCount(), 1);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(1));
  QCOMPARE(s.data(0, "FirstName"), QVariant("Andy"));
  delete dialog;
  /*
   * Check multiple selection with 1 field
   */
  sql = "SELECT * FROM Client_tbl";
  dialog = new mdtSqlSelectionDialog;
  dialog->setQuery(sql, pvDatabaseManager.database(), true);
  dialog->selectRow(0);
  dialog->selectRow(1);
  QTimer::singleShot(50, dialog, SLOT(accept()));
  QVERIFY(dialog->exec() == QDialog::Accepted);
  s = dialog->selection("Id_PK");
  QCOMPARE(s.rowCount(), 2);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(1));
  QCOMPARE(s.data(1, "Id_PK"), QVariant(2));
  delete dialog;
  /*
   * Check multiple selection with 1 field, sorting DESC
   */
  sql = "SELECT * FROM Client_tbl";
  dialog = new mdtSqlSelectionDialog;
  dialog->setQuery(sql, pvDatabaseManager.database(), true);
  dialog->addColumnToSortOrder("FirstName", Qt::DescendingOrder);
  dialog->sort();
  dialog->selectRow(0);
  dialog->selectRow(1);
  QTimer::singleShot(50, dialog, SLOT(accept()));
  QVERIFY(dialog->exec() == QDialog::Accepted);
  s = dialog->selection("Id_PK");
  QCOMPARE(s.rowCount(), 2);
  /**
   *  \todo Clarifier et fixer
  QCOMPARE(s.data(0, "Id_PK"), QVariant(1));
  QCOMPARE(s.data(1, "Id_PK"), QVariant(2));
  */
  delete dialog;
  /*
   * Check multiple selection with 2 fields, sorting DESC
   */
  sql = "SELECT * FROM Client_tbl";
  dialog = new mdtSqlSelectionDialog;
  dialog->setQuery(sql, pvDatabaseManager.database(), true);
  dialog->addColumnToSortOrder("FirstName", Qt::DescendingOrder);
  dialog->selectRow(0);
  dialog->selectRow(1);
  QTimer::singleShot(50, dialog, SLOT(accept()));
  QVERIFY(dialog->exec() == QDialog::Accepted);
  fields.clear();
  fields << "Id_PK" << "FirstName";
  s = dialog->selection(fields);
  QCOMPARE(s.rowCount(), 2);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(1));
  QCOMPARE(s.data(0, "FirstName"), QVariant("Andy"));
  QCOMPARE(s.data(1, "Id_PK"), QVariant(2));
  QCOMPARE(s.data(1, "FirstName"), QVariant("Bety"));
  delete dialog;

  // Cleanup
  clearTestDatabaseData();
}

void mdtDatabaseWidgetTest::sqlTableWidgetTest()
{
  mdtSqlTableWidget *sqlTableWidget;
  QSqlTableModel model;
  QSqlTableModel *addressModel;
  mdtSqlTableWidget *addressWidget;
  mdtSqlRelation *relation;
  QTableView *view;
  mdtSqlTableSelection s;
  QStringList fields;
  QModelIndex index;

  // Populate database
  populateTestDatabase();
  // Setup model + widget
  model.setTable("Client_tbl");
  model.select();
  sqlTableWidget = new mdtSqlTableWidget;
  sqlTableWidget->setModel(&model);
  sqlTableWidget->resize(500, 300);
  sqlTableWidget->show();
  view = sqlTableWidget->tableView();
  QVERIFY(view != 0);
  view->setSelectionMode(QAbstractItemView::MultiSelection);
  // Setup needed fields for selection
  fields.clear();
  fields << "Id_PK";

  /*
   * Check selection: select row 1
   */
  view->clearSelection();
  view->selectRow(1);
  s = sqlTableWidget->currentSelection(fields);
  QCOMPARE(s.rowCount(), 1);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(2));
  // Check current data
  QCOMPARE(sqlTableWidget->currentData("Id_PK"), QVariant(2));
  /*
   * Check selection: select row 0
   */
  view->clearSelection();
  view->selectRow(0);
  s = sqlTableWidget->currentSelection("Id_PK");
  QCOMPARE(s.rowCount(), 1);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(1));
  // Check current data
  QCOMPARE(sqlTableWidget->currentData("Id_PK"), QVariant(1));

  /*
   * Sort and check selection
   */
  sqlTableWidget->clearColumnsSortOrder();
  sqlTableWidget->addColumnToSortOrder("FirstName", Qt::DescendingOrder);
  sqlTableWidget->sort();
  // Select row 0 in view - we must have Id_PK 3
  view->clearSelection();
  view->selectRow(0);
  s = sqlTableWidget->currentSelection(fields);
  QCOMPARE(s.rowCount(), 1);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(3));
  // Check current data
  QCOMPARE(sqlTableWidget->currentData("Id_PK"), QVariant(3));
  // Select row 1 in view - we must have Id_PK 4 (Charly)
  view->clearSelection();
  view->selectRow(1);
  s = sqlTableWidget->currentSelection(fields);
  QCOMPARE(s.rowCount(), 1);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(4));
  // Check current data
  QCOMPARE(sqlTableWidget->currentData("Id_PK"), QVariant(4));

  /*
   * Add a table widget to show addresses
   */
  // Setup address model
  addressModel = new QSqlTableModel(0, pvDatabaseManager.database());
  addressModel->setTable("Address_tbl");
  QVERIFY(addressModel->select());
  // Setup address relation
  relation = new mdtSqlRelation;
  relation->setParentModel(sqlTableWidget->model());
  relation->setChildModel(addressModel);
  relation->addRelation("Id_PK", "Client_Id_FK", false);
  // Setup address widget
  addressWidget = new mdtSqlTableWidget;
  addressWidget->setModel(addressModel);
  addressWidget->resize(500, 250);
  sqlTableWidget->addChildWidget(addressWidget, relation);
  addressWidget->show();
  /*
   * Sort client ascending and check displayed addresses
   */
  // Sort client
  sqlTableWidget->clearColumnsSortOrder();
  sqlTableWidget->addColumnToSortOrder("FirstName", Qt::AscendingOrder);
  sqlTableWidget->sort();
  // Select row 0 and check
  view->clearSelection();
  view->selectRow(0);
  QCOMPARE(addressWidget->currentData("Client_Id_FK"), QVariant(1));
  // Select row 1 and check
  view->clearSelection();
  view->selectRow(1);
  QCOMPARE(addressWidget->currentData("Client_Id_FK"), QVariant(2));
  /*
   * Sort client descending and check displayed addresses
   */
  // Sort client
  sqlTableWidget->clearColumnsSortOrder();
  sqlTableWidget->addColumnToSortOrder("FirstName", Qt::DescendingOrder);
  sqlTableWidget->sort();
  // Select row 0 and check (Zeta, Id_PK 3)
  view->clearSelection();
  view->selectRow(0);
  QVERIFY(addressWidget->currentData("Client_Id_FK").isNull());
  // Select row 1 and check (Charly, Id_PK 4)
  view->selectRow(1);
  QVERIFY(addressWidget->currentData("Client_Id_FK").isNull());
  // Select row 0 and check (Zeta, Id_PK 3)
  view->clearSelection();
  view->selectRow(0);
  QVERIFY(addressWidget->currentData("Client_Id_FK").isNull());
  // Select row 2 and check (Bety, Id_PK 2)
  view->clearSelection();
  view->selectRow(2);
  QCOMPARE(addressWidget->currentData("Client_Id_FK"), QVariant(2));
  // Select row 3 and check (Andy, Id_PK 1)
  view->clearSelection();
  view->selectRow(3);
  QCOMPARE(addressWidget->currentData("Client_Id_FK"), QVariant(1));
  /*
   * Check multiple row selection with ASC sorting
   *  -> We must have only selected items,
   *     but sorting must be keeped
   */
  sqlTableWidget->clearColumnsSortOrder();
  sqlTableWidget->addColumnToSortOrder("FirstName", Qt::AscendingOrder);
  sqlTableWidget->sort();
  // Select row 0 (Andy, Id_PK 1) and row 1 (Bety, Id_PK 2) in view
  view->clearSelection();
  view->selectRow(0);
  view->selectRow(1);
  s = sqlTableWidget->currentSelection(fields);
  QCOMPARE(s.rowCount(), 2);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(1));
  QCOMPARE(s.data(1, "Id_PK"), QVariant(2));
  // Select row 2 (Charly, Id_PK 4) and row 3 (Zeta, Id_PK 3) in view
  view->clearSelection();
  view->selectRow(2);
  view->selectRow(3);
  s = sqlTableWidget->currentSelection(fields);
  QCOMPARE(s.rowCount(), 2);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(4));
  QCOMPARE(s.data(1, "Id_PK"), QVariant(3));
  // Select row 1 (Bety, Id_PK 2) and row 2 (Charly, Id_PK 4) in view
  view->clearSelection();
  view->selectRow(1);
  view->selectRow(2);
  s = sqlTableWidget->currentSelection(fields);
  QCOMPARE(s.rowCount(), 2);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(2));
  QCOMPARE(s.data(1, "Id_PK"), QVariant(4));
  // Select row 0 (Andy, Id_PK 1), row 1 (Bety, Id_PK 2) and row 2 (Charly, Id_PK 4) in view
  view->clearSelection();
  view->selectRow(0);
  view->selectRow(1);
  view->selectRow(2);
  s = sqlTableWidget->currentSelection(fields);
  QCOMPARE(s.rowCount(), 3);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(1));
  QCOMPARE(s.data(1, "Id_PK"), QVariant(2));
  QCOMPARE(s.data(2, "Id_PK"), QVariant(4));
  // Same as before, bust select rows in reverse order
  view->clearSelection();
  view->selectRow(2);
  view->selectRow(1);
  view->selectRow(0);
  s = sqlTableWidget->currentSelection(fields);
  QCOMPARE(s.rowCount(), 3);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(1));
  QCOMPARE(s.data(1, "Id_PK"), QVariant(2));
  QCOMPARE(s.data(2, "Id_PK"), QVariant(4));

  /*
   * Play
   */

  /*
  while(sqlTableWidget->isVisible()){
    QTest::qWait(1000);
  }
  */
  // Cleanup
  clearTestDatabaseData();
  delete sqlTableWidget;
}


/*
 * Helper methods
 */

void mdtDatabaseWidgetTest::createDatabaseSchema()
{
  QTemporaryFile dbFile;
  QFileInfo dbFileInfo;
  mdtSqlSchemaTable st;
  QSqlField field;

  /*
   * Check Sqlite database creation
   */
  QVERIFY(dbFile.open());
  dbFile.close();
  dbFileInfo.setFile(dbFile.fileName() + ".db");
  QVERIFY(pvDatabaseManager.createDatabaseSqlite(dbFileInfo, mdtSqlDatabaseManager::OverwriteExisting));
  /*
   * Create schema
   */
  // Client_tbl
  st.clear();
  st.setTableName("Client_tbl", "UTF8");
  // Id_PK
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  st.addField(field, true);
  // FirstName
  field = QSqlField();
  field.setName("FirstName");
  field.setRequired(true);
  field.setType(QVariant::String);
  field.setLength(50);
  st.addField(field, false);
  // Remarks
  field = QSqlField();
  field.setName("Remarks");
  field.setType(QVariant::String);
  field.setLength(100);
  st.addField(field, false);
  QVERIFY(pvDatabaseManager.createTable(st, mdtSqlDatabaseManager::OverwriteExisting));
  // Address_tbl
  st.clear();
  st.setTableName("Address_tbl", "UTF8");
  // Id_PK
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  st.addField(field, true);
  // StreetName
  field = QSqlField();
  field.setName("StreetName");
  field.setType(QVariant::String);
  field.setLength(50);
  st.addField(field, false);
  // StreetNumber
  field = QSqlField();
  field.setName("StreetNumber");
  field.setType(QVariant::Int);
  st.addField(field, false);
  // Client_Id_FK
  field = QSqlField();
  field.setName("Client_Id_FK");
  field.setType(QVariant::Int);
  st.addField(field, false);
  st.addForeignKey("Client_Id_FK_fk", "Client_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  QVERIFY(st.addFieldToForeignKey("Client_Id_FK_fk", "Client_Id_FK", "Id_PK"));
  QVERIFY(pvDatabaseManager.createTable(st, mdtSqlDatabaseManager::OverwriteExisting));

  // Enable foreing keys support
  QVERIFY(pvDatabaseManager.setForeignKeysEnabled(true));
}

void mdtDatabaseWidgetTest::populateTestDatabase()
{
  QSqlQuery query(pvDatabaseManager.database());

  // Inert some data in Client_tbl
  QVERIFY(query.exec("INSERT INTO 'Client_tbl' (Id_PK, FirstName) VALUES(1, 'Andy')"));
  QVERIFY(query.exec("INSERT INTO 'Client_tbl' (Id_PK, 'FirstName', 'Remarks') VALUES(2, 'Bety', 'Remark on Bety')"));
  QVERIFY(query.exec("INSERT INTO 'Client_tbl' (Id_PK, 'FirstName', 'Remarks') VALUES(3, 'Zeta', 'Remark on Zeta')"));
  QVERIFY(query.exec("INSERT INTO 'Client_tbl' (Id_PK, 'FirstName', 'Remarks') VALUES(4, 'Charly', 'Remark on Charly')"));
  QVERIFY(query.exec("SELECT * FROM 'Client_tbl'"));
  while(query.next()){
    QVERIFY(!query.record().isEmpty());
  }
  // Inert some data into Address_tbl
  QVERIFY(query.exec("INSERT INTO 'Address_tbl' ('StreetName', 'StreetNumber', 'Client_Id_FK') VALUES('Andy street 1', 11 , 1)"));
  QVERIFY(query.exec("INSERT INTO 'Address_tbl' ('StreetName', 'StreetNumber', 'Client_Id_FK') VALUES('Andy street 2', 12 , 1)"));
  QVERIFY(query.exec("INSERT INTO 'Address_tbl' ('StreetName', 'StreetNumber', 'Client_Id_FK') VALUES('Bety street 1', 21 , 2)"));
  QVERIFY(query.exec("SELECT * FROM 'Address_tbl'"));
  while(query.next()){
    QVERIFY(!query.record().isEmpty());
  }
}

void mdtDatabaseWidgetTest::populate1000Names()
{
  QSqlQuery query(pvDatabaseManager.database());
  QString sql;
  int i;

  QVERIFY(pvDatabaseManager.database().transaction());
  for(i = 0; i < 1000; ++i){
    sql = QString("INSERT INTO 'Client_tbl' (Id_PK, 'FirstName', 'Remarks') VALUES(%1, 'Name %2', 'Remark %3')").arg(i+1).arg(i+1).arg(i+1);
    QVERIFY(query.exec(sql));
  }
  QVERIFY(pvDatabaseManager.database().commit());
}

void mdtDatabaseWidgetTest::clearTestDatabaseData()
{
  QString sql;
  QSqlQuery query(pvDatabaseManager.database());

  sql = "DELETE FROM Address_tbl";
  QVERIFY(query.exec(sql));
  sql = "DELETE FROM Client_tbl";
  QVERIFY(query.exec(sql));
}

/*
 * Main
 */
int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtDatabaseWidgetTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
