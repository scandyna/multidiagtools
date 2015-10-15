/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
///#include "mdtSqlDatabaseSqlite.h"
#include "mdtSqlForeignKey.h"
#include "mdtSqlSchemaTable.h"
#include "mdtSqlDatabaseSchema.h"
#include "mdtSqlForeignKeySetting.h"
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
#include "mdtSqlForm.h"
#include "mdtSqlFieldSelectionDialog.h"
#include "mdtDoubleEdit.h"
#include "mdtSqlDialog.h"
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
#include <QPoint>

#include <QDebug>

/*
 * sqlDataWidgetControllerTestWidget implementation
 */

sqlDataWidgetControllerTestWidget::sqlDataWidgetControllerTestWidget(QWidget *parent)
  :QWidget(parent)
{
  QVBoxLayout *l = new QVBoxLayout;
  fld_FirstName = new QLineEdit;
  fld_FirstName->setObjectName("fld_FirstName");
  fld_Remarks = new QLineEdit;
  fld_Remarks->setObjectName("fld_Remarks");
  fld_Detail = new QLineEdit;
  fld_Detail->setObjectName("fld_Detail");
  fld_SomeValueDouble = new mdtDoubleEdit;
  fld_SomeValueDouble->setObjectName("fld_SomeValueDouble");
  l->addWidget(fld_FirstName);
  l->addWidget(fld_Remarks);
  l->addWidget(fld_Detail);
  l->addWidget(fld_SomeValueDouble);
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
 * mdtTableViewControllerTestObject implementation
 */

mdtTableViewControllerTestObject::mdtTableViewControllerTestObject()
 : QObject(0)
{
}

void mdtTableViewControllerTestObject::doubleClick(QTableView* tv, int row, int column)
{
  Q_ASSERT(tv != nullptr);
  int x = tv->columnViewportPosition(column) + 5;
  int y = tv->rowViewportPosition(row) + 10;
  QTest::mouseClick( tv->viewport(), Qt::LeftButton, 0, QPoint(x, y));
  QTest::mouseDClick( tv->viewport(), Qt::LeftButton, 0, QPoint(x, y));
}

void mdtTableViewControllerTestObject::doubleClickedReceiver(const QSqlRecord& record)
{
  doubleClickReceiverRecord = record;
}



/*
 * Test scenario data class
 */

class mdtDatabaseWidgetTestScenario1Data
{
 public:
  // Constructor
  mdtDatabaseWidgetTestScenario1Data(QSqlDatabase db)
   : pvDatabase(db)
   {
   }
   // Destructor - Will also clear data
   ~mdtDatabaseWidgetTestScenario1Data()
   {
     clear();
   }
   // Populate with scenario data
   bool populate();
   bool populate1000Names();
   // Clear scenario data
   void clear();
 private:
  // Log errors to terminal
  void debugSqlError(const QSqlError & error);
  Q_DISABLE_COPY(mdtDatabaseWidgetTestScenario1Data);
  QSqlDatabase pvDatabase;
};

bool mdtDatabaseWidgetTestScenario1Data::populate()
{
  QSqlQuery query(pvDatabase);

  if(!pvDatabase.transaction()){
    debugSqlError(pvDatabase.lastError());
    return false;
  }
  // Inert some data in Client_tbl
  if(!query.exec("INSERT INTO 'Client_tbl' (Id_PK, FirstName) VALUES(1, 'Andy')")){
    debugSqlError(query.lastError());
    pvDatabase.rollback();
    return false;
  }
  if(!query.exec("INSERT INTO 'Client_tbl' (Id_PK, 'FirstName', 'Remarks', 'SomeValueDouble') VALUES(2, 'Bety', 'Remark on Bety', 2)")){
    debugSqlError(query.lastError());
    pvDatabase.rollback();
    return false;
  }
  if(!query.exec("INSERT INTO 'Client_tbl' (Id_PK, 'FirstName', 'Remarks') VALUES(3, 'Zeta', 'Remark on Zeta')")){
    debugSqlError(query.lastError());
    pvDatabase.rollback();
    return false;
  }
  if(!query.exec("INSERT INTO 'Client_tbl' (Id_PK, 'FirstName', 'Remarks') VALUES(4, 'Charly', 'Remark on Charly')")){
    debugSqlError(query.lastError());
    pvDatabase.rollback();
    return false;
  }
  if(!query.exec("SELECT * FROM 'Client_tbl'")){
    debugSqlError(query.lastError());
    pvDatabase.rollback();
    return false;
  }
  while(query.next()){
    if(query.record().isEmpty()){
      qDebug() << "mdtDatabaseWidgetTestScenario1Data: inserting data into Client_tbl failed";
      pvDatabase.rollback();
      return false;
    }
  }
  // Inert some data into Address_tbl
  if(!query.exec("INSERT INTO 'Address_tbl' ('StreetName', 'StreetNumber', 'Client_Id_FK') VALUES('Andy street 1', 11 , 1)")){
    debugSqlError(query.lastError());
    pvDatabase.rollback();
    return false;
  }
  if(!query.exec("INSERT INTO 'Address_tbl' ('StreetName', 'StreetNumber', 'Client_Id_FK') VALUES('Andy street 2', 12 , 1)")){
    debugSqlError(query.lastError());
    pvDatabase.rollback();
    return false;
  }
  if(!query.exec("INSERT INTO 'Address_tbl' ('StreetName', 'StreetNumber', 'Client_Id_FK') VALUES('Bety street 1', 21 , 2)")){
    debugSqlError(query.lastError());
    pvDatabase.rollback();
    return false;
  }
  if(!query.exec("SELECT * FROM 'Address_tbl'")){
    debugSqlError(query.lastError());
    pvDatabase.rollback();
    return false;
  }
  while(query.next()){
    if(query.record().isEmpty()){
      qDebug() << "mdtDatabaseWidgetTestScenario1Data: inserting data into Address_tbl failed";
      pvDatabase.rollback();
      return false;
    }
  }
  if(!pvDatabase.commit()){
    debugSqlError(query.lastError());
    pvDatabase.rollback();
    return false;
  }

  return true;
}

bool mdtDatabaseWidgetTestScenario1Data::populate1000Names()
{
  QSqlQuery query(pvDatabase);
  QString sql;
  int i;

  if(!pvDatabase.transaction()){
    debugSqlError(pvDatabase.lastError());
    return false;
  }
  for(i = 0; i < 1000; ++i){
    sql = QString("INSERT INTO 'Client_tbl' (Id_PK, 'FirstName', 'Remarks') VALUES(%1, 'Name %2', 'Remark %3')").arg(i+1).arg(i+1).arg(i+1);
    if(!query.exec(sql)){
      debugSqlError(query.lastError());
      pvDatabase.rollback();
      return false;
    }
  }
  if(!pvDatabase.commit()){
    debugSqlError(query.lastError());
    pvDatabase.rollback();
    return false;
  }

  return true;
}

void mdtDatabaseWidgetTestScenario1Data::clear()
{
  QString sql;
  QSqlQuery query(pvDatabase);

  sql = "DELETE FROM Address_tbl";
  QVERIFY(query.exec(sql));
  sql = "DELETE FROM ClientDetail_tbl";
  QVERIFY(query.exec(sql));
  sql = "DELETE FROM Client_tbl";
  QVERIFY(query.exec(sql));
}

void mdtDatabaseWidgetTestScenario1Data::debugSqlError(const QSqlError& error)
{
  qDebug() << error;
}

/*
 * Test implementation
 */

void mdtDatabaseWidgetTest::initTestCase()
{
  createDatabaseSchema();
  QVERIFY(pvDatabase.isOpen());
}

void mdtDatabaseWidgetTest::cleanupTestCase()
{
  QFile::remove(pvDatabase.databaseName());
}

void mdtDatabaseWidgetTest::sqlRelationTest()
{
  mdtDatabaseWidgetTestScenario1Data scenario1(pvDatabase);
  QSqlTableModel clientModel;
  QSqlTableModel detailModel;
  QSqlTableModel addressModel;
  mdtSqlRelation clientDetailRelation;
  mdtSqlRelation clientAdressRelation;
  mdtSqlRelationInfo relationInfo;
  QModelIndex index;

  // Create test data
//   populateTestDatabase();
  QVERIFY(scenario1.populate());
  // Setup models
  clientModel.setEditStrategy(QSqlTableModel::OnManualSubmit);
  clientModel.setTable("Client_tbl");
  QVERIFY(clientModel.select());
  detailModel.setEditStrategy(QSqlTableModel::OnManualSubmit);
  detailModel.setTable("ClientDetail_tbl");
  QVERIFY(detailModel.select());
  addressModel.setTable("Address_tbl");
  QVERIFY(addressModel.select());
  /*
   * Check 1-many relation
   */
  // Setup relation
  clientAdressRelation.setParentModel(&clientModel);
  clientAdressRelation.setChildModel(&addressModel);
  relationInfo.addRelation("Id_PK", "Client_Id_FK", true);
  QVERIFY(clientAdressRelation.addRelations(relationInfo));
  // Go to first client
  clientAdressRelation.setParentCurrentIndex(0);
  // Check client data
  QCOMPARE(clientModel.rowCount(), 4);
  index = clientModel.index(0, 0);  // Id_PK
  QCOMPARE(clientModel.data(index), QVariant(1));
  index = clientModel.index(0, 1);  // FirstName
  QCOMPARE(clientModel.data(index), QVariant("Andy"));
  // Check address count
  QCOMPARE(addressModel.rowCount(), 2);
  // Check address data - row 0
  index = addressModel.index(0, 1); // StreetName
  QCOMPARE(addressModel.data(index), QVariant("Andy street 1"));
  index = addressModel.index(0, 3); // Client_Id_FK
  QCOMPARE(addressModel.data(index), QVariant(1));
  // Check address data - row 1
  index = addressModel.index(1, 1); // StreetName
  QCOMPARE(addressModel.data(index), QVariant("Andy street 2"));
  index = addressModel.index(1, 3); // Client_Id_FK
  QCOMPARE(addressModel.data(index), QVariant(1));
  // Go to second client
  clientAdressRelation.setParentCurrentIndex(1);
  // Check client data
  QCOMPARE(clientModel.rowCount(), 4);
  index = clientModel.index(1, 0);  // Id_PK
  QCOMPARE(clientModel.data(index), QVariant(2));
  index = clientModel.index(1, 1);  // FirstName
  QCOMPARE(clientModel.data(index), QVariant("Bety"));
  // Check address count
  QCOMPARE(addressModel.rowCount(), 1);
  // Check address data - row 0
  index = addressModel.index(0, 1); // StreetName
  QCOMPARE(addressModel.data(index), QVariant("Bety street 1"));
  index = addressModel.index(0, 3); // Client_Id_FK
  QCOMPARE(addressModel.data(index), QVariant(2));
  // Go to third client
  clientAdressRelation.setParentCurrentIndex(2);
  // Check client data
  QCOMPARE(clientModel.rowCount(), 4);
  index = clientModel.index(2, 0);  // Id_PK
  QCOMPARE(clientModel.data(index), QVariant(3));
  index = clientModel.index(2, 1);  // FirstName
  QCOMPARE(clientModel.data(index), QVariant("Zeta"));
  // Check address count
  QCOMPARE(addressModel.rowCount(), 0);
  // Insert a new address
  QVERIFY(addressModel.insertRows(0, 1));
  index = addressModel.index(0, 1); // StreetName
  QVERIFY(addressModel.setData(index, "Zeta street 1"));
  QVERIFY(addressModel.submitAll());
  // Check address count
  QCOMPARE(addressModel.rowCount(), 1);
  // Check address data - row 0
  index = addressModel.index(0, 1); // StreetName
  QCOMPARE(addressModel.data(index), QVariant("Zeta street 1"));
  index = addressModel.index(0, 3); // Client_Id_FK
  QCOMPARE(addressModel.data(index), QVariant(3));
  // Go to last client
  clientAdressRelation.setParentCurrentIndex(3);
  // Check client data
  QCOMPARE(clientModel.rowCount(), 4);
  index = clientModel.index(3, 0);  // Id_PK
  QCOMPARE(clientModel.data(index), QVariant(4));
  index = clientModel.index(3, 1);  // FirstName
  QCOMPARE(clientModel.data(index), QVariant("Charly"));
  // Check address count
  QCOMPARE(addressModel.rowCount(), 0);
  // Go back to third client
  clientAdressRelation.setParentCurrentIndex(2);
  // Check client data
  QCOMPARE(clientModel.rowCount(), 4);
  index = clientModel.index(2, 0);  // Id_PK
  QCOMPARE(clientModel.data(index), QVariant(3));
  index = clientModel.index(2, 1);  // FirstName
  QCOMPARE(clientModel.data(index), QVariant("Zeta"));
  // Check address count
  QCOMPARE(addressModel.rowCount(), 1);
  // Check address data - row 0
  index = addressModel.index(0, 1); // StreetName
  QCOMPARE(addressModel.data(index), QVariant("Zeta street 1"));
  index = addressModel.index(0, 3); // Client_Id_FK
  QCOMPARE(addressModel.data(index), QVariant(3));
  /*
   * Check 1-1 relation
   */
  // Setup relation
  clientDetailRelation.setParentModel(&clientModel);
  clientDetailRelation.setChildModel(&detailModel);
  relationInfo.clear();
  relationInfo.setRelationType(mdtSqlRelationInfo::OneToOne);
  relationInfo.addRelation("Id_PK", "Client_Id_FK_PK", true);
  QVERIFY(clientDetailRelation.addRelations(relationInfo));
  // Go to first client
  clientDetailRelation.setParentCurrentIndex(0);
  QCOMPARE(clientModel.rowCount(), 4);
  index = clientModel.index(0, 0);  // Id_PK
  QCOMPARE(clientModel.data(index), QVariant(1));
  index = clientModel.index(0, 1);  // FirstName
  QCOMPARE(clientModel.data(index), QVariant("Andy"));
  // Check details count
  QCOMPARE(detailModel.rowCount(), 0);
  // Insert a new client
  QVERIFY(clientModel.insertRow(4));
  QCOMPARE(clientModel.rowCount(), 5);
  clientDetailRelation.setParentCurrentRow(4);
  QVERIFY(detailModel.insertRow(0));
  QCOMPARE(detailModel.rowCount(), 1);
  // Edit new record
  index = clientModel.index(4, 1); // FirstName
  QVERIFY(clientModel.setData(index, "New client 1"));
  index = detailModel.index(0, 1);
  QVERIFY(detailModel.setData(index, "Detail about New client 1"));
  // Save new record
  QVERIFY(clientModel.submitAll());
  QVERIFY(detailModel.submitAll());
  clientDetailRelation.setParentCurrentRow(4);
  // Check client data
  QCOMPARE(clientModel.rowCount(), 5);
  index = clientModel.index(4, 0);  // Id_PK
  QCOMPARE(clientModel.data(index), QVariant(5));
  index = clientModel.index(4, 1);  // FirstName
  QCOMPARE(clientModel.data(index), QVariant("New client 1"));
  // Check detail data
  QCOMPARE(detailModel.rowCount(), 1);
  index = detailModel.index(0, 0);  // Client_Id_FK_PK
  QCOMPARE(detailModel.data(index), QVariant(5));
  index = detailModel.index(0, 1);  // Detail
  QCOMPARE(detailModel.data(index), QVariant("Detail about New client 1"));
  // Go to first client
  clientDetailRelation.setParentCurrentRow(0);
  QCOMPARE(clientModel.rowCount(), 5);
  index = clientModel.index(0, 0);  // Id_PK
  QCOMPARE(clientModel.data(index), QVariant(1));
  index = clientModel.index(0, 1);  // FirstName
  QCOMPARE(clientModel.data(index), QVariant("Andy"));
  // Check details count
  QCOMPARE(detailModel.rowCount(), 0);
  // Go back to last client
  clientDetailRelation.setParentCurrentRow(4);
  QCOMPARE(clientModel.rowCount(), 5);
  index = clientModel.index(4, 0);  // Id_PK
  QCOMPARE(clientModel.data(index), QVariant(5));
  index = clientModel.index(4, 1);  // FirstName
  QCOMPARE(clientModel.data(index), QVariant("New client 1"));
  // Check details count
  QCOMPARE(detailModel.rowCount(), 1);
  // Check detail data
  QCOMPARE(detailModel.rowCount(), 1);
  index = detailModel.index(0, 0);  // Client_Id_FK_PK
  QCOMPARE(detailModel.data(index), QVariant(5));
  index = detailModel.index(0, 1);  // Detail
  QCOMPARE(detailModel.data(index), QVariant("Detail about New client 1"));
  // Insert a new client
  QVERIFY(clientModel.insertRow(5));
  QCOMPARE(clientModel.rowCount(), 6);
  clientDetailRelation.setParentCurrentRow(5);
  QVERIFY(detailModel.insertRow(0));
  QCOMPARE(detailModel.rowCount(), 1);
  // Edit new record
  index = clientModel.index(5, 1); // FirstName
  QVERIFY(clientModel.setData(index, "New client 2"));
  index = detailModel.index(0, 1);
  QVERIFY(detailModel.setData(index, "Detail about New client 2"));
  // Revert new record
  QCOMPARE(clientModel.rowCount(), 6);
  QVERIFY(clientModel.removeRow(5));
  QVERIFY(detailModel.removeRow(0));
  clientDetailRelation.setParentCurrentRow(4);
  // Check that are back to New client 1
  QCOMPARE(clientModel.rowCount(), 5);
  index = clientModel.index(4, 0);  // Id_PK
  QCOMPARE(clientModel.data(index), QVariant(5));
  index = clientModel.index(4, 1);  // FirstName
  QCOMPARE(clientModel.data(index), QVariant("New client 1"));
  // Check details count
  QCOMPARE(detailModel.rowCount(), 1);
  // Check detail data
  QCOMPARE(detailModel.rowCount(), 1);
  index = detailModel.index(0, 0);  // Client_Id_FK_PK
  QCOMPARE(detailModel.data(index), QVariant(5));
  index = detailModel.index(0, 1);  // Detail
  QCOMPARE(detailModel.data(index), QVariant("Detail about New client 1"));
  // Remove client
  QVERIFY(detailModel.removeRow(0));
  detailModel.submitAll();
  qDebug() << detailModel.lastError();
  QVERIFY(detailModel.submitAll());
  QVERIFY(clientModel.removeRow(4));
  QVERIFY(clientModel.submitAll());
  clientDetailRelation.setParentCurrentRow(4);
  // Check that are back to Charly
  QCOMPARE(clientModel.rowCount(), 4);
  index = clientModel.index(3, 0);  // Id_PK
  QCOMPARE(clientModel.data(index), QVariant(4));
  index = clientModel.index(3, 1);  // FirstName
  QCOMPARE(clientModel.data(index), QVariant("Charly"));
  // Check details count
  QCOMPARE(detailModel.rowCount(), 0);
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
  field.setRequired(true);
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
  QCOMPARE(fh.data(), QVariant("ABCD"));
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
  field.setRequired(true);
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
  field.setRequired(true);
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
  field.setRequired(true);
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
  field.setRequired(true);
  field.setReadOnly(true);
  fh.setField(field);
  // Check widget flags
  QVERIFY(pte.isEnabled());
  QVERIFY(pte.isReadOnly());

  /*
   * Check with mdtDoubleEdit
   */
  mdtDoubleEdit de;
  // Setup field
  field.clear();
  field.setType(QVariant::Double);
  field.setRequired(true);
  field.setReadOnly(false);
  // Setup double edit
  de.setEditionMode(mdtDoubleEdit::DefaultEditionMode);
  de.setMinimumToMinusInfinity();
  de.setMaximumToInfinity();
  // Setup field handler
  fh.setField(field);
  fh.setDataWidget(&de);
  de.show();
  // Check field handler flags
  QVERIFY(fh.isNull());
  QVERIFY(!fh.dataWasEdited());
  QVERIFY(!fh.isReadOnly());
  // Check widget flags
  QVERIFY(de.isEnabled());
  QVERIFY(de.value().isNull());
  QVERIFY(!de.lineEdit()->isReadOnly());
  // User edit ...
  qDebug() << "TEST - input 2.5 ...";
  de.lineEdit()->clear();
  QTest::keyClicks(de.lineEdit(), "2.5");
  // Check field handler flags
  QVERIFY(fh.checkBeforeSubmit());
  QVERIFY(!fh.isNull());
  QVERIFY(fh.dataWasEdited());
  QCOMPARE(fh.data(), QVariant(2.5));
  // Set value from database
  fh.setData(3.2);
  QVERIFY(!fh.isNull());
  QVERIFY(!fh.dataWasEdited());
  QCOMPARE(fh.data(), QVariant(3.2));
  QCOMPARE(de.lineEdit()->text().trimmed(), QString("3.2"));
  // Set a null value from database, then a non null one (added when searching a bug, 2015/04/12)
  fh.setData(QVariant());
  QVERIFY(fh.isNull());
  QVERIFY(!fh.dataWasEdited());
  QCOMPARE(fh.data(), QVariant());
  QCOMPARE(de.lineEdit()->text().trimmed(), QString(""));
  fh.setData(1.2);
  QVERIFY(!fh.isNull());
  QVERIFY(!fh.dataWasEdited());
  QCOMPARE(fh.data(), QVariant(1.2));
  QCOMPARE(de.lineEdit()->text().trimmed(), QString("1.2"));

  // Check with +infinity value - Input by user
  de.setPlusInfinity();
  QVERIFY(fh.checkBeforeSubmit());
  QVERIFY(!fh.isNull());
  QVERIFY(fh.dataWasEdited());
  QCOMPARE(fh.data(), QVariant(std::numeric_limits<double>::infinity()));
  // Check with +infinity value - Input by database
  fh.setData(std::numeric_limits<double>::infinity());
  QVERIFY(!fh.isNull());
  QVERIFY(!fh.dataWasEdited());
  QCOMPARE(fh.data(), QVariant(std::numeric_limits<double>::infinity()));
  // Check with -infinity value - Input by user
  de.setMinusInfinity();
  QVERIFY(fh.checkBeforeSubmit());
  QVERIFY(!fh.isNull());
  QVERIFY(fh.dataWasEdited());
  QCOMPARE(fh.data(), QVariant(-std::numeric_limits<double>::infinity()));
  // Check with -infinity value - Input by database
  fh.setData(-std::numeric_limits<double>::infinity());
  QVERIFY(!fh.isNull());
  QVERIFY(!fh.dataWasEdited());
  QCOMPARE(fh.data(), QVariant(-std::numeric_limits<double>::infinity()));
  // Check not required field when user not enter anything
  field.setRequired(false);
  fh.setField(field);
  fh.setData(QVariant());
  QVERIFY(fh.isNull());
  QVERIFY(!fh.dataWasEdited());
  QVERIFY(fh.checkBeforeSubmit());
  QVERIFY(fh.isNull());
  QVERIFY(!fh.dataWasEdited());

  /*
   * Play
   */
  /*
  while(de.isVisible()){
    QTest::qWait(1000);
  }
  */
}

void mdtDatabaseWidgetTest::sqlDataWidgetControllerTest()
{
  mdtDatabaseWidgetTestScenario1Data scenario1(pvDatabase);
  QSqlQuery q(pvDatabase);
  sqlDataWidgetControllerTestWidget w;
  ///std::shared_ptr<mdtUiMessageHandler> messageHandler(new mdtUiMessageHandler(&w));
  mdtSqlDataWidgetController wc;
  std::shared_ptr<QSqlTableModel> m1(new QSqlTableModel(0, pvDatabase));
  std::shared_ptr<QSqlTableModel> m2(new QSqlTableModel(0, pvDatabase));
  std::shared_ptr<QSqlTableModel> model;
  QVariant data;
  QStringList fields;
  mdtSqlRecord rec;
  QList<QVariant> dataList;
  bool ok;

  // For this test, we wont foreign_keys support
  QVERIFY(q.exec("PRAGMA foreign_keys = OFF"));
  // Create test data
  QVERIFY(scenario1.populate());
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
  QVERIFY(wc.model()->editStrategy() == QSqlTableModel::OnManualSubmit);
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
  wc.setTableName("Client_tbl", pvDatabase, "Clients");
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
  QVERIFY(!w.fld_SomeValueDouble->isEnabled());
  QVERIFY(w.fld_FirstName->text().isEmpty());
  QVERIFY(w.fld_Remarks->text().isEmpty());
  QVERIFY(w.fld_SomeValueDouble->isNull());
  // Check control buttons states
  /**
  QVERIFY(!w.toFirstEnabled);
  QVERIFY(!w.toLastEnabled);
  QVERIFY(!w.toNextEnabled);
  QVERIFY(!w.toPreviousEnabled);
  QVERIFY(!w.submitEnabled);
  QVERIFY(!w.revertEnabled);
  QVERIFY(!w.insertEnabled); /// \todo Define the correct behaviour when model was never selected
  QVERIFY(!w.removeEnabled); /// \todo Define the correct behaviour when model was never selected
  */
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
  QVERIFY(!w.fld_SomeValueDouble->isEnabled());
  QVERIFY(w.fld_FirstName->text().isEmpty());
  QVERIFY(w.fld_Remarks->text().isEmpty());
  QVERIFY(w.fld_SomeValueDouble->isNull());
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
  QVERIFY(w.fld_SomeValueDouble->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Andy"));
  QVERIFY(w.fld_Remarks->text().isEmpty());
  QVERIFY(w.fld_SomeValueDouble->isNull());
  QVERIFY(wc.allDataAreSaved());
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
  QVERIFY(w.fld_SomeValueDouble->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Bety"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark on Bety"));
  QCOMPARE(w.fld_SomeValueDouble->value(), QVariant(2.0));
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
  QVERIFY(w.fld_SomeValueDouble->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Andy"));
  QVERIFY(w.fld_Remarks->text().isEmpty());
  QVERIFY(w.fld_SomeValueDouble->isNull());
  QVERIFY(!w.toFirstEnabled);
  QVERIFY(w.toLastEnabled);
  QVERIFY(w.toNextEnabled);
  QVERIFY(!w.toPreviousEnabled);
  wc.toNext();
  QCOMPARE(wc.currentRow(), 1);
  QVERIFY(w.fld_FirstName->isEnabled());
  QVERIFY(w.fld_Remarks->isEnabled());
  QVERIFY(w.fld_SomeValueDouble->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Bety"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark on Bety"));
  QCOMPARE(w.fld_SomeValueDouble->value(), QVariant(2.0));
  QVERIFY(w.toFirstEnabled);
  QVERIFY(w.toLastEnabled);
  QVERIFY(w.toNextEnabled);
  QVERIFY(w.toPreviousEnabled);
  wc.toPrevious();
  QCOMPARE(wc.currentRow(), 0);
  QVERIFY(w.fld_FirstName->isEnabled());
  QVERIFY(w.fld_Remarks->isEnabled());
  QVERIFY(w.fld_SomeValueDouble->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Andy"));
  QVERIFY(w.fld_Remarks->text().isEmpty());
  QVERIFY(w.fld_SomeValueDouble->isNull());
  QVERIFY(!w.toFirstEnabled);
  QVERIFY(w.toLastEnabled);
  QVERIFY(w.toNextEnabled);
  QVERIFY(!w.toPreviousEnabled);
  wc.toLast();
  QCOMPARE(wc.currentRow(), 3);
  QVERIFY(w.fld_FirstName->isEnabled());
  QVERIFY(w.fld_Remarks->isEnabled());
  QVERIFY(w.fld_SomeValueDouble->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Charly"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark on Charly"));
  QVERIFY(w.fld_SomeValueDouble->isNull());
  QVERIFY(w.toFirstEnabled);
  QVERIFY(!w.toLastEnabled);
  QVERIFY(!w.toNextEnabled);
  QVERIFY(w.toPreviousEnabled);
  wc.toFirst();
  QCOMPARE(wc.currentRow(), 0);
  QVERIFY(w.fld_FirstName->isEnabled());
  QVERIFY(w.fld_Remarks->isEnabled());
  QVERIFY(w.fld_SomeValueDouble->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Andy"));
  QVERIFY(w.fld_Remarks->text().isEmpty());
  QVERIFY(w.fld_SomeValueDouble->isNull());
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
   *  Because of internal state machine,
   *  witch runs asynchronousliy, we must wait between each action that is not explicitly synch (xyAndWait() functions)
   */
  QCOMPARE(wc.rowCount(), 4);
  QCOMPARE(wc.currentRow(), 0);
  // Insert a record (asynch version)
  wc.insert();
  QTest::qWait(50);
  QCOMPARE(wc.rowCount(), 5);
  QCOMPARE(wc.currentRow(), 4);
  QVERIFY(w.fld_FirstName->isEnabled());
  QVERIFY(w.fld_Remarks->isEnabled());
  QVERIFY(w.fld_SomeValueDouble->isEnabled());
  QVERIFY(w.fld_FirstName->text().isEmpty());
  QVERIFY(w.fld_Remarks->text().isEmpty());
  QVERIFY(w.fld_SomeValueDouble->isNull());
  QTest::keyClicks(w.fld_FirstName, "New name 1");
  QTest::keyClicks(w.fld_Remarks, "New remark 1");
  QTest::keyClicks(w.fld_SomeValueDouble->lineEdit(), "5");
  QVERIFY(!wc.allDataAreSaved());
  // Submit with asynch version
  wc.submit();
  QTest::qWait(1000); // Writing in DB can be very slow, f.ex. with Sqlite on HDD
  // Check that model was updated
  QVERIFY(wc.allDataAreSaved());
  QCOMPARE(wc.rowCount(), 5);
  QCOMPARE(wc.currentRow(), 4);
  data = model->data(model->index(4, model->fieldIndex("FirstName")));
  QCOMPARE(data.toString(), QString("New name 1"));
  data = model->data(model->index(4, model->fieldIndex("Remarks")));
  QCOMPARE(data.toString(), QString("New remark 1"));
  data = model->data(model->index(4, model->fieldIndex("SomeValueDouble")));
  QCOMPARE(data, QVariant(5.0));
  // Check that data() works
  QCOMPARE(wc.data(4, "FirstName", ok), QVariant("New name 1"));
  QVERIFY(ok);
  QVERIFY(wc.data(200, "FirstName", ok).isNull());
  QVERIFY(!ok);
  QCOMPARE(wc.data(4, "FirstName"), QVariant("New name 1"));
  QCOMPARE(wc.data(4, "SomeValueDouble"), QVariant(5.0));
  // Check that currentData() works
  QCOMPARE(wc.currentData("FirstName", ok), QVariant("New name 1"));
  QVERIFY(ok);
  QCOMPARE(wc.currentData("Remarks", ok), QVariant("New remark 1"));
  QVERIFY(ok);
  QCOMPARE(wc.currentData("FirstName"), QVariant("New name 1"));
  QCOMPARE(wc.currentData("Remarks"), QVariant("New remark 1"));
  QCOMPARE(wc.currentData("SomeValueDouble"), QVariant(5.0));
  qDebug() << "Formated FirstName: " << wc.currentFormatedValue("FirstName");
  qDebug() << "Formated FirstName: " << wc.currentFormatedValue("Remarks");
  // Check that widget displays the correct row
  QCOMPARE(w.fld_FirstName->text(), QString("New name 1"));
  QCOMPARE(w.fld_Remarks->text(), QString("New remark 1"));
  QCOMPARE(w.fld_SomeValueDouble->value(), QVariant(5.0));
  // Insert a record (synch version)
  QVERIFY(wc.insertAndWait());
  QCOMPARE(wc.rowCount(), 6);
  QCOMPARE(wc.currentRow(), 5);
  QVERIFY(w.fld_FirstName->isEnabled());
  QVERIFY(w.fld_Remarks->isEnabled());
  QVERIFY(w.fld_SomeValueDouble->isEnabled());
  QVERIFY(w.fld_FirstName->text().isEmpty());
  QVERIFY(w.fld_Remarks->text().isEmpty());
  QVERIFY(w.fld_SomeValueDouble->isNull());
  QTest::keyClicks(w.fld_FirstName, "New name 2");
  QTest::keyClicks(w.fld_Remarks, "New remark 2");
  QTest::keyClicks(w.fld_SomeValueDouble->lineEdit(), "6");
  QVERIFY(!wc.allDataAreSaved());
  // Submit with synch version
  QVERIFY(wc.submitAndWait());
  QVERIFY(wc.allDataAreSaved());
  // Check that model was updated
  QCOMPARE(model->rowCount(), 6);
  QCOMPARE(wc.rowCount(), 6);
  QCOMPARE(wc.currentRow(), 5);
  data = model->data(model->index(5, model->fieldIndex("FirstName")));
  QCOMPARE(data.toString(), QString("New name 2"));
  data = model->data(model->index(5, model->fieldIndex("Remarks")));
  QCOMPARE(data.toString(), QString("New remark 2"));
  data = model->data(model->index(5, model->fieldIndex("SomeValueDouble")));
  QCOMPARE(data, QVariant(6.0));
  // Check that widget displays the correct row
  QCOMPARE(w.fld_FirstName->text(), QString("New name 2"));
  QCOMPARE(w.fld_Remarks->text(), QString("New remark 2"));
  QCOMPARE(w.fld_SomeValueDouble->value(), QVariant(6.0));
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
   * Check currentRow after inert, revert
   */
  wc.toFirst();
  QTest::qWait(50);
  wc.toNext();
  QTest::qWait(50);
  QCOMPARE(wc.currentRow(), 1);
  wc.insert();
  QTest::qWait(50);
  wc.revert();
  QTest::qWait(50);
  QCOMPARE(wc.currentRow(), 1);
  /*
   * Check edition
   *  - Edit current row
   *  - Check in model
   *  - Check in widgets
   */
  wc.toLast();
  QTest::qWait(50);
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
  QVERIFY(wc.removeAndWait());
  /**
  wc.remove();
  QTest::qWait(1000); // Writing in DB can be very slow, f.ex. with Sqlite on HDD
  */
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
  scenario1.clear();
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
  QVERIFY(scenario1.populate1000Names());
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
  QVERIFY(wc.insertAndWait());
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
  QVERIFY(wc.removeAndWait());
  // Check that model was updated
  QCOMPARE(wc.currentRow(), 399);
  // Check that widget displays the correct row
  QCOMPARE(w.fld_FirstName->text(), QString("Name 401"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark 401"));
  QCOMPARE(wc.rowCount(), 1000);
  /*
   * Check set current row with match data
   */
  QVERIFY(wc.select());
  QVERIFY(wc.rowCount(false) < 1000);
  QCOMPARE(wc.currentRow(), 0);
  QVERIFY(wc.setCurrentRow("FirstName", "Name 600"));
  QCOMPARE(wc.currentData("FirstName"), QVariant("Name 600"));
  QVERIFY(!wc.setCurrentRow("FirstName", "Non existing name"));
  QCOMPARE(wc.currentData("FirstName"), QVariant("Name 600"));
  /*
   * Check data with match data
   */
  QVERIFY(wc.select());
  QVERIFY(wc.rowCount(false) < 1000);
  QCOMPARE(wc.data("FirstName", "Name 10", "Remarks", ok), QVariant("Remark 10"));
  QVERIFY(ok);
  QCOMPARE(wc.data("FirstName", "Name 1000", "Remarks", ok), QVariant("Remark 1000"));
  QVERIFY(ok);
  QCOMPARE(wc.data("FirstName", "Name 500", "Remarks", ok), QVariant("Remark 500"));
  QVERIFY(ok);
  QVERIFY(wc.data("FirstName", "Name 99999999", "Remarks", ok).isNull());
  QVERIFY(!ok);
  QCOMPARE(wc.data("FirstName", "Name 2", "Remarks"), QVariant("Remark 2"));
  /*
   * Check sorting
   */
  // Repopulate original test data
  scenario1.clear();
  QVERIFY(scenario1.populate());
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
   * Check data list
   */
  dataList = wc.dataList("FirstName");
  QCOMPARE(dataList.size(), 5);
  QCOMPARE(dataList.at(0), QVariant("Andy"));
  QCOMPARE(dataList.at(1), QVariant("Bety"));
  QCOMPARE(dataList.at(2), QVariant("Charly"));
  QCOMPARE(dataList.at(3), QVariant("Laura"));
  QCOMPARE(dataList.at(4), QVariant("Zeta"));
  /*
   * Check programmed edition - No submit
   */
  QVERIFY(wc.allDataAreSaved(true));
  QVERIFY(wc.setCurrentData("Remarks", "Edited remark on Zeta", false));
  QVERIFY(!wc.allDataAreSaved(true));
  QCOMPARE(w.fld_FirstName->text(), QString("Zeta"));
  QCOMPARE(w.fld_Remarks->text(), QString("Edited remark on Zeta"));
  QCOMPARE(wc.currentData("FirstName"), QVariant("Zeta"));
  QCOMPARE(wc.currentData("Remarks"), QVariant("Edited remark on Zeta"));
  wc.revert();
  QTest::qWait(50);
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
  QVERIFY(wc.allDataAreSaved(true));
  QVERIFY(wc.setCurrentData("Remarks", "Edited remark on Zeta", true));
  QVERIFY(wc.allDataAreSaved(true));
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
  /// \note Many row edition disabled because row changing is not allowed without saving data.
//   // Edit each row
//   wc.toFirst();
//   QTest::qWait(50);
//   QVERIFY(wc.setCurrentData("Remarks", "Edited remark on Andy", false));
//   wc.toNext();
//   QTest::qWait(50);
//   QVERIFY(wc.setCurrentData("Remarks", "Edited remark on Bety", false));
//   wc.toNext();
//   QTest::qWait(50);
//   QVERIFY(wc.setCurrentData("Remarks", "Edited remark on Charly", false));
//   wc.toNext();
//   QTest::qWait(50);
//   QVERIFY(wc.setCurrentData("Remarks", "Edited remark on Laura", false));
//   wc.toNext();
//   QTest::qWait(50);
//   QVERIFY(wc.setCurrentData("Remarks", "Edited remark 2 on Zeta", false));
//   QVERIFY(!wc.allDataAreSaved(true));
//   // Submit all rows
//   QVERIFY(wc.submitAndWait());
//   QVERIFY(wc.allDataAreSaved(true));
//   // Check that database was updated
//   QVERIFY(wc.select());
//   wc.sort();
//   wc.toFirst();
//   QTest::qWait(50);
//   QCOMPARE(w.fld_FirstName->text(), QString("Andy"));
//   QCOMPARE(w.fld_Remarks->text(), QString("Edited remark on Andy"));
//   QCOMPARE(wc.currentData("FirstName"), QVariant("Andy"));
//   QCOMPARE(wc.currentData("Remarks"), QVariant("Edited remark on Andy"));
//   // Go to next row and check
//   wc.toNext();
//   QTest::qWait(50);
//   QCOMPARE(w.fld_FirstName->text(), QString("Bety"));
//   QCOMPARE(w.fld_Remarks->text(), QString("Edited remark on Bety"));
//   QCOMPARE(wc.currentData("FirstName"), QVariant("Bety"));
//   QCOMPARE(wc.currentData("Remarks"), QVariant("Edited remark on Bety"));
//   // Go to next row and check
//   wc.toNext();
//   QTest::qWait(50);
//   QCOMPARE(w.fld_FirstName->text(), QString("Charly"));
//   QCOMPARE(w.fld_Remarks->text(), QString("Edited remark on Charly"));
//   QCOMPARE(wc.currentData("FirstName"), QVariant("Charly"));
//   QCOMPARE(wc.currentData("Remarks"), QVariant("Edited remark on Charly"));
//   // Go to next row and check
//   wc.toNext();
//   QTest::qWait(50);
//   QCOMPARE(w.fld_FirstName->text(), QString("Laura"));
//   QCOMPARE(w.fld_Remarks->text(), QString("Edited remark on Laura"));
//   QCOMPARE(wc.currentData("FirstName"), QVariant("Laura"));
//   QCOMPARE(wc.currentData("Remarks"), QVariant("Edited remark on Laura"));
//   // Go to next row and check
//   wc.toNext();
//   QTest::qWait(50);
//   QCOMPARE(w.fld_FirstName->text(), QString("Zeta"));
//   QCOMPARE(w.fld_Remarks->text(), QString("Edited remark 2 on Zeta"));
//   QCOMPARE(wc.currentData("FirstName"), QVariant("Zeta"));
//   QCOMPARE(wc.currentData("Remarks"), QVariant("Edited remark 2 on Zeta"));
  /*
   * Check programmed edition:
   *  - Edit field(s)
   *  - Submit the row at once
   */
  // Edit row
  wc.toFirst();
  QTest::qWait(50);
  QVERIFY(wc.setCurrentData("Remarks", "Edited remark on Andy", false));
  // Submit row
  QVERIFY(!wc.allDataAreSaved(true));
  QVERIFY(wc.submitAndWait());
  QVERIFY(wc.allDataAreSaved(true));
  // Edit next row
  wc.toNext();
  QTest::qWait(50);
  QVERIFY(wc.setCurrentData("Remarks", "Edited remark on Bety", false));
  // Submit row
  QVERIFY(!wc.allDataAreSaved(true));
  QVERIFY(wc.submitAndWait());
  QVERIFY(wc.allDataAreSaved(true));
  // Edit next row
  wc.toNext();
  QTest::qWait(50);
  QVERIFY(wc.setCurrentData("Remarks", "Edited remark on Charly", false));
  // Submit row
  QVERIFY(!wc.allDataAreSaved(true));
  QVERIFY(wc.submitAndWait());
  QVERIFY(wc.allDataAreSaved(true));
  // Edit next row
  wc.toNext();
  QTest::qWait(50);
  QVERIFY(wc.setCurrentData("Remarks", "Edited remark on Laura", false));
  // Submit row
  QVERIFY(!wc.allDataAreSaved(true));
  QVERIFY(wc.submitAndWait());
  QVERIFY(wc.allDataAreSaved(true));
  // Edit next row
  wc.toNext();
  QTest::qWait(50);
  QVERIFY(wc.setCurrentData("Remarks", "Edited remark 2 on Zeta", false));
  // Submit row
  QVERIFY(!wc.allDataAreSaved(true));
  QVERIFY(wc.submitAndWait());
  QVERIFY(wc.allDataAreSaved(true));
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
  /*
   * Check filter
   */
  // Check simple filter
  QVERIFY(wc.setFilter("Id_PK", 1));
  QCOMPARE(wc.rowCount(false), 1);
  QCOMPARE(w.fld_FirstName->text(), QString("Andy"));
  QCOMPARE(w.fld_Remarks->text(), QString("Edited remark on Andy"));
  QCOMPARE(wc.currentData("FirstName"), QVariant("Andy"));
  QCOMPARE(wc.currentData("Remarks"), QVariant("Edited remark on Andy"));
  // Check multiple values filter
  dataList.clear();
  dataList << 2 << 3 << 4;
  QVERIFY(wc.setFilter("Id_PK", dataList));
  QCOMPARE(wc.rowCount(false), 3);
  QCOMPARE(w.fld_FirstName->text(), QString("Bety"));
  QCOMPARE(w.fld_Remarks->text(), QString("Edited remark on Bety"));
  QCOMPARE(wc.currentData("FirstName"), QVariant("Bety"));
  QCOMPARE(wc.currentData("Remarks"), QVariant("Edited remark on Bety"));
  // Clear filter
  wc.clearFilter();
  QCOMPARE(wc.rowCount(false), 5);
  QCOMPARE(w.fld_FirstName->text(), QString("Andy"));
  QCOMPARE(w.fld_Remarks->text(), QString("Edited remark on Andy"));
  QCOMPARE(wc.currentData("FirstName"), QVariant("Andy"));
  QCOMPARE(wc.currentData("Remarks"), QVariant("Edited remark on Andy"));
  /*
   * Check record() method
   */
  fields.clear();
  fields << "Id_PK" << "FirstName";
  rec = wc.record(0, fields);
  QCOMPARE(rec.count(), 2);
  QVERIFY(rec.field("Id_PK").type() == QVariant::Int);
  QVERIFY(rec.field("FirstName").type() == QVariant::String);
  QCOMPARE(rec.value("Id_PK"), QVariant(1));
  QCOMPARE(rec.value("FirstName"), QVariant("Andy"));
  fields.clear();
  fields << "Remarks" << "FirstName";
  rec = wc.record(0, fields);
  QCOMPARE(rec.count(), 2);
  QVERIFY(rec.field("FirstName").type() == QVariant::String);
  QVERIFY(rec.field("Remarks").type() == QVariant::String);
  QCOMPARE(rec.value("FirstName"), QVariant("Andy"));
  QCOMPARE(rec.value("Remarks"), QVariant("Edited remark on Andy"));
  rec = wc.record(2, fields);
  QCOMPARE(rec.count(), 2);
  QVERIFY(rec.field("FirstName").type() == QVariant::String);
  QVERIFY(rec.field("Remarks").type() == QVariant::String);
  QCOMPARE(rec.value("FirstName"), QVariant("Charly"));
  QCOMPARE(rec.value("Remarks"), QVariant("Edited remark on Charly"));
  fields.clear();
  fields << "FirstName" << "Remarks";
  rec = wc.record(0, fields);
  QCOMPARE(rec.count(), 2);
  QVERIFY(rec.field("FirstName").type() == QVariant::String);
  QVERIFY(rec.field("Remarks").type() == QVariant::String);
  QCOMPARE(rec.value("FirstName"), QVariant("Andy"));
  QCOMPARE(rec.value("Remarks"), QVariant("Edited remark on Andy"));
  rec = wc.record(1, fields);
  QCOMPARE(rec.count(), 2);
  QVERIFY(rec.field("FirstName").type() == QVariant::String);
  QVERIFY(rec.field("Remarks").type() == QVariant::String);
  QCOMPARE(rec.value("FirstName"), QVariant("Bety"));
  QCOMPARE(rec.value("Remarks"), QVariant("Edited remark on Bety"));
  rec = wc.record(2, fields);
  QCOMPARE(rec.count(), 2);
  QVERIFY(rec.field("FirstName").type() == QVariant::String);
  QVERIFY(rec.field("Remarks").type() == QVariant::String);
  QCOMPARE(rec.value("FirstName"), QVariant("Charly"));
  QCOMPARE(rec.value("Remarks"), QVariant("Edited remark on Charly"));
  rec = wc.record(3, fields);
  QCOMPARE(rec.count(), 2);
  QVERIFY(rec.field("FirstName").type() == QVariant::String);
  QVERIFY(rec.field("Remarks").type() == QVariant::String);
  QCOMPARE(rec.value("FirstName"), QVariant("Laura"));
  QCOMPARE(rec.value("Remarks"), QVariant("Edited remark on Laura"));
  rec = wc.record(4, fields);
  QCOMPARE(rec.count(), 2);
  QVERIFY(rec.field("FirstName").type() == QVariant::String);
  QVERIFY(rec.field("Remarks").type() == QVariant::String);
  QCOMPARE(rec.value("FirstName"), QVariant("Zeta"));
  QCOMPARE(rec.value("Remarks"), QVariant("Edited remark 2 on Zeta"));
  /*
   * Check record() method with match
   */
  fields.clear();
  fields << "Id_PK" << "FirstName";
  rec = wc.record("Id_PK", 1, fields, ok);
  QVERIFY(ok);
  QCOMPARE(rec.count(), 2);
  QVERIFY(rec.field("Id_PK").type() == QVariant::Int);
  QVERIFY(rec.field("FirstName").type() == QVariant::String);
  QCOMPARE(rec.value("Id_PK"), QVariant(1));
  QCOMPARE(rec.value("FirstName"), QVariant("Andy"));
  rec = wc.record("Id_PK", 4, fields);
  QCOMPARE(rec.count(), 2);
  QVERIFY(rec.field("Id_PK").type() == QVariant::Int);
  QVERIFY(rec.field("FirstName").type() == QVariant::String);
  QCOMPARE(rec.value("Id_PK"), QVariant(4));
  QCOMPARE(rec.value("FirstName"), QVariant("Charly"));
  /*
   * Check programmed edition with match data - No submit
   */
  QCOMPARE(wc.data("Id_PK", 2, "FirstName"), QVariant("Bety"));
  QVERIFY(wc.setData("Id_PK", 2, "FirstName", "Bety (P edited)", false));
  QCOMPARE(wc.data("Id_PK", 2, "FirstName"), QVariant("Bety (P edited)"));
  wc.revert();
  QTest::qWait(50);
  QVERIFY(wc.select());
  QCOMPARE(wc.data("Id_PK", 2, "FirstName"), QVariant("Bety"));
  /*
   * Check programmed edition with match data - Direct submit
   */
  QVERIFY(wc.setData("Id_PK", 2, "FirstName", "Bety (P edited)", true));
  QCOMPARE(wc.data("Id_PK", 2, "FirstName"), QVariant("Bety (P edited)"));
  QVERIFY(wc.select());
  QCOMPARE(wc.data("Id_PK", 2, "FirstName"), QVariant("Bety (P edited)"));
  /*
   * Check programmed edition:
   *  - Edit many rows
   *  - Submit all at once
   */
  // Edit each row
  QVERIFY(wc.setData("Id_PK", 1, "FirstName", "Andy (P edited)", false));
  QVERIFY(wc.setData("Id_PK", 2, "FirstName", "Bety (P edited)", false));
  QVERIFY(wc.setData("Id_PK", 3, "FirstName", "Zeta (P edited)", false));
  QVERIFY(wc.setData("Id_PK", 4, "FirstName", "Charly (P edited)", false));
  // Check cache
  QCOMPARE(wc.data("Id_PK", 1, "FirstName"), QVariant("Andy (P edited)"));
  QCOMPARE(wc.data("Id_PK", 2, "FirstName"), QVariant("Bety (P edited)"));
  QCOMPARE(wc.data("Id_PK", 3, "FirstName"), QVariant("Zeta (P edited)"));
  QCOMPARE(wc.data("Id_PK", 4, "FirstName"), QVariant("Charly (P edited)"));
  // Submit, select and check
  QVERIFY(wc.submitAndWait());
  QVERIFY(wc.select());
  QCOMPARE(wc.data("Id_PK", 1, "FirstName"), QVariant("Andy (P edited)"));
  QCOMPARE(wc.data("Id_PK", 2, "FirstName"), QVariant("Bety (P edited)"));
  QCOMPARE(wc.data("Id_PK", 3, "FirstName"), QVariant("Zeta (P edited)"));
  QCOMPARE(wc.data("Id_PK", 4, "FirstName"), QVariant("Charly (P edited)"));
  /*
   * Check data() method with 2 keys
   */
  QCOMPARE(wc.data("Id_PK", 1, "FirstName", "Andy (P edited)", "Remarks", ok), QVariant("Edited remark on Andy"));
  QVERIFY(ok);
  QCOMPARE(wc.data("Id_PK", 1, "FirstName", "Non existing", "Remarks", ok), QVariant());
  QVERIFY(!ok);

  /*
   * Check programmed edition with 2 keys:
   *  - Edit many rows
   *  - Submit all at once
   */
  // Edit some rows
  QVERIFY(wc.setData("Id_PK", 1, "FirstName", "Andy (P edited)", "Remarks", "Changed remark on Andy", false));
  QVERIFY(wc.setData("Id_PK", 2, "FirstName", "Bety (P edited)", "Remarks", "Changed remark on Bety", false));
  // Check cache
  QCOMPARE(wc.data("Id_PK", 1, "Remarks", ok), QVariant("Changed remark on Andy"));
  QVERIFY(ok);
  QCOMPARE(wc.data("Id_PK", 2, "Remarks", ok), QVariant("Changed remark on Bety"));
  QVERIFY(ok);
  // Submit, select and check
  QVERIFY(wc.submitAndWait());
  QVERIFY(wc.select());
  QCOMPARE(wc.data("Id_PK", 1, "Remarks", ok), QVariant("Changed remark on Andy"));
  QVERIFY(ok);
  QCOMPARE(wc.data("Id_PK", 2, "Remarks", ok), QVariant("Changed remark on Bety"));
  QVERIFY(ok);

  // Re-enable foreign_keys support
  QVERIFY(q.exec("PRAGMA foreign_keys = ON"));
}

void mdtDatabaseWidgetTest::sqlDataWidgetControllerRoTest()
{
  mdtDatabaseWidgetTestScenario1Data scenario1(pvDatabase);
  QSqlQuery q(pvDatabase);
  sqlDataWidgetControllerTestWidget w;
  mdtSqlDataWidgetController wc;
  QVariant data;
  QStringList fields;
  mdtSqlRecord rec;
  QList<QVariant> dataList;
  bool ok;

  // For this test, we wont foreign_keys support
  QVERIFY(q.exec("PRAGMA foreign_keys = OFF"));
  // Create test data
  QVERIFY(scenario1.populate());
  // Setup
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
  QCOMPARE(wc.rowCount(), 0);
  QVERIFY(wc.currentRow() < 0);
  QVERIFY(wc.currentState() == mdtAbstractSqlTableController::Stopped);
  /*
   * Set table name (and tell controller that we cannot write to it)
   */
  wc.setCanWriteToDatabase(false);
  wc.setTableName("Client_tbl", pvDatabase, "Clients");
  QCOMPARE(wc.userFriendlyTableName(), QString("Clients"));
  QCOMPARE(wc.rowCount(), 0);
  QVERIFY(wc.currentRow() < 0);
  QVERIFY(!wc.setCurrentRow(-1));
  QVERIFY(!wc.setCurrentRow(0));
  /*
   * Map widgets
   */
  QVERIFY(wc.mapFormWidgets(&w, "fld_FirstName"));
  // Check fields - Because state machine is not running, all widgets must be diseabled
  QVERIFY(!w.fld_FirstName->isEnabled());
  QVERIFY(!w.fld_Remarks->isEnabled());
  QVERIFY(w.fld_FirstName->text().isEmpty());
  QVERIFY(w.fld_Remarks->text().isEmpty());
  /*
   * Start of state machine
   */
  QVERIFY(wc.currentState() == mdtAbstractSqlTableController::Stopped);
  wc.start();
  QVERIFY(wc.currentState() == mdtAbstractSqlTableController::Visualizing);
  // Check fields - Must be all OFF, because model was never selected
  QVERIFY(!w.fld_FirstName->isEnabled());
  QVERIFY(!w.fld_Remarks->isEnabled());
  QVERIFY(w.fld_FirstName->text().isEmpty());
  QVERIFY(w.fld_Remarks->text().isEmpty());
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
  QVERIFY(wc.allDataAreSaved());
  QCOMPARE(wc.currentData("FirstName", ok), QVariant("Andy"));
  QVERIFY(ok);
  QVERIFY(wc.currentData("Remarks", ok).isNull());
  QVERIFY(ok);

//   /*
//    * Check edition
//    *  - Edit current row
//    *  - Check in model
//    *  - Check in widgets
//    *  - Check that database was not updated
//    */

  // Check control buttons states
  QVERIFY(!w.toFirstEnabled);
  QVERIFY(w.toLastEnabled);
  QVERIFY(w.toNextEnabled);
  QVERIFY(!w.toPreviousEnabled);
  QVERIFY(!w.submitEnabled);
  QVERIFY(!w.revertEnabled);
  QVERIFY(w.insertEnabled);
  QVERIFY(w.removeEnabled);
  // Try to edit in widgets
  QTest::keyClicks(w.fld_Remarks, "Edited remark on Andy");
  // Data saved flag must be untouched
  QVERIFY(wc.allDataAreSaved());
  // Check control buttons states - must be keeped unchanged (RO)
  QVERIFY(!w.toFirstEnabled);
  QVERIFY(w.toLastEnabled);
  QVERIFY(w.toNextEnabled);
  QVERIFY(!w.toPreviousEnabled);
  QVERIFY(!w.submitEnabled);
  QVERIFY(!w.revertEnabled);
  QVERIFY(w.insertEnabled);
  QVERIFY(w.removeEnabled);
//   QVERIFY(!w.toFirstEnabled);
//   QVERIFY(!w.toLastEnabled);
//   QVERIFY(!w.toNextEnabled);
//   QVERIFY(!w.toPreviousEnabled);
//   QVERIFY(w.submitEnabled);
//   QVERIFY(w.revertEnabled);
//   QVERIFY(!w.insertEnabled);
//   QVERIFY(!w.removeEnabled);

  // Submit - Must not work
  QVERIFY(!wc.submitAndWait());
  // All data are saved flag must allways be true
  QVERIFY(wc.allDataAreSaved());
  // Check that model is keeped unchanged
  QVERIFY(wc.select());
  QCOMPARE(wc.rowCount(), 4);
  QCOMPARE(wc.currentRow(), 0);
  QCOMPARE(wc.currentData("FirstName", ok), QVariant("Andy"));
  QVERIFY(ok);
  QVERIFY(wc.currentData("Remarks", ok).isNull());
  QVERIFY(ok);
  // Check widget coherence
  QVERIFY(w.fld_FirstName->isEnabled());
  QVERIFY(w.fld_Remarks->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Andy"));
  QVERIFY(w.fld_Remarks->text().isEmpty());
  /*
   * Check navigation and current data
   */
  // Current buttons states
  QVERIFY(!w.toFirstEnabled);
  QVERIFY(w.toLastEnabled);
  QVERIFY(w.toNextEnabled);
  QVERIFY(!w.toPreviousEnabled);
  // Movto next
  wc.toNext();
  // Check navigation buttons states
  QVERIFY(w.toFirstEnabled);
  QVERIFY(w.toLastEnabled);
  QVERIFY(w.toNextEnabled);
  QVERIFY(w.toPreviousEnabled);
  // Check data
  QCOMPARE(wc.currentRow(), 1);
  QCOMPARE(wc.currentData("FirstName", ok), QVariant("Bety"));
  QVERIFY(ok);
  QCOMPARE(wc.currentData("Remarks", ok), QVariant("Remark on Bety"));
  QVERIFY(ok);
  /*
   * Enable sort and check current data
   */
  wc.addColumnToSortOrder("FirstName", Qt::AscendingOrder);
  wc.sort();
  wc.toFirst();
  QCOMPARE(wc.currentData("FirstName", ok), QVariant("Andy"));
  QVERIFY(ok);
  wc.toNext();
  QCOMPARE(wc.currentData("FirstName", ok), QVariant("Bety"));
  QVERIFY(ok);
  wc.toNext();
  QCOMPARE(wc.currentData("FirstName", ok), QVariant("Charly"));
  QVERIFY(ok);
  wc.toNext();
  QCOMPARE(wc.currentData("FirstName", ok), QVariant("Zeta"));
  QVERIFY(ok);


  /**
   * \todo Corriger classe + terminer test !
   */


  
  // Check that model was updated
  QVERIFY(wc.allDataAreSaved());
  QCOMPARE(wc.currentRow(), 0);
  QCOMPARE(wc.currentData("FirstName", ok), QVariant("Andy"));
  QVERIFY(ok);
  QCOMPARE(wc.currentData("Remarks", ok), QVariant("Edited remark on Andy"));
  QVERIFY(ok);
  // Check that widget displays the correct row
  QCOMPARE(w.fld_FirstName->text(), QString("Andy"));
  QCOMPARE(w.fld_Remarks->text(), QString("Edited remark on Andy"));
  // Check control buttons states - after submit
  QVERIFY(!w.toFirstEnabled);
  QVERIFY(w.toLastEnabled);
  QVERIFY(w.toNextEnabled);
  QVERIFY(!w.toPreviousEnabled);
  QVERIFY(!w.submitEnabled);
  QVERIFY(!w.revertEnabled);
  QVERIFY(w.insertEnabled);
  QVERIFY(w.removeEnabled);
  // Select data and check that database was not updated
  QVERIFY(wc.select());
  QCOMPARE(wc.currentRow(), 0);
  QCOMPARE(wc.currentData("FirstName", ok), QVariant("Andy"));
  QVERIFY(ok);
  QVERIFY(wc.currentData("Remarks", ok).isNull());
  QVERIFY(ok);

  // Clear test data
//   clearTestDatabaseData();
  // Re-enable foreign_keys support
  QVERIFY(q.exec("PRAGMA foreign_keys = ON"));
}

void mdtDatabaseWidgetTest::sqlDataWidgetController2tableTest()
{
  mdtDatabaseWidgetTestScenario1Data scenario1(pvDatabase);
  QSqlQuery q(pvDatabase);
  sqlDataWidgetControllerTestWidget w;
  mdtSqlDataWidgetController clientController;
  std::shared_ptr<mdtSqlDataWidgetController> detailController;
  mdtSqlRelationInfo relationInfo;
  QVariant data;
  QVariant clientId;
  QStringList fields;
  mdtSqlRecord rec;
  QList<QVariant> dataList;
  ///bool ok;

  // For this test, we wont foreign_keys support
  ///QVERIFY(q.exec("PRAGMA foreign_keys = OFF"));
  // Create test data
  QVERIFY(scenario1.populate());
  // Setup
  connect(&clientController, SIGNAL(toFirstEnabledStateChanged(bool)), &w, SLOT(setToFirstEnableState(bool)));
  connect(&clientController, SIGNAL(toLastEnabledStateChanged(bool)), &w, SLOT(setToLastEnableState(bool)));
  connect(&clientController, SIGNAL(toNextEnabledStateChanged(bool)), &w, SLOT(setToNextEnableState(bool)));
  connect(&clientController, SIGNAL(toPreviousEnabledStateChanged(bool)), &w, SLOT(setToPreviousEnableState(bool)));
  connect(&clientController, SIGNAL(insertEnabledStateChanged(bool)), &w, SLOT(setInsertEnableState(bool)));
  connect(&clientController, SIGNAL(removeEnabledStateChanged(bool)), &w, SLOT(setRemoveEnableState(bool)));
  connect(&clientController, SIGNAL(submitEnabledStateChanged(bool)), &w, SLOT(setSubmitEnableState(bool)));
  connect(&clientController, SIGNAL(revertEnabledStateChanged(bool)), &w, SLOT(setRevertEnableState(bool)));
  clientController.setTableName("Client_tbl", pvDatabase, "Client");
  relationInfo.setChildTableName("ClientDetail_tbl");
  relationInfo.setRelationType(mdtSqlRelationInfo::OneToOne);
  relationInfo.addRelation("Id_PK", "Client_Id_FK_PK", true);
  QVERIFY(clientController.addChildController<mdtSqlDataWidgetController>(relationInfo, tr("Client")));
  // Map widgets
  QVERIFY(clientController.mapFormWidgets(&w));
  detailController = clientController.childController<mdtSqlDataWidgetController>("ClientDetail_tbl");
  QVERIFY(detailController.get() != 0);
  QVERIFY(detailController->mapFormWidgets(&w));
  // Initial state
  clientController.start();
  w.show();
  QVERIFY(w.fld_FirstName->text().isEmpty());
  QVERIFY(w.fld_Remarks->text().isEmpty());
  QVERIFY(w.fld_Detail->text().isEmpty());
  QVERIFY(!w.fld_FirstName->isEnabled());
  QVERIFY(!w.fld_Remarks->isEnabled());
  QVERIFY(!w.fld_Detail->isEnabled());
  // Select
  QVERIFY(clientController.select());
  QVERIFY(w.fld_FirstName->isEnabled());
  QVERIFY(w.fld_Remarks->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Andy"));
  QVERIFY(w.fld_Remarks->text().isEmpty());
  QVERIFY(w.fld_Detail->text().isEmpty());
  /*
   * Because database was created without any relation
   *  between Client_tbl and ClientDetail_tbl,
   *  the entry for current client not exists in ClientDetail_tbl.
   * We do a insertion, and check that entry is correctly created for Client_tbl and ClientDetail_tbl.
   */
  clientController.insert();
  QTest::qWait(50);
  // Check GUI state
  QVERIFY(w.fld_FirstName->isEnabled());
  QVERIFY(w.fld_Remarks->isEnabled());
  QVERIFY(w.fld_Detail->isEnabled());
  QVERIFY(w.fld_FirstName->text().isEmpty());
  QVERIFY(w.fld_Remarks->text().isEmpty());
  QVERIFY(w.fld_Detail->text().isEmpty());
  // Edit new record
  QTest::keyClicks(w.fld_FirstName, "Laura");
  QTest::keyClicks(w.fld_Remarks, "Remark on Laura");
  QTest::keyClicks(w.fld_Detail, "Detail on Laura");
  // Submit and check
  qDebug() << "TEST - submit ...";
  QVERIFY(clientController.submitAndWait());
  qDebug() << "TEST - submit DONE";
  clientId = clientController.currentData("Id_PK");
  QVERIFY(!clientId.isNull());
  QCOMPARE(detailController->currentData("Client_Id_FK_PK"), clientId);
  // Check GUI state
  QVERIFY(w.fld_FirstName->isEnabled());
  QVERIFY(w.fld_Remarks->isEnabled());
  QVERIFY(w.fld_Detail->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Laura"));
  QCOMPARE(w.fld_Remarks->text(), QString("Remark on Laura"));
  QCOMPARE(w.fld_Detail->text(), QString("Detail on Laura"));
  /*
   * Select, check navigation and that data where really stored to database
   */
  QVERIFY(clientController.select());
  // Go to first and check
  clientController.toFirst();
  QTest::qWait(50);
  QCOMPARE(clientController.currentData("FirstName"), QVariant("Andy"));
  QVERIFY(w.fld_FirstName->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Andy"));
  QVERIFY(detailController->currentData("Detail").isNull());
  QVERIFY(!w.fld_Detail->isEnabled());
  QVERIFY(w.fld_Detail->text().isEmpty());
  // Go to last and check
  clientController.toLast();
  QTest::qWait(50);
  QCOMPARE(clientController.currentData("FirstName"), QVariant("Laura"));
  QVERIFY(w.fld_FirstName->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Laura"));
  QCOMPARE(detailController->currentData("Detail"), QVariant("Detail on Laura"));
  QVERIFY(w.fld_Detail->isEnabled());
  QCOMPARE(w.fld_Detail->text(), QString("Detail on Laura"));
  /*
   * Check edition + submit
   */
  // Edit and check that widgets displays correctly ...
  QTest::keyClicks(w.fld_FirstName, " (edited)");
  QCOMPARE(w.fld_FirstName->text(), QString("Laura (edited)"));
  QTest::keyClicks(w.fld_Detail, " (edited detail)");
  QCOMPARE(w.fld_Detail->text(), QString("Detail on Laura (edited detail)"));
  // Submit and check
  QVERIFY(clientController.submitAndWait());
  clientId = clientController.currentData("Id_PK");
  QVERIFY(!clientId.isNull());
  QCOMPARE(detailController->currentData("Client_Id_FK_PK"), clientId);
  QCOMPARE(clientController.currentData("FirstName"), QVariant("Laura (edited)"));
  QCOMPARE(detailController->currentData("Detail"), QVariant("Detail on Laura (edited detail)"));
  // Check GUI state
  QVERIFY(w.fld_FirstName->isEnabled());
  QVERIFY(w.fld_Detail->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Laura (edited)"));
  QCOMPARE(w.fld_Detail->text(), QString("Detail on Laura (edited detail)"));
  // Go back
  clientController.toPrevious();
  QTest::qWait(50);
  QCOMPARE(clientController.currentData("FirstName"), QVariant("Charly"));
  QVERIFY(w.fld_FirstName->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Charly"));
  QVERIFY(detailController->currentData("Detail").isNull());
  QVERIFY(!w.fld_Detail->isEnabled());
  QVERIFY(w.fld_Detail->text().isEmpty());
  // Come back to Laura and check
  clientController.toNext();
  QTest::qWait(50);
  QCOMPARE(clientController.currentData("FirstName"), QVariant("Laura (edited)"));
  QVERIFY(w.fld_FirstName->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Laura (edited)"));
  QCOMPARE(detailController->currentData("Detail"), QVariant("Detail on Laura (edited detail)"));
  QVERIFY(w.fld_Detail->isEnabled());
  QCOMPARE(w.fld_Detail->text(), QString("Detail on Laura (edited detail)"));
  /*
   * Check edition by editing only fld_Detail
   * Check that edition buttons are updated + that submit works properly
   */
  // Check edition buttons states
  QVERIFY(!w.submitEnabled);
  QVERIFY(!w.revertEnabled);
  QVERIFY(w.insertEnabled);
  QVERIFY(w.removeEnabled);
  // Edit and check that widgets displays correctly ...
  QTest::keyClicks(w.fld_Detail, " 2");
  QCOMPARE(w.fld_Detail->text(), QString("Detail on Laura (edited detail) 2"));
  // Check edition buttons states
  QVERIFY(w.submitEnabled);
  QVERIFY(w.revertEnabled);
  QVERIFY(!w.insertEnabled);
  QVERIFY(!w.removeEnabled);
  // Submit and check
  QVERIFY(clientController.submitAndWait());
  clientId = clientController.currentData("Id_PK");
  QVERIFY(!clientId.isNull());
  QCOMPARE(detailController->currentData("Client_Id_FK_PK"), clientId);
  QCOMPARE(clientController.currentData("FirstName"), QVariant("Laura (edited)"));
  QCOMPARE(detailController->currentData("Detail"), QVariant("Detail on Laura (edited detail) 2"));
  // Check GUI state
  QVERIFY(w.fld_FirstName->isEnabled());
  QVERIFY(w.fld_Detail->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Laura (edited)"));
  QCOMPARE(w.fld_Detail->text(), QString("Detail on Laura (edited detail) 2"));
  // Check edition buttons states
  QVERIFY(!w.submitEnabled);
  QVERIFY(!w.revertEnabled);
  QVERIFY(w.insertEnabled);
  QVERIFY(w.removeEnabled);
  // Go back
  clientController.toPrevious();
  QTest::qWait(50);
  QCOMPARE(clientController.currentData("FirstName"), QVariant("Charly"));
  QVERIFY(w.fld_FirstName->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Charly"));
  QVERIFY(detailController->currentData("Detail").isNull());
  QVERIFY(!w.fld_Detail->isEnabled());
  QVERIFY(w.fld_Detail->text().isEmpty());
  // Come back to Laura and check
  clientController.toNext();
  QTest::qWait(50);
  QCOMPARE(clientController.currentData("FirstName"), QVariant("Laura (edited)"));
  QVERIFY(w.fld_FirstName->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Laura (edited)"));
  QCOMPARE(detailController->currentData("Detail"), QVariant("Detail on Laura (edited detail) 2"));
  QVERIFY(w.fld_Detail->isEnabled());
  QCOMPARE(w.fld_Detail->text(), QString("Detail on Laura (edited detail) 2"));
  /*
   * Check insertion + revert
   */
  clientController.insert();
  QTest::qWait(50);
  // Check row counts
  QCOMPARE(clientController.rowCount(false), 6);
  QCOMPARE(detailController->rowCount(false), 1);
  // Check GUI state
  QVERIFY(w.fld_FirstName->isEnabled());
  QVERIFY(w.fld_Detail->isEnabled());
  QVERIFY(w.fld_FirstName->text().isEmpty());
  QVERIFY(w.fld_Detail->text().isEmpty());
  // Edit new record
  QTest::keyClicks(w.fld_FirstName, "New client 1");
  QTest::keyClicks(w.fld_Detail, "Detail on New client 1");
  // Revert and check
  clientController.revert();
  QTest::qWait(50);
  // Check row counts
  QCOMPARE(clientController.rowCount(false), 5);
  QCOMPARE(detailController->rowCount(false), 1);
  // Check that we are back to Laura
  QCOMPARE(clientController.currentData("FirstName"), QVariant("Laura (edited)"));
  QVERIFY(w.fld_FirstName->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Laura (edited)"));
  QCOMPARE(detailController->currentData("Detail"), QVariant("Detail on Laura (edited detail) 2"));
  QVERIFY(w.fld_Detail->isEnabled());
  QCOMPARE(w.fld_Detail->text(), QString("Detail on Laura (edited detail) 2"));
  /*
   * Check edition + revert
   */
  // Edit and check that widgets displays correctly ...
  QTest::keyClicks(w.fld_FirstName, " 2");
  QCOMPARE(w.fld_FirstName->text(), QString("Laura (edited) 2"));
  QTest::keyClicks(w.fld_Detail, ".2");
  QCOMPARE(w.fld_Detail->text(), QString("Detail on Laura (edited detail) 2.2"));
  // Revert and check
  clientController.revert();
  QTest::qWait(50);
  QCOMPARE(clientController.currentData("FirstName"), QVariant("Laura (edited)"));
  QVERIFY(w.fld_FirstName->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Laura (edited)"));
  QCOMPARE(detailController->currentData("Detail"), QVariant("Detail on Laura (edited detail) 2"));
  QVERIFY(w.fld_Detail->isEnabled());
  QCOMPARE(w.fld_Detail->text(), QString("Detail on Laura (edited detail) 2"));
  // Go back
  clientController.toPrevious();
  QTest::qWait(50);
  QCOMPARE(clientController.currentData("FirstName"), QVariant("Charly"));
  QVERIFY(w.fld_FirstName->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Charly"));
  QVERIFY(detailController->currentData("Detail").isNull());
  QVERIFY(!w.fld_Detail->isEnabled());
  QVERIFY(w.fld_Detail->text().isEmpty());
  // Come back to Laura and check
  clientController.toNext();
  QTest::qWait(50);
  QCOMPARE(clientController.currentData("FirstName"), QVariant("Laura (edited)"));
  QVERIFY(w.fld_FirstName->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Laura (edited)"));
  QCOMPARE(detailController->currentData("Detail"), QVariant("Detail on Laura (edited detail) 2"));
  QVERIFY(w.fld_Detail->isEnabled());
  QCOMPARE(w.fld_Detail->text(), QString("Detail on Laura (edited detail) 2"));
  /*
   * Check edition by editing only fld_Detail
   * Check that edition buttons are updated + that revert works properly
   */
  // Check edition buttons states
  QVERIFY(!w.submitEnabled);
  QVERIFY(!w.revertEnabled);
  QVERIFY(w.insertEnabled);
  QVERIFY(w.removeEnabled);
  // Edit and check that widgets displays correctly ...
  QTest::keyClicks(w.fld_Detail, " 3");
  QCOMPARE(w.fld_Detail->text(), QString("Detail on Laura (edited detail) 2 3"));
  // Check edition buttons states
  QVERIFY(w.submitEnabled);
  QVERIFY(w.revertEnabled);
  QVERIFY(!w.insertEnabled);
  QVERIFY(!w.removeEnabled);
  // Revert and check
  clientController.revert();
  QTest::qWait(50);
  clientId = clientController.currentData("Id_PK");
  QVERIFY(!clientId.isNull());
  QCOMPARE(detailController->currentData("Client_Id_FK_PK"), clientId);
  QCOMPARE(clientController.currentData("FirstName"), QVariant("Laura (edited)"));
  QCOMPARE(detailController->currentData("Detail"), QVariant("Detail on Laura (edited detail) 2"));
  // Check GUI state
  QVERIFY(w.fld_FirstName->isEnabled());
  QVERIFY(w.fld_Detail->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Laura (edited)"));
  QCOMPARE(w.fld_Detail->text(), QString("Detail on Laura (edited detail) 2"));
  // Check edition buttons states
  QVERIFY(!w.submitEnabled);
  QVERIFY(!w.revertEnabled);
  QVERIFY(w.insertEnabled);
  QVERIFY(w.removeEnabled);
  // Go back
  clientController.toPrevious();
  QTest::qWait(50);
  QCOMPARE(clientController.currentData("FirstName"), QVariant("Charly"));
  QVERIFY(w.fld_FirstName->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Charly"));
  QVERIFY(detailController->currentData("Detail").isNull());
  QVERIFY(!w.fld_Detail->isEnabled());
  QVERIFY(w.fld_Detail->text().isEmpty());
  // Come back to Laura and check
  clientController.toNext();
  QTest::qWait(50);
  QCOMPARE(clientController.currentData("FirstName"), QVariant("Laura (edited)"));
  QVERIFY(w.fld_FirstName->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Laura (edited)"));
  QCOMPARE(detailController->currentData("Detail"), QVariant("Detail on Laura (edited detail) 2"));
  QVERIFY(w.fld_Detail->isEnabled());
  QCOMPARE(w.fld_Detail->text(), QString("Detail on Laura (edited detail) 2"));
  /*
   * Check remove
   */
  QCOMPARE(clientController.rowCount(false), 5);
  QVERIFY(clientController.removeAndWait());
  QCOMPARE(clientController.rowCount(false), 4);
  // Check that we are back to Charly
  QCOMPARE(clientController.currentData("FirstName"), QVariant("Charly"));
  QVERIFY(w.fld_FirstName->isEnabled());
  QCOMPARE(w.fld_FirstName->text(), QString("Charly"));
  QVERIFY(detailController->currentData("Detail").isNull());
  QVERIFY(!w.fld_Detail->isEnabled());
  QVERIFY(w.fld_Detail->text().isEmpty());


  
  /*
   * Check edition buttons
   */

  
  /*
   * Play
   */
  /*
  while(w.isVisible()){
    QTest::qWait(500);
  }
  */
  // Re-enable foreign_keys support
  QVERIFY(q.exec("PRAGMA foreign_keys = ON"));
}

void mdtDatabaseWidgetTest::sqlTableViewControllerTest()
{
  mdtDatabaseWidgetTestScenario1Data scenario1(pvDatabase);
  mdtTableViewControllerTestObject testObject;
  mdtSqlTableViewController tvc;
  QTableView tv;
  QSqlQuery q(pvDatabase);
  QModelIndex index;
  QLineEdit *lineEdit;
  mdtSqlTableSelection s;
  QStringList fields;
  QList<QVariant> dataList, expectedDataList;
  QStringList strList;

  // For this test, we wont foreign_keys support
  QVERIFY(q.exec("PRAGMA foreign_keys = OFF"));
  // Create test data
  QVERIFY(scenario1.populate());
  /*
   * Setup
   */
  tvc.setTableView(&tv);
  tvc.setTableName("Client_tbl", pvDatabase, "Clients");
  QCOMPARE(tvc.tableName(), QString("Client_tbl"));
  connect(&tvc, SIGNAL(doubleClicked(const QSqlRecord&)), &testObject, SLOT(doubleClickedReceiver(const QSqlRecord&)));
  tv.setEditTriggers(QAbstractItemView::EditKeyPressed);
  tv.resize(400, 300);
  tv.show();
  tvc.start();
  /*
   * Select and check
   */
  QVERIFY(tvc.select());
  QVERIFY(tv.model() != 0);
  // Check that table view displays correct data - Row 0
  index = tv.model()->index(0, 1);
  QCOMPARE(tv.model()->data(index), QVariant("Andy"));
  index = tv.model()->index(0, 2);
  QCOMPARE(tv.model()->data(index), QVariant(""));
  // Check that data() works - Row 0
  QCOMPARE(tvc.data(0, "FirstName"), QVariant("Andy"));
  QCOMPARE(tvc.data(0, "Remarks"), QVariant(""));
  // Check that table view displays correct data - Row 1
  index = tv.model()->index(1, 1);
  QCOMPARE(tv.model()->data(index), QVariant("Bety"));
  index = tv.model()->index(1, 2);
  QCOMPARE(tv.model()->data(index), QVariant("Remark on Bety"));
  // Check that data() works - Row 1
  QCOMPARE(tvc.data(1, "FirstName"), QVariant("Bety"));
  QCOMPARE(tvc.data(1, "Remarks"), QVariant("Remark on Bety"));
  // Check that table view displays correct data - Row 2
  index = tv.model()->index(2, 1);
  QCOMPARE(tv.model()->data(index), QVariant("Zeta"));
  index = tv.model()->index(2, 2);
  QCOMPARE(tv.model()->data(index), QVariant("Remark on Zeta"));
  // Check that data() works - Row 2
  QCOMPARE(tvc.data(2, "FirstName"), QVariant("Zeta"));
  QCOMPARE(tvc.data(2, "Remarks"), QVariant("Remark on Zeta"));
  // Check that table view displays correct data - Row 3
  index = tv.model()->index(3, 1);
  QCOMPARE(tv.model()->data(index), QVariant("Charly"));
  index = tv.model()->index(3, 2);
  QCOMPARE(tv.model()->data(index), QVariant("Remark on Charly"));
  // Check that data() works - Row 3
  QCOMPARE(tvc.data(3, "FirstName"), QVariant("Charly"));
  QCOMPARE(tvc.data(3, "Remarks"), QVariant("Remark on Charly"));
  // Check that currentData() works
  index = tv.model()->index(0, 0);
  tv.setCurrentIndex(index);
  QCOMPARE(tvc.currentData("FirstName"), QVariant("Andy"));
  QCOMPARE(tvc.currentData("Remarks"), QVariant(""));
  index = tv.model()->index(1, 0);
  tv.setCurrentIndex(index);
  QCOMPARE(tvc.currentData("FirstName"), QVariant("Bety"));
  QCOMPARE(tvc.currentData("Remarks"), QVariant("Remark on Bety"));
  index = tv.model()->index(2, 0);
  tv.setCurrentIndex(index);
  QCOMPARE(tvc.currentData("FirstName"), QVariant("Zeta"));
  QCOMPARE(tvc.currentData("Remarks"), QVariant("Remark on Zeta"));
  index = tv.model()->index(3, 0);
  tv.setCurrentIndex(index);
  QCOMPARE(tvc.currentData("FirstName"), QVariant("Charly"));
  QCOMPARE(tvc.currentData("Remarks"), QVariant("Remark on Charly"));
  /*
   * Sort data and check
   */
  tvc.addColumnToSortOrder("FirstName", Qt::AscendingOrder);
  tvc.sort();
  // Check that table view displays correct data - Row 0
  index = tv.model()->index(0, 1);
  QCOMPARE(tv.model()->data(index), QVariant("Andy"));
  index = tv.model()->index(0, 2);
  QCOMPARE(tv.model()->data(index), QVariant(""));
  // Check that data() works - Row 0
  QCOMPARE(tvc.data(0, "FirstName"), QVariant("Andy"));
  QCOMPARE(tvc.data(0, "Remarks"), QVariant(""));
  // Check that table view displays correct data - Row 1
  index = tv.model()->index(1, 1);
  QCOMPARE(tv.model()->data(index), QVariant("Bety"));
  index = tv.model()->index(1, 2);
  QCOMPARE(tv.model()->data(index), QVariant("Remark on Bety"));
  // Check that data() works - Row 1
  QCOMPARE(tvc.data(1, "FirstName"), QVariant("Bety"));
  QCOMPARE(tvc.data(1, "Remarks"), QVariant("Remark on Bety"));
  // Check that table view displays correct data - Row 2
  index = tv.model()->index(2, 1);
  QCOMPARE(tv.model()->data(index), QVariant("Charly"));
  index = tv.model()->index(2, 2);
  QCOMPARE(tv.model()->data(index), QVariant("Remark on Charly"));
  // Check that data() works - Row 2
  QCOMPARE(tvc.data(2, "FirstName"), QVariant("Charly"));
  QCOMPARE(tvc.data(2, "Remarks"), QVariant("Remark on Charly"));
  // Check that table view displays correct data - Row 3
  index = tv.model()->index(3, 1);
  QCOMPARE(tv.model()->data(index), QVariant("Zeta"));
  index = tv.model()->index(3, 2);
  QCOMPARE(tv.model()->data(index), QVariant("Remark on Zeta"));
  // Check that data() works - Row 3
  QCOMPARE(tvc.data(3, "FirstName"), QVariant("Zeta"));
  QCOMPARE(tvc.data(3, "Remarks"), QVariant("Remark on Zeta"));
  // Check that currentData() works
  index = tv.model()->index(0, 0);
  tv.setCurrentIndex(index);
  QCOMPARE(tvc.currentData("FirstName"), QVariant("Andy"));
  QCOMPARE(tvc.currentData("Remarks"), QVariant(""));
  index = tv.model()->index(1, 0);
  tv.setCurrentIndex(index);
  QCOMPARE(tvc.currentData("FirstName"), QVariant("Bety"));
  QCOMPARE(tvc.currentData("Remarks"), QVariant("Remark on Bety"));
  index = tv.model()->index(2, 0);
  tv.setCurrentIndex(index);
  QCOMPARE(tvc.currentData("FirstName"), QVariant("Charly"));
  QCOMPARE(tvc.currentData("Remarks"), QVariant("Remark on Charly"));
  index = tv.model()->index(3, 0);
  tv.setCurrentIndex(index);
  QCOMPARE(tvc.currentData("FirstName"), QVariant("Zeta"));
  QCOMPARE(tvc.currentData("Remarks"), QVariant("Remark on Zeta"));
  /*
   * Check edition (by user) and submit
   */
  QVERIFY(tvc.currentState() == mdtAbstractSqlTableController::Visualizing);
  // Begin edition and get edition widget
  index = tv.model()->index(0, 2);
  tv.edit(index);
  lineEdit = qobject_cast<QLineEdit*>(tv.indexWidget(index));
  QVERIFY(lineEdit != 0);
  QTest::keyClicks(lineEdit, "Edited remark on Andy");
  QVERIFY(tvc.currentState() == mdtAbstractSqlTableController::Editing);
  QTest::keyClick(lineEdit, Qt::Key_Enter);
  QTest::qWait(50);
  // Submit
  QVERIFY(!tvc.allDataAreSaved());
  QVERIFY(tvc.submitAndWait());
  QVERIFY(tvc.currentState() == mdtAbstractSqlTableController::Visualizing);
  QVERIFY(tvc.allDataAreSaved());
  // Select and check that database was updated
  QVERIFY(tvc.select());
  QCOMPARE(tvc.data(0, "FirstName"), QVariant("Andy"));
  QCOMPARE(tvc.data(0, "Remarks"), QVariant("Edited remark on Andy"));
  /*
   * Check edition (by user) and revert
   */
  QCOMPARE(tvc.rowCount(), 4);
  QVERIFY(tvc.currentState() == mdtAbstractSqlTableController::Visualizing);
  // Begin edition and get edition widget
  index = tv.model()->index(0, 2);
  tv.edit(index);
  lineEdit = qobject_cast<QLineEdit*>(tv.indexWidget(index));
  QVERIFY(lineEdit != 0);
  QTest::keyClicks(lineEdit, "Edited remark 2 on Andy");
  QTest::keyClick(lineEdit, Qt::Key_Enter);
  QTest::qWait(50);
  tvc.revert();
  QTest::qWait(50);
  // Select and check that database was not updated
  QVERIFY(tvc.select());
  QCOMPARE(tvc.data(0, "FirstName"), QVariant("Andy"));
  QCOMPARE(tvc.data(0, "Remarks"), QVariant("Edited remark on Andy"));
  /*
   * Check insertion (by user) and submit
   */
  
  qDebug() << "TEST - check insertion + submit ...";
  tvc.setDefaultColumnToSelect("FirstName");
  QCOMPARE(tvc.rowCount(), 4);
  tvc.insert();   // Will also call edit itself
  QTest::qWait(50);
  QCOMPARE(tvc.rowCount(false), 5);
  // Get edition widget
  index = tv.model()->index(4, 1);
  ///tv.edit(index);
  lineEdit = qobject_cast<QLineEdit*>(tv.indexWidget(index));
  QVERIFY(lineEdit != 0);
  QTest::keyClicks(lineEdit, "New name 1");
  QTest::keyClick(lineEdit, Qt::Key_Enter);
  QTest::qWait(50);
  // Check that data was set to model
  QCOMPARE(tvc.currentData("FirstName"), QVariant("New name 1"));
  QCOMPARE(tvc.currentData("Remarks"), QVariant(""));
  // Submit
  QVERIFY(!tvc.allDataAreSaved());
  QVERIFY(tvc.submitAndWait());
  QVERIFY(tvc.currentState() == mdtAbstractSqlTableController::Visualizing);
  QVERIFY(tvc.allDataAreSaved());
  // Select and check that database was updated
  /*
   * Select and check
   * Note: because of sorting, we find New name 1 at row 3 after select()
   */
  QVERIFY(tvc.select());
  QCOMPARE(tvc.rowCount(), 5);
  QCOMPARE(tvc.data(3, "FirstName"), QVariant("New name 1"));
  QCOMPARE(tvc.data(3, "Remarks"), QVariant(""));
  /*
   * Check insertion (by user) and revert
   */
  
  qDebug() << "TEST - check insertion + revert ...";
  tvc.setDefaultColumnToSelect("Remarks");
  QCOMPARE(tvc.rowCount(), 5);
  tvc.insert();
  QTest::qWait(50);
  QCOMPARE(tvc.rowCount(false), 6);
  ///index = tv.model()->index(tvc.currentRow(), 2);
  // Get edition widget
  index = tv.model()->index(5, 2);
  ///tv.edit(index);
  lineEdit = qobject_cast<QLineEdit*>(tv.indexWidget(index));
  QVERIFY(lineEdit != 0);
  QTest::keyClicks(lineEdit, "New remark 1");
  QTest::keyClick(lineEdit, Qt::Key_Enter);
  QTest::qWait(50);
  tvc.revert();
  QTest::qWait(50);
  // Select and check that database was NOT updated
  QVERIFY(tvc.select());
  QCOMPARE(tvc.rowCount(), 5);
  QCOMPARE(tvc.data(3, "FirstName"), QVariant("New name 1"));
  QCOMPARE(tvc.data(3, "Remarks"), QVariant(""));
  /*
   * Check delete
   */
  QCOMPARE(tvc.rowCount(), 5);
  tv.selectRow(3);
  tvc.remove();
  QTest::qWait(1000); // Writing in DB can be very slow, f.ex. with Sqlite on HDD
  QCOMPARE(tvc.rowCount(), 4);
  // Select and check that row was removed
  tvc.addColumnToSortOrder("FirstName", Qt::AscendingOrder);
  QVERIFY(tvc.select());
  tvc.sort();
  QCOMPARE(tvc.rowCount(), 4);
  index = tv.model()->index(0, 0);
  tv.setCurrentIndex(index);
  QCOMPARE(tvc.currentData("FirstName"), QVariant("Andy"));
  QCOMPARE(tvc.currentData("Remarks"), QVariant("Edited remark on Andy"));
  index = tv.model()->index(1, 0);
  tv.setCurrentIndex(index);
  QCOMPARE(tvc.currentData("FirstName"), QVariant("Bety"));
  QCOMPARE(tvc.currentData("Remarks"), QVariant("Remark on Bety"));
  index = tv.model()->index(2, 0);
  tv.setCurrentIndex(index);
  QCOMPARE(tvc.currentData("FirstName"), QVariant("Charly"));
  QCOMPARE(tvc.currentData("Remarks"), QVariant("Remark on Charly"));
  index = tv.model()->index(3, 0);
  tv.setCurrentIndex(index);
  QCOMPARE(tvc.currentData("FirstName"), QVariant("Zeta"));
  QCOMPARE(tvc.currentData("Remarks"), QVariant("Remark on Zeta"));
  /*
   * Check programmed edition:
   *  - Edit many rows
   *  - Submit all at once
   */
  QVERIFY(tvc.currentState() == mdtAbstractSqlTableController::Visualizing);
  // Edit each row
  QVERIFY(tvc.setData(0, "Remarks", "Edited remark 2 on Andy", false));
  QVERIFY(tvc.currentState() == mdtAbstractSqlTableController::Editing);
  QVERIFY(tvc.setData(1, "Remarks", "Edited remark on Bety", false));
  QVERIFY(tvc.setData(2, "Remarks", "Edited remark on Charly", false));
  QVERIFY(tvc.setData(3, "Remarks", "Edited remark on Zeta", false));
  // Submit all rows
  qDebug() << "TEST - submitting ...";
  QVERIFY(tvc.currentState() == mdtAbstractSqlTableController::Editing);
  QVERIFY(tvc.submitAndWait());
  QVERIFY(tvc.currentState() == mdtAbstractSqlTableController::Visualizing);
  
  qDebug() << "TEST - submitting DONE";
  
  // Check that database was updated
  QVERIFY(tvc.select());
  tvc.sort();
  QCOMPARE(tvc.data(0, "FirstName"), QVariant("Andy"));
  QCOMPARE(tvc.data(0, "Remarks"), QVariant("Edited remark 2 on Andy"));
  QCOMPARE(tvc.data(1, "FirstName"), QVariant("Bety"));
  QCOMPARE(tvc.data(1, "Remarks"), QVariant("Edited remark on Bety"));
  QCOMPARE(tvc.data(2, "FirstName"), QVariant("Charly"));
  QCOMPARE(tvc.data(2, "Remarks"), QVariant("Edited remark on Charly"));
  QCOMPARE(tvc.data(3, "FirstName"), QVariant("Zeta"));
  QCOMPARE(tvc.data(3, "Remarks"), QVariant("Edited remark on Zeta"));
  /*
   * Prepare for selection tests
   */
  tvc.clearColumnsSortOrder();
  tvc.addColumnToSortOrder("Id_PK", Qt::AscendingOrder);
  QVERIFY(tvc.select());
  tvc.sort();
  QCOMPARE(tvc.data(0, "Id_PK"), QVariant(1));
  QCOMPARE(tvc.data(0, "FirstName"), QVariant("Andy"));
  QCOMPARE(tvc.data(1, "Id_PK"), QVariant(2));
  QCOMPARE(tvc.data(1, "FirstName"), QVariant("Bety"));
  QCOMPARE(tvc.data(2, "Id_PK"), QVariant(3));
  QCOMPARE(tvc.data(2, "FirstName"), QVariant("Zeta"));
  QCOMPARE(tvc.data(3, "Id_PK"), QVariant(4));
  QCOMPARE(tvc.data(3, "FirstName"), QVariant("Charly"));
  /*
   * Check selection: select row 1
   */
  tv.clearSelection();
  tv.selectRow(1);
  s = tvc.currentSelection("Id_PK");
  QCOMPARE(s.rowCount(), 1);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(2));
  // Check current data
  QCOMPARE(tvc.currentData("Id_PK"), QVariant(2));
  /*
   * Check selection: select row 0
   */
  tv.clearSelection();
  tv.selectRow(0);
  s = tvc.currentSelection("Id_PK");
  QCOMPARE(s.rowCount(), 1);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(1));
  // Check current data
  QCOMPARE(tvc.currentData("Id_PK"), QVariant(1));
  /*
   * Check selection: select row 2
   */
  tv.clearSelection();
  tv.selectRow(2);
  s = tvc.currentSelection("Id_PK");
  QCOMPARE(s.rowCount(), 1);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(3));
  // Check current data
  QCOMPARE(tvc.currentData("Id_PK"), QVariant(3));
  /*
   * Check selection: select row 3
   */
  tv.clearSelection();
  tv.selectRow(3);
  s = tvc.currentSelection("Id_PK");
  QCOMPARE(s.rowCount(), 1);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(4));
  // Check current data
  QCOMPARE(tvc.currentData("Id_PK"), QVariant(4));
  /*
   * Prepare for selection tests - Sort by FirstName
   */
  tv.setSelectionMode(QAbstractItemView::MultiSelection);
  tvc.clearColumnsSortOrder();
  tvc.addColumnToSortOrder("FirstName", Qt::AscendingOrder);
  QVERIFY(tvc.select());
  tvc.sort();
  QCOMPARE(tvc.data(0, "Id_PK"), QVariant(1));
  QCOMPARE(tvc.data(0, "FirstName"), QVariant("Andy"));
  QCOMPARE(tvc.data(1, "Id_PK"), QVariant(2));
  QCOMPARE(tvc.data(1, "FirstName"), QVariant("Bety"));
  QCOMPARE(tvc.data(2, "Id_PK"), QVariant(4));
  QCOMPARE(tvc.data(2, "FirstName"), QVariant("Charly"));
  QCOMPARE(tvc.data(3, "Id_PK"), QVariant(3));
  QCOMPARE(tvc.data(3, "FirstName"), QVariant("Zeta"));
  /*
   * Check selection: select row 0
   */
  tv.clearSelection();
  tv.selectRow(0);
  s = tvc.currentSelection("Id_PK");
  QCOMPARE(s.rowCount(), 1);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(1));
  // Check current data
  QCOMPARE(tvc.currentData("Id_PK"), QVariant(1));
  /*
   * Check selection: select row 1
   */
  tv.clearSelection();
  tv.selectRow(1);
  s = tvc.currentSelection("Id_PK");
  QCOMPARE(s.rowCount(), 1);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(2));
  // Check current data
  QCOMPARE(tvc.currentData("Id_PK"), QVariant(2));
  /*
   * Check selection: select row 2
   */
  tv.clearSelection();
  tv.selectRow(2);
  s = tvc.currentSelection("Id_PK");
  QCOMPARE(s.rowCount(), 1);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(4));
  // Check current data
  QCOMPARE(tvc.currentData("Id_PK"), QVariant(4));
  /*
   * Check selection: select row 3
   */
  tv.clearSelection();
  tv.selectRow(3);
  s = tvc.currentSelection("Id_PK");
  QCOMPARE(s.rowCount(), 1);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(3));
  // Check current data
  QCOMPARE(tvc.currentData("Id_PK"), QVariant(3));
  /*
   * Check multiple row selection with ASC sorting
   *  -> We must have only selected items,
   *     but sorting must be keeped
   */
  // Select row 0 (Andy, Id_PK 1) and row 1 (Bety, Id_PK 2) in view
  tv.clearSelection();
  tv.selectRow(0);
  tv.selectRow(1);
  s = tvc.currentSelection("Id_PK");
  QCOMPARE(s.rowCount(), 2);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(1));
  QCOMPARE(s.data(1, "Id_PK"), QVariant(2));
  // Select row 2 (Charly, Id_PK 4) and row 3 (Zeta, Id_PK 3) in view
  tv.clearSelection();
  tv.selectRow(2);
  tv.selectRow(3);
  s = tvc.currentSelection("Id_PK");
  QCOMPARE(s.rowCount(), 2);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(4));
  QCOMPARE(s.data(1, "Id_PK"), QVariant(3));
  // Select row 1 (Bety, Id_PK 2) and row 2 (Charly, Id_PK 4) in view
  tv.clearSelection();
  tv.selectRow(1);
  tv.selectRow(2);
  s = tvc.currentSelection("Id_PK");
  QCOMPARE(s.rowCount(), 2);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(2));
  QCOMPARE(s.data(1, "Id_PK"), QVariant(4));
  // Select row 0 (Andy, Id_PK 1), row 1 (Bety, Id_PK 2) and row 2 (Charly, Id_PK 4) in view
  tv.clearSelection();
  tv.selectRow(0);
  tv.selectRow(1);
  tv.selectRow(2);
  s = tvc.currentSelection("Id_PK");
  QCOMPARE(s.rowCount(), 3);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(1));
  QCOMPARE(s.data(1, "Id_PK"), QVariant(2));
  QCOMPARE(s.data(2, "Id_PK"), QVariant(4));
  // Same as before, bust select rows in reverse order
  tv.clearSelection();
  tv.selectRow(2);
  tv.selectRow(1);
  tv.selectRow(0);
  s = tvc.currentSelection("Id_PK");
  QCOMPARE(s.rowCount(), 3);
  QCOMPARE(s.data(0, "Id_PK"), QVariant(1));
  QCOMPARE(s.data(1, "Id_PK"), QVariant(2));
  QCOMPARE(s.data(2, "Id_PK"), QVariant(4));
  /*
   * Check row data getters
   */
  fields.clear();
  fields << "FirstName" << "Remarks" << "SomeValueDouble";
  // Check header getter
  expectedDataList.clear();
  expectedDataList << "FirstName" << "Remarks" << "SomeValueDouble";
  dataList = tvc.headerRowData(fields);
  QCOMPARE(dataList, expectedDataList);
  strList = tvc.headerRowDataStr(fields);
  QCOMPARE(strList.size(), 3);
  QCOMPARE(strList.at(0), QString("FirstName"));
  QCOMPARE(strList.at(1), QString("Remarks"));
  QCOMPARE(strList.at(2), QString("SomeValueDouble"));
  /* Check QVariant row data getter */
  // Row 0
  dataList = tvc.rowData(0, fields);
  QCOMPARE(dataList.size(), 3);
  QCOMPARE(dataList.at(0), QVariant("Andy"));
  QCOMPARE(dataList.at(1), QVariant("Edited remark 2 on Andy"));
  QVERIFY(dataList.at(2).isNull());
  // Row 1
  dataList = tvc.rowData(1, fields);
  QCOMPARE(dataList.size(), 3);
  QCOMPARE(dataList.at(0), QVariant("Bety"));
  QCOMPARE(dataList.at(1), QVariant("Edited remark on Bety"));
  QCOMPARE(dataList.at(2), QVariant(2.0));
  // Row 2
  dataList = tvc.rowData(2, fields);
  QCOMPARE(dataList.size(), 3);
  QCOMPARE(dataList.at(0), QVariant("Charly"));
  QCOMPARE(dataList.at(1), QVariant("Edited remark on Charly"));
  QVERIFY(dataList.at(2).isNull());
  // Row 3
  dataList = tvc.rowData(3, fields);
  QCOMPARE(dataList.size(), 3);
  QCOMPARE(dataList.at(0), QVariant("Zeta"));
  QCOMPARE(dataList.at(1), QVariant("Edited remark on Zeta"));
  QVERIFY(dataList.at(2).isNull());
  /* Check QString row data getter */
  // Row 0
  strList = tvc.rowDataStr(0, fields);
  QCOMPARE(strList.size(), 3);
  QCOMPARE(strList.at(0), QString("Andy"));
  QCOMPARE(strList.at(1), QString("Edited remark 2 on Andy"));
  QCOMPARE(strList.at(2), QString(""));
  // Row 1
  strList = tvc.rowDataStr(1, fields);
  QCOMPARE(strList.size(), 3);
  QCOMPARE(strList.at(0), QString("Bety"));
  QCOMPARE(strList.at(1), QString("Edited remark on Bety"));
  QCOMPARE(strList.at(2), QString("2"));
  // Row 2
  strList = tvc.rowDataStr(2, fields);
  QCOMPARE(strList.size(), 3);
  QCOMPARE(strList.at(0), QString("Charly"));
  QCOMPARE(strList.at(1), QString("Edited remark on Charly"));
  QCOMPARE(strList.at(2), QString(""));
  // Row 3
  strList = tvc.rowDataStr(3, fields);
  QCOMPARE(strList.size(), 3);
  QCOMPARE(strList.at(0), QString("Zeta"));
  QCOMPARE(strList.at(1), QString("Edited remark on Zeta"));
  QCOMPARE(strList.at(2), QString(""));
  /*
   * Check double click
   */
  // Row 0
  testObject.doubleClick(&tv, 0, 0);
  QCOMPARE(testObject.doubleClickReceiverRecord.value("FirstName"), QVariant("Andy"));
  testObject.doubleClick(&tv, 0, 1);
  QCOMPARE(testObject.doubleClickReceiverRecord.value("FirstName"), QVariant("Andy"));
  testObject.doubleClick(&tv, 0, 2);
  QCOMPARE(testObject.doubleClickReceiverRecord.value("FirstName"), QVariant("Andy"));
  testObject.doubleClick(&tv, 0, 3);
  QCOMPARE(testObject.doubleClickReceiverRecord.value("FirstName"), QVariant("Andy"));
  // Row 1
  testObject.doubleClick(&tv, 1, 0);
  QCOMPARE(testObject.doubleClickReceiverRecord.value("FirstName"), QVariant("Bety"));
  testObject.doubleClick(&tv, 1, 1);
  QCOMPARE(testObject.doubleClickReceiverRecord.value("FirstName"), QVariant("Bety"));
  testObject.doubleClick(&tv, 1, 2);
  QCOMPARE(testObject.doubleClickReceiverRecord.value("FirstName"), QVariant("Bety"));
  testObject.doubleClick(&tv, 1, 3);
  QCOMPARE(testObject.doubleClickReceiverRecord.value("FirstName"), QVariant("Bety"));
  // Row 2
  testObject.doubleClick(&tv, 2, 0);
  QCOMPARE(testObject.doubleClickReceiverRecord.value("FirstName"), QVariant("Charly"));
  testObject.doubleClick(&tv, 2, 1);
  QCOMPARE(testObject.doubleClickReceiverRecord.value("FirstName"), QVariant("Charly"));
  testObject.doubleClick(&tv, 2, 2);
  QCOMPARE(testObject.doubleClickReceiverRecord.value("FirstName"), QVariant("Charly"));
  testObject.doubleClick(&tv, 2, 3);
  QCOMPARE(testObject.doubleClickReceiverRecord.value("FirstName"), QVariant("Charly"));
  // Row 3
  testObject.doubleClick(&tv, 3, 0);
  QCOMPARE(testObject.doubleClickReceiverRecord.value("FirstName"), QVariant("Zeta"));
  testObject.doubleClick(&tv, 3, 1);
  QCOMPARE(testObject.doubleClickReceiverRecord.value("FirstName"), QVariant("Zeta"));
  testObject.doubleClick(&tv, 3, 2);
  QCOMPARE(testObject.doubleClickReceiverRecord.value("FirstName"), QVariant("Zeta"));
  testObject.doubleClick(&tv, 3, 3);
  QCOMPARE(testObject.doubleClickReceiverRecord.value("FirstName"), QVariant("Zeta"));

  /*
   * Play
   */
  /*
  while(tv.isVisible()){
    QTest::qWait(500);
  }
  */

  // Re-enable foreign_keys support
  QVERIFY(q.exec("PRAGMA foreign_keys = ON"));
}

// void mdtDatabaseWidgetTest::sqlTableViewControllerDataExportTest()
// {
//   mdtSqlTableViewController tvc;
//   QTableView tv;
//   QSqlQuery q(pvDatabase);
//   ///QModelIndex index;
//   ///QLineEdit *lineEdit;
//   ///mdtSqlTableSelection s;
// 
//   // For this test, we wont foreign_keys support
//   QVERIFY(q.exec("PRAGMA foreign_keys = OFF"));
//   // Create test data
//   populateTestDatabase();
//   /*
//    * Setup
//    */
//   tvc.setTableView(&tv);
//   tvc.setTableName("Client_tbl", pvDatabase, "Clients");
//   QCOMPARE(tvc.tableName(), QString("Client_tbl"));
//   tv.setEditTriggers(QAbstractItemView::EditKeyPressed);
//   tv.resize(400, 300);
//   tv.show();
//   tvc.start();
// 
//   /*
//    * Setup sorting and select
//    */
//   tvc.addColumnToSortOrder("FirstName", Qt::AscendingOrder);
//   tvc.sort();
//   QVERIFY(tvc.select());
// 
//   
//   /*
//    * Play
//    */
//   while(tv.isVisible()){
//     QTest::qWait(500);
//   }
// 
//   // Clear test data
//   clearTestDatabaseData();
//   // Re-enable foreign_keys support
//   QVERIFY(q.exec("PRAGMA foreign_keys = ON"));
// }

void mdtDatabaseWidgetTest::sqlControllerParentChildTest()
{
  mdtDatabaseWidgetTestScenario1Data scenario1(pvDatabase);
  sqlDataWidgetControllerTestWidget clientWidget;
  mdtSqlDataWidgetController clientController;
  QTableView addressView;
  std::shared_ptr<mdtSqlTableViewController> addressController;
  mdtSqlRelationInfo relationInfo;
  QModelIndex index;
  QLineEdit *lineEdit;
  QSpinBox *spinBox;

  // Create test data
  QVERIFY(scenario1.populate());
  /*
   * Setup
   */
  // Setup client widget and controller
  clientController.setTableName("Client_tbl", pvDatabase, "Clients");
  clientController.addColumnToSortOrder("FirstName", Qt::AscendingOrder);
  QVERIFY(clientController.mapFormWidgets(&clientWidget));
  connect(&clientController, SIGNAL(toFirstEnabledStateChanged(bool)), &clientWidget, SLOT(setToFirstEnableState(bool)));
  connect(&clientController, SIGNAL(toLastEnabledStateChanged(bool)), &clientWidget, SLOT(setToLastEnableState(bool)));
  connect(&clientController, SIGNAL(toNextEnabledStateChanged(bool)), &clientWidget, SLOT(setToNextEnableState(bool)));
  connect(&clientController, SIGNAL(toPreviousEnabledStateChanged(bool)), &clientWidget, SLOT(setToPreviousEnableState(bool)));
  connect(&clientController, SIGNAL(insertEnabledStateChanged(bool)), &clientWidget, SLOT(setInsertEnableState(bool)));
  connect(&clientController, SIGNAL(removeEnabledStateChanged(bool)), &clientWidget, SLOT(setRemoveEnableState(bool)));
  connect(&clientController, SIGNAL(submitEnabledStateChanged(bool)), &clientWidget, SLOT(setSubmitEnableState(bool)));
  connect(&clientController, SIGNAL(revertEnabledStateChanged(bool)), &clientWidget, SLOT(setRevertEnableState(bool)));
  clientWidget.show();
  // Setup address view and controller
  relationInfo.setChildTableName("Address_tbl");
  relationInfo.addRelation("Id_PK", "Client_Id_FK", true);
  QVERIFY(clientController.addChildController<mdtSqlTableViewController>(relationInfo, "Addresses"));
  addressController = clientController.childController<mdtSqlTableViewController>("Address_tbl");
  QVERIFY(addressController.get() != 0);
  addressController->setTableView(&addressView);
  addressController->addColumnToSortOrder("StreetName", Qt::AscendingOrder);
  connect(&clientController, SIGNAL(childWidgetEnableStateChanged(bool)), &addressView, SLOT(setEnabled(bool)));
  addressView.resize(400, 300);
  addressView.show();
  // Start
  clientController.start();
  QVERIFY(addressView.isEnabled());
  /*
   * Select and check
   */
  QVERIFY(clientController.select());
  clientController.sort();
  QCOMPARE(clientController.rowCount(), 4);
  QCOMPARE(clientController.currentRow(), 0);
  QCOMPARE(clientWidget.fld_FirstName->text(), QString("Andy"));
  QCOMPARE(clientWidget.fld_Remarks->text(), QString(""));
  QCOMPARE(addressController->rowCount(), 2);
  QCOMPARE(addressController->data(0, "StreetName"), QVariant("Andy street 1"));
  QCOMPARE(addressController->data(1, "StreetName"), QVariant("Andy street 2"));
  // Go to next and check
  clientController.toNext();
  QTest::qWait(50);
  QCOMPARE(clientWidget.fld_FirstName->text(), QString("Bety"));
  QCOMPARE(clientWidget.fld_Remarks->text(), QString("Remark on Bety"));
  QCOMPARE(addressController->rowCount(), 1);
  QCOMPARE(addressController->data(0, "StreetName"), QVariant("Bety street 1"));
  // Go to next and check
  clientController.toNext();
  QTest::qWait(50);
  QCOMPARE(clientWidget.fld_FirstName->text(), QString("Charly"));
  QCOMPARE(clientWidget.fld_Remarks->text(), QString("Remark on Charly"));
  QCOMPARE(addressController->rowCount(), 0);
  // Go to previous and check
  clientController.toPrevious();
  QTest::qWait(50);
  QCOMPARE(clientWidget.fld_FirstName->text(), QString("Bety"));
  QCOMPARE(clientWidget.fld_Remarks->text(), QString("Remark on Bety"));
  QCOMPARE(addressController->rowCount(), 1);
  QCOMPARE(addressController->data(0, "StreetName"), QVariant("Bety street 1"));
  // Go to last and check
  clientController.toLast();
  QTest::qWait(50);
  QCOMPARE(clientWidget.fld_FirstName->text(), QString("Zeta"));
  QCOMPARE(clientWidget.fld_Remarks->text(), QString("Remark on Zeta"));
  QCOMPARE(addressController->rowCount(), 0);
  // Go to first and check
  clientController.toFirst();
  QTest::qWait(50);
  QCOMPARE(clientWidget.fld_FirstName->text(), QString("Andy"));
  QCOMPARE(clientWidget.fld_Remarks->text(), QString(""));
  QCOMPARE(addressController->rowCount(), 2);
  QCOMPARE(addressController->data(0, "StreetName"), QVariant("Andy street 1"));
  QCOMPARE(addressController->data(1, "StreetName"), QVariant("Andy street 2"));
  // Go to Id_PK 4 (Charly) and check
  QVERIFY(clientController.setCurrentRow("Id_PK", 4));
  QCOMPARE(clientWidget.fld_FirstName->text(), QString("Charly"));
  QCOMPARE(clientWidget.fld_Remarks->text(), QString("Remark on Charly"));
  QCOMPARE(addressController->rowCount(), 0);
  /*
   * Check insertion in child table
   */
  addressController->setDefaultColumnToSelect("StreetName");
  addressController->insert();  // Will also call edit itself
  QTest::qWait(50);
  index = addressView.model()->index(0, 1);
  ///addressView.edit(index);
  lineEdit = qobject_cast<QLineEdit*>(addressView.indexWidget(index));
  QVERIFY(lineEdit != 0);
  QTest::keyClicks(lineEdit, "New Charly street 2");
  QTest::keyClick(lineEdit, Qt::Key_Enter);
  QTest::qWait(50);
  index = addressView.model()->index(0, 2);
  addressView.edit(index);
  spinBox = qobject_cast<QSpinBox*>(addressView.indexWidget(index));
  QVERIFY(lineEdit != 0);
  QTest::keyClicks(spinBox, "2");
  QTest::keyClick(spinBox, Qt::Key_Enter);
  QTest::qWait(50);
  QVERIFY(addressController->submitAndWait());
  QCOMPARE(addressController->data(0, "StreetName"), QVariant("New Charly street 2"));
  QCOMPARE(addressController->data(0, "StreetNumber"), QVariant(2));
  // Go to previous and check
  clientController.toPrevious();
  QTest::qWait(50);
  QCOMPARE(clientWidget.fld_FirstName->text(), QString("Bety"));
  QCOMPARE(clientWidget.fld_Remarks->text(), QString("Remark on Bety"));
  QCOMPARE(addressController->rowCount(), 1);
  QCOMPARE(addressController->data(0, "StreetName"), QVariant("Bety street 1"));
  // Go back to next and check
  clientController.toNext();
  QTest::qWait(50);
  QCOMPARE(clientWidget.fld_FirstName->text(), QString("Charly"));
  QCOMPARE(clientWidget.fld_Remarks->text(), QString("Remark on Charly"));
  QCOMPARE(addressController->rowCount(), 1);
  QCOMPARE(addressController->data(0, "StreetName"), QVariant("New Charly street 2"));
  QCOMPARE(addressController->data(0, "StreetNumber"), QVariant(2));
  /*
   * Check row changing on usaved data in child controller
   */
  index = addressView.model()->index(0, 1);
  qDebug() << "TEST - editing data in address table ...";
  addressView.edit(index);
  lineEdit = qobject_cast<QLineEdit*>(addressView.indexWidget(index));
  QVERIFY(lineEdit != 0);
  QTest::keyClicks(lineEdit, "Edited Charly street 2");
  QTest::qWait(50);
  QCOMPARE(clientController.currentRow(), 2);
  qDebug() << "TEST - Trying toPrevious() on unsaved data - current row: " << clientController.currentRow();
  clientController.toPrevious();
  QTest::qWait(50);
  QCOMPARE(clientController.currentRow(), 2);
  // Save modification and check
  QTest::keyClick(lineEdit, Qt::Key_Enter);
  QTest::qWait(50);
  QVERIFY(addressController->submitAndWait());
  QCOMPARE(addressController->rowCount(), 1);
  QCOMPARE(addressController->data(0, "StreetName"), QVariant("Edited Charly street 2"));
  QCOMPARE(addressController->data(0, "StreetNumber"), QVariant(2));
  /*
   * Check edition (by user) on in main table
   */
  QVERIFY(addressView.isEnabled());
  QTest::keyClicks(clientWidget.fld_FirstName, "1");
  QVERIFY(!addressView.isEnabled());
  clientController.revert();
  QTest::qWait(50);
  QVERIFY(addressView.isEnabled());
  /*
   * Check insertion in main table
   */
  QVERIFY(addressView.isEnabled());
  QCOMPARE(addressController->rowCount(), 1);
  clientController.insert();
  QTest::qWait(50);
  QVERIFY(!addressView.isEnabled());
  QCOMPARE(addressController->rowCount(), 0);
  // Revert and check
  clientController.revert();
  QTest::qWait(50);
  QCOMPARE(clientController.currentRow(), 2);
  QVERIFY(addressView.isEnabled());
  QCOMPARE(addressController->rowCount(), 1);

  /*
   * Play
   */
  /*
  while(clientWidget.isVisible()){
    QTest::qWait(500);
  }
  */
}


void mdtDatabaseWidgetTest::sqlTableSelectionItemTest()
{
  mdtDatabaseWidgetTestScenario1Data scenario1(pvDatabase);
  QSqlTableModel model;
  QModelIndex index;

  // Populate database and setup model
  QVERIFY(scenario1.populate());
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
}

void mdtDatabaseWidgetTest::sqlTableSelectionRowTest()
{
  mdtDatabaseWidgetTestScenario1Data scenario1(pvDatabase);
  QSqlTableModel model;
  QModelIndex index;
  mdtSqlTableSelectionRow row;

  // Populate database and setup model
  QVERIFY(scenario1.populate());
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
}

void mdtDatabaseWidgetTest::sqlTableSelectionTest()
{
  mdtDatabaseWidgetTestScenario1Data scenario1(pvDatabase);
  QSqlTableModel model;
  QTableView view;
  QItemSelectionModel *selectionModel;
  QModelIndex index;
  mdtSqlTableSelection s;
  QStringList fieldList;
  QList<QVariant> expectedDataList;

  // Populate database and setup model + view
  QVERIFY(scenario1.populate());
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
}

void mdtDatabaseWidgetTest::sqlSelectionDialogTest()
{
  mdtDatabaseWidgetTestScenario1Data scenario1(pvDatabase);
  mdtSqlSelectionDialog *dialog;
  QSqlQuery q;
  QString sql;
  mdtSqlTableSelection s;
  QStringList fields;

  // Populate database
  QVERIFY(scenario1.populate());
  /*
   * Check single selection with 1 field, at column 0
   */
  sql = "SELECT * FROM Client_tbl";
  dialog = new mdtSqlSelectionDialog;
  QVERIFY(dialog->setQuery(sql, pvDatabase));
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
  QVERIFY(dialog->setQuery(sql, pvDatabase));
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
  QVERIFY(dialog->setQuery(sql, pvDatabase));
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
  QVERIFY(dialog->setQuery(sql, pvDatabase));
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
  QVERIFY(dialog->setQuery(sql, pvDatabase));
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
  QVERIFY(dialog->setQuery(sql, pvDatabase, true));
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
  QVERIFY(dialog->setQuery(sql, pvDatabase, true));
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
  QVERIFY(dialog->setQuery(sql, pvDatabase, true));
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
}

void mdtDatabaseWidgetTest::sqlTableWidgetTest()
{
  mdtDatabaseWidgetTestScenario1Data scenario1(pvDatabase);
  mdtSqlTableWidget *sqlTableWidget;
  std::shared_ptr<mdtSqlTableViewController> addressController;
  mdtSqlTableWidget *addressWidget;
  mdtSqlRelationInfo relationInfo;
  QTableView *view;
  mdtSqlTableSelection s;
  QStringList fields;
  QModelIndex index;

  // Populate database
  QVERIFY(scenario1.populate());
  // Setup client widget
  sqlTableWidget = new mdtSqlTableWidget;
  sqlTableWidget->setTableName("Client_tbl", pvDatabase);
  sqlTableWidget->resize(500, 300);
  sqlTableWidget->show();
  view = sqlTableWidget->tableView();
  QVERIFY(view != 0);
  view->setSelectionMode(QAbstractItemView::MultiSelection);
  sqlTableWidget->start();
  QVERIFY(sqlTableWidget->select());
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
  // Setup address relation
  relationInfo.setChildTableName("Address_tbl");
  relationInfo.addRelation("Id_PK", "Client_Id_FK", true);
  // Add address controller
  sqlTableWidget->addChildTable(relationInfo);
  addressController = sqlTableWidget->tableController("Address_tbl");
  QVERIFY(addressController.get() != 0);
  // Setup address widget
  addressWidget = new mdtSqlTableWidget;
  addressWidget->setTableController(addressController);
  addressWidget->resize(500, 250);
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
  QCOMPARE(sqlTableWidget->currentData("Id_PK"), QVariant(3));
  QVERIFY(addressWidget->currentData("Client_Id_FK").isNull());
  // Select row 1 and check (Charly, Id_PK 4)
  view->selectRow(1);
  QCOMPARE(sqlTableWidget->currentData("Id_PK"), QVariant(4));
  QVERIFY(addressWidget->currentData("Client_Id_FK").isNull());
  // Select row 0 and check (Zeta, Id_PK 3)
  view->clearSelection();
  view->selectRow(0);
  QCOMPARE(sqlTableWidget->currentData("Id_PK"), QVariant(3));
  QVERIFY(addressWidget->currentData("Client_Id_FK").isNull());
  // Select row 2 and check (Bety, Id_PK 2)
  view->clearSelection();
  view->selectRow(2);
  QCOMPARE(sqlTableWidget->currentData("Id_PK"), QVariant(2));
  QCOMPARE(addressWidget->currentData("Client_Id_FK"), QVariant(2));
  // Select row 3 and check (Andy, Id_PK 1)
  view->clearSelection();
  view->selectRow(3);
  QCOMPARE(sqlTableWidget->currentData("Id_PK"), QVariant(1));
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
  delete sqlTableWidget;
}

void mdtDatabaseWidgetTest::sqlTableWidgetCsvExportTest()
{
  mdtDatabaseWidgetTestScenario1Data scenario1(pvDatabase);
  mdtSqlTableWidget *sqlTableWidget;
  std::shared_ptr<mdtSqlTableViewController> addressController;
  ///mdtSqlTableWidget *addressWidget;
  ///mdtSqlRelationInfo relationInfo;
  QTableView *view;
  ///mdtSqlTableSelection s;
  ///QStringList fields;
  ///QModelIndex index;
  ///QTemporaryFile tmpFile;
  QTemporaryFile csvFile;
  ///QFile csvFile;
  mdtCsvFileSettings csvSettings;
  QByteArray expecetedCsvData;

  // Populate database
//   populateTestDatabase();
  QVERIFY(scenario1.populate());
  // Setup client widget
  sqlTableWidget = new mdtSqlTableWidget;
  sqlTableWidget->setTableName("Client_tbl", pvDatabase);
  sqlTableWidget->resize(500, 300);
  sqlTableWidget->show();
  view = sqlTableWidget->tableView();
  QVERIFY(view != 0);
  view->setSelectionMode(QAbstractItemView::MultiSelection);
  sqlTableWidget->start();
  QVERIFY(sqlTableWidget->select());
  // Setup sorting
  sqlTableWidget->addColumnToSortOrder("FirstName", Qt::AscendingOrder);
  sqlTableWidget->sort();

  /*
   * Create CSV file
   */
  QVERIFY(csvFile.open());
  qDebug() << "CSV file: " << csvFile.fileName();
  csvFile.close();

  /*
   * Export to CSV file and check
   */
  QVERIFY(sqlTableWidget->exportToCsvFile(csvFile, csvSettings, true));
  // Build expected result
  expecetedCsvData  = "Id_PK,FirstName,Remarks,SomeValueDouble\r\n";
  expecetedCsvData += "1,Andy,,\r\n";
  expecetedCsvData += "2,Bety,Remark on Bety,2\r\n";
  expecetedCsvData += "4,Charly,Remark on Charly,\r\n";
  expecetedCsvData += "3,Zeta,Remark on Zeta,\r\n";
  // Read CSV file and check
  QVERIFY(csvFile.open());
  
  ///qDebug() << "CSV file: " << csvFile.readAll();
  
  QCOMPARE(csvFile.readAll(), expecetedCsvData);

  /// \todo Check also that comma are removed if no data protection is used
  
  /*
   * Play
   */
  /*
  while(sqlTableWidget->isVisible()){
    QTest::qWait(1000);
  }
  */

  // Cleanup
//   clearTestDatabaseData();
  delete sqlTableWidget;
}

void mdtDatabaseWidgetTest::sqlFormTest()
{
  mdtDatabaseWidgetTestScenario1Data scenario1(pvDatabase);
  mdtSqlForm form(0, pvDatabase);
  sqlDataWidgetControllerTestWidget clientWidget;
  mdtSqlRelationInfo relationInfo;

  // Create test data
  QVERIFY(scenario1.populate());
  // Setup form - Main table part
  form.setMainTableWidget(&clientWidget);
  QVERIFY(form.setMainTable("Client_tbl", "Clients", "FirstName"));
  // Setup address part
  relationInfo.setChildTableName("Address_tbl");
  relationInfo.addRelation("Id_PK", "Client_Id_FK", true);
  QVERIFY(form.addChildTable(relationInfo, "Addresses"));
  QVERIFY(form.sqlTableWidget("Address_tbl") != 0);
  QVERIFY(form.sqlTableWidget("NotExisting_tbl") == 0);
  // Start and select data
  form.start();
  QVERIFY(form.select());
  form.show();
  QCOMPARE(form.rowCount("Client_tbl"), 4);
  // Check that we are at correct row (Andy)
  QCOMPARE(form.currentRow(), 0);
  QCOMPARE(form.currentData("Client_tbl", "Id_PK"), QVariant(1));
  QCOMPARE(form.rowCount("Address_tbl"), 2);
  /*
   * Check insertion
   */
  QVERIFY(form.insert());
  QCOMPARE(form.rowCount("Client_tbl"), 5);
  // Check that we are at new row
  QCOMPARE(form.currentRow(), 4);
  QVERIFY(form.currentData("Client_tbl", "Id_PK").isNull());
  QCOMPARE(form.rowCount("Address_tbl"), 0);
  // Edit name - don't save now
  QVERIFY(form.setCurrentData("Client_tbl", "FirstName", "New name 1", false));
  ///QVERIFY(!form.allDataAreSaved());
  QCOMPARE(form.currentData("Client_tbl", "FirstName"), QVariant("New name 1"));
  // Edit remark and save
  QVERIFY(form.setCurrentData("Client_tbl", "Remarks", "New remark 1", true));
  QVERIFY(form.allDataAreSaved());
  QCOMPARE(form.currentRow(), 4);
  QCOMPARE(form.currentData("Client_tbl", "Remarks"), QVariant("New remark 1"));

  /*
   * Check data getters
   */
  
  /*
   * Check data setters
   */
  
  /*
   * Check filtering
   */
  
  // Check select by table name
  QVERIFY(form.select("Client_tbl"));
  QVERIFY(form.select("Address_tbl"));
  QVERIFY(!form.select("NotExisting_tbl"));

  /*
   * Play
   */
  /*
  while(form.isVisible()){
    QTest::qWait(500);
  }
  */
}

void mdtDatabaseWidgetTest::mdtSqlDialogTest()
{
  mdtDatabaseWidgetTestScenario1Data scenario1(pvDatabase);
  mdtSqlDialog sqlDialog;
  mdtSqlForm *form;
  QWidget *formWidget;

  // Create test data
  QVERIFY(scenario1.populate());

  sqlDialog.show();
  /*
   * Setup - We simly create a fake main widget for form
   */
  form = new mdtSqlForm(0, pvDatabase);
  formWidget = new QWidget;
  formWidget->setLayout(new QVBoxLayout);
  form->setMainTableWidget(formWidget);
  QVERIFY(form->setMainTable("Client_tbl", "Clients"));
  sqlDialog.setSqlForm(form);

  /*
   * Play
   */
  /*
  while(sqlDialog.isVisible()){
    QTest::qWait(500);
  }
  */

}

void mdtDatabaseWidgetTest::sqlFieldSelectionDialogTest()
{
  mdtSqlFieldSelectionDialog fsDialog(0);
  QStringList fnList;

  QVERIFY(fsDialog.getFieldNames().isEmpty());
  QVERIFY(fsDialog.getSelectedFieldNames().isEmpty());
  // Check sort on empty list - must not crach
  fsDialog.sort();
  fsDialog.sort(Qt::DescendingOrder);
  // Add fields
  fsDialog.addField("A", "a", false);
  fsDialog.addField("F", "f", false);
  fsDialog.addField("B", "b", true);
  fsDialog.addField("D", "d", true);
  fsDialog.addField("C", "c", false);
  // Check added field names
  fnList = fsDialog.getFieldNames();
  QCOMPARE(fnList.size(), 5);
  QCOMPARE(fnList.at(0), QString("A"));
  QCOMPARE(fnList.at(1), QString("F"));
  QCOMPARE(fnList.at(2), QString("B"));
  QCOMPARE(fnList.at(3), QString("D"));
  QCOMPARE(fnList.at(4), QString("C"));
  // Check selected field names
  fnList = fsDialog.getSelectedFieldNames();
  QCOMPARE(fnList.size(), 2);
  QCOMPARE(fnList.at(0), QString("B"));
  QCOMPARE(fnList.at(1), QString("D"));
  // Sort and check again
  fsDialog.sort();
  fnList = fsDialog.getFieldNames();
  QCOMPARE(fnList.size(), 5);
  QCOMPARE(fnList.at(0), QString("A"));
  QCOMPARE(fnList.at(1), QString("B"));
  QCOMPARE(fnList.at(2), QString("C"));
  QCOMPARE(fnList.at(3), QString("D"));
  QCOMPARE(fnList.at(4), QString("F"));
  // Check selected field names
  fnList = fsDialog.getSelectedFieldNames();
  QCOMPARE(fnList.size(), 2);
  QCOMPARE(fnList.at(0), QString("B"));
  QCOMPARE(fnList.at(1), QString("D"));
  // Sort descendning and check again
  fsDialog.sort(Qt::DescendingOrder);
  fnList = fsDialog.getFieldNames();
  QCOMPARE(fnList.size(), 5);
  QCOMPARE(fnList.at(0), QString("F"));
  QCOMPARE(fnList.at(1), QString("D"));
  QCOMPARE(fnList.at(2), QString("C"));
  QCOMPARE(fnList.at(3), QString("B"));
  QCOMPARE(fnList.at(4), QString("A"));
  // Check selected field names
  fnList = fsDialog.getSelectedFieldNames();
  QCOMPARE(fnList.size(), 2);
  QCOMPARE(fnList.at(0), QString("D"));
  QCOMPARE(fnList.at(1), QString("B"));

}

void mdtDatabaseWidgetTest::sqlApplicationWidgetsTest()
{
  // Following line must not compile
  //mdtSqlApplicationWidgetsTest wt;

  mdtSqlApplicationWidgetsTest::setDatabase(pvDatabase);
  QVERIFY(mdtSqlApplicationWidgetsTest::database().tables() == pvDatabase.tables());
  
  mdtError e("Test", mdtError::Warning);
  mdtSqlApplicationWidgetsTest::doSomeThing();
  
  // Cleanup
  mdtSqlApplicationWidgetsTest::clear();
}

/*
 * Helper methods
 */

void mdtDatabaseWidgetTest::createDatabaseSchema()
{
  ///QTemporaryFile dbFile;
  ///QFileInfo dbFileInfo;
  mdtSqlDatabaseSchema s;
  mdtSqlSchemaTable ts;
  mdtSqlForeignKey fk;
  mdtSqlField field;
  ///mdtSqlDatabaseSqlite db;

  /*
   * Init and open database
   */
  QVERIFY(pvTempFile.open());
  pvDatabase = QSqlDatabase::addDatabase("QSQLITE");
  pvDatabase.setDatabaseName(pvTempFile.fileName());
  QVERIFY(pvDatabase.open());

//   pvDatabase = QSqlDatabase::addDatabase("QSQLITE");
//   db.setDatabase(pvDatabase);
//   QVERIFY(db.isValid());
//   QVERIFY(pvTempFile.open());
//   QVERIFY(db.openDatabase(pvTempFile));
//   QVERIFY(pvDatabase.isOpen());
  /*
   * Check Sqlite database creation
   */
//   QVERIFY(dbFile.open());
//   dbFile.close();
//   dbFileInfo.setFile(dbFile.fileName() + ".db");
//   QVERIFY(pvDatabaseManager.createDatabaseSqlite(dbFileInfo, mdtSqlDatabaseManager::OverwriteExisting));
  /*
   * Create schema
   */
  // Client_tbl
  ts.clear();
  ts.setTableName("Client_tbl", "UTF8");
  // Id_PK
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  ts.addField(field, true);
  // FirstName
  field.clear();
  field.setName("FirstName");
  field.setRequired(true);
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  ts.addField(field, false);
  // Remarks
  field.clear();
  field.setName("Remarks");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(100);
  ts.addField(field, false);
  ///QVERIFY(pvDatabaseManager.createTable(ts, mdtSqlDatabaseManager::OverwriteExisting));
  // SomeValueDouble
  field.clear();
  field.setName("SomeValueDouble");
  field.setType(mdtSqlFieldType::Double);
  ts.addField(field, false);
  s.addTable(ts);
  ///QVERIFY(pvDatabaseManager.createTable(ts, mdtSqlDatabaseManager::OverwriteExisting));
  /*
   * ClientDetail_tbl - Linked 1-1 to Client_tbl (Real cases would be SomeTable_tbl based on Client_tbl)
   */
  ts.clear();
  ts.setTableName("ClientDetail_tbl", "UTF8");
  // Client_Id_FK_PK
  field.clear();
  field.setName("Client_Id_FK_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(false);
  ts.addField(field, true);
  fk.clear();
  fk.setParentTableName("Client_tbl");
  fk.setCreateChildIndex(true);
  fk.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk.addKeyFields("Id_PK", field);
  ts.addForeignKey(fk);
//   ts.addForeignKey("Client_Id_FK_PK_fk", "Client_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
//   QVERIFY(ts.addFieldToForeignKey("Client_Id_FK_PK_fk", "Client_Id_FK_PK", "Id_PK"));
  // Detail
  field.clear();
  field.setName("Detail");
  field.setRequired(true);
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(100);
  ts.addField(field, false);
  s.addTable(ts);
  ///QVERIFY(pvDatabaseManager.createTable(ts, mdtSqlDatabaseManager::OverwriteExisting));
  // Address_tbl
  ts.clear();
  ts.setTableName("Address_tbl", "UTF8");
  // Id_PK
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  ts.addField(field, true);
  // StreetName
  field.clear();
  field.setName("StreetName");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  ts.addField(field, false);
  // StreetNumber
  field.clear();
  field.setName("StreetNumber");
  field.setType(mdtSqlFieldType::Integer);
  ts.addField(field, false);
  // Client_Id_FK
  field.clear();
  field.setName("Client_Id_FK");
  field.setType(mdtSqlFieldType::Integer);
  ts.addField(field, false);
  fk.clear();
  fk.setParentTableName("Client_tbl");
  fk.setCreateChildIndex(true);
  fk.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk.addKeyFields("Id_PK", field);
  ts.addForeignKey(fk);
  s.addTable(ts);
//   ts.addForeignKey("Client_Id_FK_fk", "Client_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
//   QVERIFY(ts.addFieldToForeignKey("Client_Id_FK_fk", "Client_Id_FK", "Id_PK"));
  ///QVERIFY(pvDatabaseManager.createTable(ts, mdtSqlDatabaseManager::OverwriteExisting));
  // Create schema
  QVERIFY(s.createSchema(pvDatabase));

  // Enable foreing keys support
  ///mdtSqlForeignKeySetting fkSetting(pvDatabase, mdtSqlForeignKeySetting::Permanent);
  mdtSqlForeignKeySetting fkSetting(pvDatabase, mdtSqlForeignKeySetting::Permanent);
  QVERIFY(fkSetting.enable());
//   QVERIFY(pvDatabaseManager.setForeignKeysEnabled(true));
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
