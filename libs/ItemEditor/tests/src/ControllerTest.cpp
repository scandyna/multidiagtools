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
#include "ControllerTest.h"
#include "ItemViewTestEdit.h"
#include "Mdt/Application.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include "Mdt/ItemEditor/ControllerStatePermission.h"
#include "Mdt/ItemEditor/TableViewController.h"
#include "Mdt/ItemEditor/WidgetMapperController.h"
#include "Mdt/ItemEditor/ItemSelectionModel.h"
#include <QSignalSpy>
#include <QStringListModel>
#include <QTableView>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaMethod>

namespace ItemModel = Mdt::ItemModel;
namespace ItemEditor = Mdt::ItemEditor;
using ItemModel::VariantTableModel;
using ItemEditor::ControllerState;
using ItemEditor::TableViewController;
using ItemEditor::WidgetMapperController;

void ControllerTest::initTestCase()
{
}

void ControllerTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void ControllerTest::statePermissionTest()
{
  using Mdt::ItemEditor::ControllerState;
  using Mdt::ItemEditor::ControllerStatePermission;

  // Current row change
  QVERIFY(ControllerStatePermission::canChangeCurrentRow(ControllerState::Visualizing));
  QVERIFY(!ControllerStatePermission::canChangeCurrentRow(ControllerState::Editing));
  // Insert
  QVERIFY(ControllerStatePermission::canInsert(ControllerState::Visualizing));
  QVERIFY(!ControllerStatePermission::canInsert(ControllerState::Editing));
  // Submit
  QVERIFY(!ControllerStatePermission::canSubmit(ControllerState::Visualizing));
  QVERIFY(ControllerStatePermission::canSubmit(ControllerState::Editing));
  // Revert
  QVERIFY(!ControllerStatePermission::canRevert(ControllerState::Visualizing));
  QVERIFY(ControllerStatePermission::canRevert(ControllerState::Editing));
  // Remove
  QVERIFY(ControllerStatePermission::canRemove(ControllerState::Visualizing));
  QVERIFY(!ControllerStatePermission::canRemove(ControllerState::Editing));
}

// void ControllerTest::controllerListTest()
// {
//   ControllerList list;
//   TableViewController tableController;
// 
//   /*
//    * Initial state
//    */
//   QCOMPARE(list.size(), 0);
//   QVERIFY(list.isEmpty());
//   QVERIFY(list.cbegin() == list.cend());
//   /*
//    * Add 1 element
//    */
//   list.addController(&tableController);
//   QCOMPARE(list.size(), 1);
//   QVERIFY(!list.isEmpty());
//   QVERIFY(list.cbegin() != list.cend());
//   /*
//    * Clear
//    */
//   list.clear();
//   QVERIFY(list.isEmpty());
// }

/*
 * Helper functions
 */

void ControllerTest::beginEditing(QAbstractItemView& view, const QModelIndex& index, BeginEditTrigger trigger)
{
  ItemViewTestEdit::beginEditing(view, index, trigger);
}

void ControllerTest::editText(QAbstractItemView& view, const QModelIndex& editingIndex, const QString& str)
{
  ItemViewTestEdit::editText(view, editingIndex, str);
}

void ControllerTest::endEditing(QAbstractItemView& view, const QModelIndex& editingIndex, EndEditTrigger trigger)
{
  ItemViewTestEdit::endEditing(view, editingIndex, trigger);
}

void ControllerTest::edit(QAbstractItemView& view, const QModelIndex& index, const QString& str,
                            BeginEditTrigger beginEditTrigger, EndEditTrigger endEditTrigger)
{
  ItemViewTestEdit::edit(view, index, str, beginEditTrigger, endEditTrigger);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  ControllerTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
