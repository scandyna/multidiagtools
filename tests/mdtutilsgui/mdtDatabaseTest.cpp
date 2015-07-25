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
#include "mdtDatabaseTest.h"
#include "mdtApplication.h"
#include "mdtApplication.h"
#include "mdtDoubleEdit.h"
#include "mdtSortFilterProxyModel.h"
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
/// \todo Rewrite using mdtSqlForm
void mdtDatabaseTest::relationsTest()
{
  QFAIL("Not implemeted yet..");
  /**
  QSqlTableModel parentModel;
  QSqlTableModel childModel;
  QModelIndex index;
  mdtSqlRelation *relation;
  mdtSqlFormWidget *clientWidget;
  mdtSqlTableWidget *addressWidget;
  Ui::mdtSqlFormWidgetTestForm form;
  ///mdtSqlWindowOld window;
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
*/
  
  // Play...
  /**
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
  QVERIFY(!proxy.hasColumnToSort());
  model.setTable("somedata");
  proxy.setSourceModel(&model);
  proxy.addColumnToSortOrder(1);
  QVERIFY(proxy.hasColumnToSort());
  proxy.addColumnToSortOrder(&model, "remarks");
  QVERIFY(proxy.hasColumnToSort());
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

  // Disable sorting and check
  proxy.disableSorting();
  QVERIFY(model.select());
  QCOMPARE(proxy.rowCount(), 10);
  index = proxy.index(0, 1);
  QCOMPARE(proxy.data(index).toString(), QString("Andy"));
  index = proxy.index(1, 1);
  QCOMPARE(proxy.data(index).toString(), QString("Bety"));
  index = proxy.index(2, 1);
  QCOMPARE(proxy.data(index).toString(), QString("Andy"));
  index = proxy.index(3, 1);
  QCOMPARE(proxy.data(index).toString(), QString("Andy"));
  index = proxy.index(4, 1);
  QCOMPARE(proxy.data(index).toString(), QString("Bety"));
  index = proxy.index(0, 2);
  QCOMPARE(proxy.data(index).toString(), QString("REM 1"));
  index = proxy.index(1, 2);
  QCOMPARE(proxy.data(index).toString(), QString("REM 3"));
  index = proxy.index(2, 2);
  QCOMPARE(proxy.data(index).toString(), QString("REM 10"));
  index = proxy.index(3, 2);
  QCOMPARE(proxy.data(index).toString(), QString("REM 2"));
  index = proxy.index(4, 2);
  QCOMPARE(proxy.data(index).toString(), QString("REM 1"));
  index = proxy.index(5, 1);
  QCOMPARE(proxy.data(index).toString(), QString("Andy"));
  index = proxy.index(6, 1);
  QCOMPARE(proxy.data(index).toString(), QString("Andy"));
  index = proxy.index(7, 1);
  QCOMPARE(proxy.data(index).toString(), QString("Bety"));
  index = proxy.index(8, 1);
  QCOMPARE(proxy.data(index).toString(), QString("Bety"));
  index = proxy.index(9, 1);
  QCOMPARE(proxy.data(index).toString(), QString("Bety"));
  index = proxy.index(5, 2);
  QCOMPARE(proxy.data(index).toString(), QString("REM 3"));
  index = proxy.index(6, 2);
  QCOMPARE(proxy.data(index).toString(), QString("REM 15"));
  index = proxy.index(7, 2);
  QCOMPARE(proxy.data(index).toString(), QString("REM 10"));
  index = proxy.index(8, 2);
  QCOMPARE(proxy.data(index).toString(), QString("REM 2"));
  index = proxy.index(9, 2);
  QCOMPARE(proxy.data(index).toString(), QString("REM 15"));

  // Re-enable sorting and check
  proxy.enableSorting();
  proxy.sort();
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
