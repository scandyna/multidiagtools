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
#include "TestTableModel.h"
#include "Mdt/Application.h"
#include "Mdt/ItemEditor/ControllerStatePermission.h"
#include "Mdt/ItemEditor/TableController.h"
#include "Mdt/ItemEditor/ItemSelectionModel.h"
#include "Mdt/ItemEditor/RowChangeEventMapper.h"
#include <QSignalSpy>
#include <QStringListModel>
#include <QTableView>
#include <QObject>

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
}

void ControllerTest::setModelTest()
{
  using Mdt::ItemEditor::TableController;

  TestTableModel tableModel;
  QStringListModel listModel;

  /*
   * Initial state
   */
  TableController controller;
  QVERIFY(controller.model() == nullptr);
  /*
   * Set model
   */
  controller.setModel(&tableModel);
  QVERIFY(controller.model() == &tableModel);
  
  /*
   * Change model
   */
  controller.setModel(&listModel);
  QVERIFY(controller.model() == &listModel);
  
}

void ControllerTest::currentRowChangeTest()
{
  using Mdt::ItemEditor::TableController;

  TestTableModel model;
  TableController controller;

  /*
   * Check on empty model
   */
  
  /*
   * Populate and check
   */
  
}

void ControllerTest::currentRowChangeTableViewTest()
{
  using Mdt::ItemEditor::TableController;
  using Mdt::ItemEditor::ItemSelectionModel;
  using Mdt::ItemEditor::RowChangeEventMapper;
  using Mdt::ItemEditor::RowState;

  TableController controller;
  TestTableModel model;
  ItemSelectionModel selectionModel(&model);
  QTableView tableView;
  RowChangeEventMapper rowChangeEventMapper;
  QSignalSpy rowStateSpy(&controller, &TableController::rowStateChanged);
  QList<QVariant> spyItem;
  RowState rs;

  /*
   * Setup
   */
  controller.setModel(&model);
  tableView.setModel(&model);
  tableView.setSelectionModel(&selectionModel);
  connect(&rowChangeEventMapper, &RowChangeEventMapper::rowStateChanged, &controller, &TableController::setRowState);
  tableView.show();

  /*
   * Initial state
   */
  QCOMPARE(controller.rowCount(), model.rowCount());
  QCOMPARE(controller.currentRow(), selectionModel.currentIndex().row());
  // Check that row count was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), selectionModel.currentIndex().row());
  /*
   * Populate model
   */
  model.populate(5, 3);
  QCOMPARE(controller.rowCount(), model.rowCount());
  QCOMPARE(controller.currentRow(), selectionModel.currentIndex().row());
  /*
   * Change current row from controller
   */

  /*
   * Check navigation slots
   */
  
  /*
   * Change model
   */
  
  /*
   * Insert
   */
  
  /*
   * Remove
   */
   

  /*
   * Play
   */
  while(tableView.isVisible()){
    QTest::qWait(500);
  }
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
