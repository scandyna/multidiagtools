/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "SqlTableViewControllerTest.h"
#include "ItemViewTestEdit.h"
#include "Mdt/Application.h"
#include "Mdt/ItemEditor/SqlTableViewController.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include <QSignalSpy>
#include <QStringListModel>
#include <QTableView>
#include <QSpinBox>
#include <QComboBox>
#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaMethod>
#include <QSqlQuery>

namespace ItemEditor = Mdt::ItemEditor;
namespace ItemModel = Mdt::ItemModel;
using ItemEditor::SqlTableViewController;
using ItemEditor::ControllerState;
using ItemModel::VariantTableModel;
using ItemModel::FilterColumn;
using ItemModel::ParentModelColumn;

void SqlTableViewControllerTest::initTestCase()
{
  // Get database instance
  mDatabase = QSqlDatabase::addDatabase("QSQLITE");
  if(!mDatabase.isValid()){
    QSKIP("QSQLITE driver is not available - Skip all tests");  // Will also skip all tests
  }
  // Create a database
  QVERIFY(mTempFile.open());
  mTempFile.close();
  mDatabase.setDatabaseName(mTempFile.fileName());
  QVERIFY(mDatabase.open());
  QSqlQuery query(mDatabase);
  // Specify encoding (is important for some tests)
  QVERIFY(query.exec("PRAGMA encoding = \"UTF-8\""));
}

void SqlTableViewControllerTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void SqlTableViewControllerTest::setModelThenViewTest()
{
  QFAIL("Not complete");
}

void SqlTableViewControllerTest::setViewThenModelTest()
{
  QFAIL("Not complete");
}

void SqlTableViewControllerTest::changeModelTest()
{
  QFAIL("Not complete");
}

void SqlTableViewControllerTest::setTableTest()
{

  /*
   * Set table without any model set
   */
  
  /*
   * Change table
   */
  
  /*
   * Change model
   */
  
  /*
   * Check setting with schema table
   */
  
  /*
   * Check setting with table template
   */
  
  QFAIL("Not complete");
}

/*
 * Helpers
 */

void SqlTableViewControllerTest::beginEditing(QAbstractItemView& view, const QModelIndex& index, BeginEditTrigger trigger)
{
  ItemViewTestEdit::beginEditing(view, index, trigger);
}

void SqlTableViewControllerTest::editText(QAbstractItemView& view, const QModelIndex& editingIndex, const QString& str)
{
  ItemViewTestEdit::editText(view, editingIndex, str);
}

void SqlTableViewControllerTest::endEditing(QAbstractItemView& view, const QModelIndex& editingIndex, EndEditTrigger trigger)
{
  ItemViewTestEdit::endEditing(view, editingIndex, trigger);
}

void SqlTableViewControllerTest::edit(QAbstractItemView& view, const QModelIndex& index, const QString& str, BeginEditTrigger beginEditTrigger, EndEditTrigger endEditTrigger)
{
  ItemViewTestEdit::edit(view, index, str, beginEditTrigger, endEditTrigger);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  SqlTableViewControllerTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
