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

#include <QDebug>

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
