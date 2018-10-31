/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "DataWidgetMapperTest.h"
#include "Mdt/Application.h"
#include "Mdt/ItemEditor/MappedWidgetList.h"
#include "Mdt/ItemEditor/DataWidgetMapper.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include "Mdt/ItemModel/FormatProxyModel.h"
#include "Mdt/TestLib/ItemModel.h"
#include "Mdt/TestLib/Widgets.h"
#include <QSignalSpy>
#include <QScopedPointer>
#include <QStringListModel>
#include <QModelIndex>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QTextEdit>
#include <QPlainTextEdit>

#include <QTableView>

using namespace Mdt::ItemModel;
using namespace Mdt::ItemEditor;
using namespace Mdt::TestLib;

void DataWidgetMapperTest::initTestCase()
{
}

void DataWidgetMapperTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void DataWidgetMapperTest::setModelThenMapWidgetsTest()
{
  DataWidgetMapper mapper;
  VariantTableModel model;
  QLineEdit edit0, edit1;

  QSignalSpy rowChangedSpy(&mapper, &DataWidgetMapper::currentRowChanged);
  QVERIFY(rowChangedSpy.isValid());
  QSignalSpy editStartedSpy(&mapper, &DataWidgetMapper::dataEditionStarted);
  QVERIFY(editStartedSpy.isValid());
  /*
   * Initial state
   */
  QVERIFY(mapper.model() == nullptr);
//   QVERIFY(mapper.itemDelegate() != nullptr);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  /*
   * Setup and set model
   */
  model.resize(1, 2);
  model.populateColumn(0, {"1"});
  model.populateColumn(1, {"A"});
  mapper.setModel(&model);
  QVERIFY(mapper.model() == &model);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  /*
   * Map widgets
   */
  mapper.addMapping(&edit0, 0);
  mapper.addMapping(&edit1, 1);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  QVERIFY(!edit0.isEnabled());
  QVERIFY(!edit1.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.text().isEmpty());
  /*
   * Check that mapping works
   */
  mapper.setCurrentRow(0);
  QCOMPARE(mapper.currentRow(), 0);
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(editStartedSpy.count(), 0);
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  QCOMPARE(edit0.text(), QString("1"));
  QCOMPARE(edit1.text(), QString("A"));
  rowChangedSpy.clear();
}

void DataWidgetMapperTest::mapWidgetsThenSetModelTest()
{
  DataWidgetMapper mapper;
  VariantTableModel model;
  QLineEdit edit0, edit1;

  QSignalSpy rowChangedSpy(&mapper, &DataWidgetMapper::currentRowChanged);
  QVERIFY(rowChangedSpy.isValid());
  QSignalSpy editStartedSpy(&mapper, &DataWidgetMapper::dataEditionStarted);
  QVERIFY(editStartedSpy.isValid());
  /*
   * Initial state
   */
  QVERIFY(mapper.model() == nullptr);
//   QVERIFY(mapper.itemDelegate() != nullptr);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  /*
   * Map widgets
   */
  mapper.addMapping(&edit0, 0);
  mapper.addMapping(&edit1, 1);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  QVERIFY(!edit0.isEnabled());
  QVERIFY(!edit1.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.text().isEmpty());
  /*
   * Setup and set model
   */
  model.resize(1, 2);
  model.populateColumn(0, {"1"});
  model.populateColumn(1, {"A"});
  mapper.setModel(&model);
  QVERIFY(mapper.model() == &model);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  /*
   * Check that mapping works
   */
  mapper.setCurrentRow(0);
  QCOMPARE(mapper.currentRow(), 0);
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(editStartedSpy.count(), 0);
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  QCOMPARE(edit0.text(), QString("1"));
  QCOMPARE(edit1.text(), QString("A"));
  rowChangedSpy.clear();
}

void DataWidgetMapperTest::changeModelTest()
{
  DataWidgetMapper mapper;
  QLineEdit edit0, edit1;

  QSignalSpy rowChangedSpy(&mapper, &DataWidgetMapper::currentRowChanged);
  QVERIFY(rowChangedSpy.isValid());
  QSignalSpy editStartedSpy(&mapper, &DataWidgetMapper::dataEditionStarted);
  QVERIFY(editStartedSpy.isValid());
  /*
   * Initial state
   */
  QVERIFY(mapper.model() == nullptr);
//   QVERIFY(mapper.itemDelegate() != nullptr);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  /*
   * Set a empty model
   */
  VariantTableModel model0;
  QCOMPARE(model0.rowCount(), 0);
  mapper.setModel(&model0);
  QVERIFY(mapper.model() == &model0);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  /*
   * Map widgets
   */
  mapper.addMapping(&edit0, 0);
  mapper.addMapping(&edit1, 1);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  QVERIFY(!edit0.isEnabled());
  QVERIFY(!edit1.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.text().isEmpty());
  /*
   * Setup and set model
   */
  VariantTableModel model1;
  model1.resize(1, 2);
  model1.populateColumn(0, {"1"});
  model1.populateColumn(1, {"A"});
  mapper.setModel(&model1);
  QVERIFY(mapper.model() == &model1);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  /*
   * Check that mapping works
   */
  mapper.setCurrentRow(0);
  QCOMPARE(mapper.currentRow(), 0);
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(editStartedSpy.count(), 0);
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  QCOMPARE(edit0.text(), QString("1"));
  QCOMPARE(edit1.text(), QString("A"));
  rowChangedSpy.clear();
  /*
   * Setup and set a other model
   */
  VariantTableModel model2;
  model2.resize(1, 2);
  model2.populateColumn(0, {"2"});
  model2.populateColumn(1, {"B"});
  mapper.setModel(&model2);
  QVERIFY(mapper.model() == &model2);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(rowChangedSpy.takeFirst().at(0).toInt(), -1);
  QCOMPARE(editStartedSpy.count(), 0);
  QVERIFY(!edit0.isEnabled());
  QVERIFY(!edit1.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.text().isEmpty());
  /*
   * Check that mapping works
   */
  mapper.setCurrentRow(0);
  QCOMPARE(mapper.currentRow(), 0);
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(rowChangedSpy.takeFirst().at(0).toInt(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  QCOMPARE(edit0.text(), QString("2"));
  QCOMPARE(edit1.text(), QString("B"));
  rowChangedSpy.clear();
  /*
   * Setup and set a other model with less columns that widgets
   */
  VariantTableModel model3;
  model3.resize(1, 1);
  model3.populateColumn(0, {"3"});
  mapper.setModel(&model3);
  QVERIFY(mapper.model() == &model3);
  QCOMPARE(mapper.currentRow(), -1);
  QVERIFY(!edit0.isEnabled());
  QVERIFY(!edit1.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.text().isEmpty());
  /*
   * Check that mapping works
   */
  mapper.setCurrentRow(0);
  QCOMPARE(mapper.currentRow(), 0);
  QVERIFY(edit0.isEnabled());
  QVERIFY(!edit1.isEnabled());
  QCOMPARE(edit0.text(), QString("3"));
  QVERIFY(edit1.text().isEmpty());
  rowChangedSpy.clear();
}

void DataWidgetMapperTest::modelResetTest()
{
  DataWidgetMapper mapper;
  QLineEdit edit0, edit1;

  QSignalSpy rowChangedSpy(&mapper, &DataWidgetMapper::currentRowChanged);
  QVERIFY(rowChangedSpy.isValid());
  QSignalSpy editStartedSpy(&mapper, &DataWidgetMapper::dataEditionStarted);
  QVERIFY(editStartedSpy.isValid());
  /*
   * Initial state
   */
  QVERIFY(mapper.model() == nullptr);
//   QVERIFY(mapper.itemDelegate() != nullptr);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  /*
   * Set model
   */
  VariantTableModel model;
  mapper.setModel(&model);
  QVERIFY(mapper.model() == &model);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  /*
   * Map widgets
   */
  mapper.addMapping(&edit0, 0);
  mapper.addMapping(&edit1, 1);
  QVERIFY(!edit0.isEnabled());
  QVERIFY(!edit1.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.text().isEmpty());
  /*
   * Populate model with 1 column
   *
   * Note: widget mapper allways goes to row -1 after a model reset.
   *       The controller then set a appropriate current row.
   */
  model.populate(3, 1);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  QVERIFY(!edit0.isEnabled());
  QVERIFY(!edit1.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.text().isEmpty());
  // Now, change to first row
  mapper.setCurrentRow(0);
  QCOMPARE(mapper.currentRow(), 0);
  // Check that current row changed was signaled
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(rowChangedSpy.takeFirst().at(0).toInt(), 0);
  // Check that no edition started was signaled
  QCOMPARE(editStartedSpy.count(), 0);
  // Check editors state
  QVERIFY(edit0.isEnabled());
  QVERIFY(!edit1.isEnabled());
  QCOMPARE(edit0.text(), model.data(0, 0).toString());
  QVERIFY(edit1.text().isEmpty());
  /*
   * Populate model with 2 columns
   *
   * Note: widget mapper allways goes to row -1 after a model reset.
   *       The controller then set a appropriate current row.
   */
  model.populate(3, 2);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(rowChangedSpy.takeFirst().at(0).toInt(), -1);
  QCOMPARE(editStartedSpy.count(), 0);
  // Go to first row
  mapper.setCurrentRow(0);
  // Check that current row changed was signaled
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(rowChangedSpy.takeFirst().at(0).toInt(), 0);
  // Check that no edition started was signaled
  QCOMPARE(editStartedSpy.count(), 0);
  // Check that editors state was updated
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  QCOMPARE(edit0.text(), model.data(0, 0).toString());
  QCOMPARE(edit1.text(), model.data(0, 1).toString());
}

void DataWidgetMapperTest::addMappingTest()
{
  DataWidgetMapper mapper;
  VariantTableModel model;

  QSignalSpy rowChangedSpy(&mapper, &DataWidgetMapper::currentRowChanged);
  QVERIFY(rowChangedSpy.isValid());
  QSignalSpy editStartedSpy(&mapper, &DataWidgetMapper::dataEditionStarted);
  QVERIFY(editStartedSpy.isValid());
  /*
   * Setup and set model
   */
  model.resize(1, 2);
  model.populateColumn(0, {"1"});
  model.populateColumn(1, {"A"});
  mapper.setModel(&model);
  QCOMPARE(mapper.currentRow(), -1);
  /*
   * Map widgets
   */
  QLineEdit edit0, edit1;
  mapper.addMapping(&edit0, 0);
  mapper.addMapping(&edit1, 1);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  QVERIFY(!edit0.isEnabled());
  QVERIFY(!edit1.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.text().isEmpty());
  /*
   * Check that mapping works
   */
  mapper.setCurrentRow(0);
  QCOMPARE(mapper.currentRow(), 0);
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(editStartedSpy.count(), 0);
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  QCOMPARE(edit0.text(), QString("1"));
  QCOMPARE(edit1.text(), QString("A"));
  rowChangedSpy.clear();
}

void DataWidgetMapperTest::removeMappingTest()
{
  DataWidgetMapper mapper;
  VariantTableModel model;

  QSignalSpy rowChangedSpy(&mapper, &DataWidgetMapper::currentRowChanged);
  QVERIFY(rowChangedSpy.isValid());
  QSignalSpy editStartedSpy(&mapper, &DataWidgetMapper::dataEditionStarted);
  QVERIFY(editStartedSpy.isValid());
  /*
   * Setup and set model
   */
  model.resize(1, 2);
  model.populateColumn(0, {"1"});
  model.populateColumn(1, {"A"});
  mapper.setModel(&model);
  QCOMPARE(mapper.currentRow(), -1);
  /*
   * Map widgets
   */
  QLineEdit edit0, edit1;
  mapper.addMapping(&edit0, 0);
  mapper.addMapping(&edit1, 1);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  QVERIFY(!edit0.isEnabled());
  QVERIFY(!edit1.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.text().isEmpty());
  /*
   * Check that mapping works
   */
  mapper.setCurrentRow(0);
  QCOMPARE(mapper.currentRow(), 0);
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(editStartedSpy.count(), 0);
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  QCOMPARE(edit0.text(), QString("1"));
  QCOMPARE(edit1.text(), QString("A"));
  rowChangedSpy.clear();
  /*
   * Remove edit0
   */
  mapper.removeMapping(&edit0);
  QCOMPARE(rowChangedSpy.count(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  // Check that edit0 was unmapped
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  // Check that edit1 is still mapped
  QVERIFY(edit1.isEnabled());
  QCOMPARE(edit1.text(), QString("A"));
  /*
   * Go to invalid row
   */
  mapper.setCurrentRow(-1);
  QCOMPARE(mapper.currentRow(), -1);
  // Check that edit0 was unmapped
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  // Check that edit1 is still mapped
  QVERIFY(!edit1.isEnabled());
  QVERIFY(edit1.text().isEmpty());
  /*
   * Remove edit1
   */
  mapper.removeMapping(&edit1);
  QVERIFY(edit1.isEnabled());
  QVERIFY(edit1.text().isEmpty());
  /*
   * Check that both widgets are unmapped
   */
  mapper.setCurrentRow(0);
  QCOMPARE(mapper.currentRow(), 0);
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.isEnabled());
  QVERIFY(edit1.text().isEmpty());
}

void DataWidgetMapperTest::changeMappingTest()
{
  DataWidgetMapper mapper;
  VariantTableModel model;

  QSignalSpy rowChangedSpy(&mapper, &DataWidgetMapper::currentRowChanged);
  QVERIFY(rowChangedSpy.isValid());
  QSignalSpy editStartedSpy(&mapper, &DataWidgetMapper::dataEditionStarted);
  QVERIFY(editStartedSpy.isValid());
  /*
   * Initial state
   */
  QVERIFY(mapper.model() == nullptr);
//   QVERIFY(mapper.itemDelegate() != nullptr);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  /*
   * Setup and set model
   */
  model.resize(1, 2);
  model.populateColumn(0, {"1"});
  model.populateColumn(1, {"A"});
  mapper.setModel(&model);
  QVERIFY(mapper.model() == &model);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  /*
   * Map widgets
   */
  QLineEdit edit0, edit1;
  mapper.addMapping(&edit0, 0);
  mapper.addMapping(&edit1, 1);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  QVERIFY(!edit0.isEnabled());
  QVERIFY(!edit1.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.text().isEmpty());
  /*
   * Check that mapping works
   */
  mapper.setCurrentRow(0);
  QCOMPARE(mapper.currentRow(), 0);
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(editStartedSpy.count(), 0);
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  QCOMPARE(edit0.text(), QString("1"));
  QCOMPARE(edit1.text(), QString("A"));
  rowChangedSpy.clear();
  /*
   * Map edit0 to column 1
   * - edit0 must be mapped to column 1
   * - edit1 must be unmapped
   */
  mapper.addMapping(&edit0, 1);
  mapper.setCurrentRow(-1);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(editStartedSpy.count(), 0);
  // Check that edit0 was mapped
  QVERIFY(!edit0.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  // Check that edit1 was unmapped
  QVERIFY(edit1.isEnabled());
  QVERIFY(edit1.text().isEmpty());
  rowChangedSpy.clear();
  /*
   * Check that mapping works
   */
  mapper.setCurrentRow(0);
  QCOMPARE(mapper.currentRow(), 0);
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(editStartedSpy.count(), 0);
  QVERIFY(edit0.isEnabled());
  QCOMPARE(edit0.text(), QString("A"));
  // Check that edit1 was unmapped
  QVERIFY(edit1.text().isEmpty());
  rowChangedSpy.clear();
  /*
   * Map other widgets to the same columns
   */
  QLineEdit edit10, edit11;
  mapper.addMapping(&edit10, 0);
  mapper.addMapping(&edit11, 1);
  mapper.setCurrentRow(-1);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(editStartedSpy.count(), 0);
  QVERIFY(!edit10.isEnabled());
  QVERIFY(!edit11.isEnabled());
  QVERIFY(edit10.text().isEmpty());
  QVERIFY(edit11.text().isEmpty());
  // Check that old mapped widgets are unmapped
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  rowChangedSpy.clear();
  /*
   * Check that mapping works
   */
  mapper.setCurrentRow(0);
  QCOMPARE(mapper.currentRow(), 0);
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(editStartedSpy.count(), 0);
  QVERIFY(edit10.isEnabled());
  QVERIFY(edit11.isEnabled());
  QCOMPARE(edit10.text(), QString("1"));
  QCOMPARE(edit11.text(), QString("A"));
  // Check that old mapped widgets are unmapped
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.text().isEmpty());
  rowChangedSpy.clear();
}

void DataWidgetMapperTest::clearMappingTest()
{
  DataWidgetMapper mapper;
  VariantTableModel model;

  QSignalSpy rowChangedSpy(&mapper, &DataWidgetMapper::currentRowChanged);
  QVERIFY(rowChangedSpy.isValid());
  QSignalSpy editStartedSpy(&mapper, &DataWidgetMapper::dataEditionStarted);
  QVERIFY(editStartedSpy.isValid());
  /*
   * Setup and set model
   */
  model.resize(1, 2);
  model.populateColumn(0, {"1"});
  model.populateColumn(1, {"A"});
  mapper.setModel(&model);
  QCOMPARE(mapper.currentRow(), -1);
  /*
   * Map widgets
   */
  QLineEdit edit0, edit1;
  mapper.addMapping(&edit0, 0);
  mapper.addMapping(&edit1, 1);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  QVERIFY(!edit0.isEnabled());
  QVERIFY(!edit1.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.text().isEmpty());
  /*
   * Check that mapping works
   */
  mapper.setCurrentRow(0);
  QCOMPARE(mapper.currentRow(), 0);
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(editStartedSpy.count(), 0);
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  QCOMPARE(edit0.text(), QString("1"));
  QCOMPARE(edit1.text(), QString("A"));
  rowChangedSpy.clear();
  /*
   * Clear mapping
   */
  mapper.clearMapping();
  QCOMPARE(rowChangedSpy.count(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.text().isEmpty());
  /*
   * Check that all widget have been unmapped
   */
  mapper.setCurrentRow(-1);
  QCOMPARE(mapper.currentRow(), -1);
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.text().isEmpty());
  mapper.setCurrentRow(0);
  QCOMPARE(mapper.currentRow(), 0);
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.text().isEmpty());
}

/** \todo
 *  - Check that flags are updated when changing current row
 *     (Some model have fixed flags per index)
 *  - Check that flags are updated on notifier
 *     (dynamic update, typical case with ControllerStatePermissionProxyModel)
 */
void DataWidgetMapperTest::itemEditableFlagTest()
{
  /*
   * DataWidgetMapper uses MappedWidget,
   * so we don't have to check every widget types here
   */
  DataWidgetMapper mapper;
  VariantTableModel model;
  QModelIndex index;
  QLineEdit le;
  QComboBox cb;
  QWidget w;
  /*
   * Setup
   */
  model.populate(2, 3);
  mapper.setModel(&model);
  mapper.addMapping(&le, 0);
  mapper.addMapping(&cb, 1);
  mapper.addMapping(&w, 2);
  mapper.setCurrentRow(0);
  /*
   * Initial state
   */
  QVERIFY(!le.isReadOnly());
//   QVERIFY(cb.isEditable());
  QVERIFY(le.isEnabled());
  QVERIFY(cb.isEnabled());
  QVERIFY(w.isEnabled());
  /*
   * Unset editable flag
   */
  // Line edit
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  model.setItemEditable(index, false);
  mapper.updateFromModelFlags();
  QVERIFY(le.isReadOnly());
//   QVERIFY(cb.isEditable());
  QVERIFY(le.isEnabled());
  QVERIFY(cb.isEnabled());
  // Combobox
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  model.setItemEditable(index, false);
  mapper.updateFromModelFlags();
  QVERIFY(le.isReadOnly());
//   QVERIFY(!cb.isEditable());
  QVERIFY(le.isEnabled());
  QVERIFY(!cb.isEnabled());
  // Widget without any editable property
  index = model.index(0, 2);
  QVERIFY(index.isValid());
  model.setItemEditable(index, false);
  mapper.updateFromModelFlags();
  QVERIFY(!w.isEnabled());
  /*
   * Set editable flag (again)
   */
  // Line edit
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  model.setItemEditable(index, true);
  mapper.updateFromModelFlags();
  QVERIFY(!le.isReadOnly());
//   QVERIFY(!cb.isEditable());
  QVERIFY(le.isEnabled());
  QVERIFY(!cb.isEnabled());
  // Combobox
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  model.setItemEditable(index, true);
  mapper.updateFromModelFlags();
  QVERIFY(!le.isReadOnly());
//   QVERIFY(cb.isEditable());
  QVERIFY(le.isEnabled());
  QVERIFY(cb.isEnabled());
  // Widget without any editable property
  index = model.index(0, 2);
  QVERIFY(index.isValid());
  model.setItemEditable(index, true);
  mapper.updateFromModelFlags();
  QVERIFY(w.isEnabled());
  /*
   * Check that model's flags are also update when changing current row
   */
  // Set editable flag for row 1
  //  Line edit
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  model.setItemEditable(index, true);
  //  Combobox
  index = model.index(1, 1);
  QVERIFY(index.isValid());
  model.setItemEditable(index, true);
  //  Widget without any editable property
  index = model.index(1, 2);
  QVERIFY(index.isValid());
  model.setItemEditable(index, true);
  // Change current row and check
  mapper.setCurrentRow(1);
  QVERIFY(!le.isReadOnly());
//   QVERIFY(cb.isEditable());
  QVERIFY(le.isEnabled());
  QVERIFY(cb.isEnabled());
  QVERIFY(w.isEnabled());
  // Unset editable flag for row 0
  //  Line edit
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  model.setItemEditable(index, false);
  //  Combobox
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  model.setItemEditable(index, false);
  //  Widget without any editable property
  index = model.index(0, 2);
  QVERIFY(index.isValid());
  model.setItemEditable(index, false);
  // Change current row and check
  mapper.setCurrentRow(0);
  QVERIFY(le.isReadOnly());
//   QVERIFY(!cb.isEditable());
  QVERIFY(le.isEnabled());
  QVERIFY(!cb.isEnabled());
  QVERIFY(!w.isEnabled());
}

void DataWidgetMapperTest::itemEnabledFlagTest()
{
  DataWidgetMapper mapper;
  VariantTableModel model;
  QModelIndex index;
  QLineEdit le;
  /*
   * Setup
   */
  model.populate(1, 1);
  mapper.setModel(&model);
  mapper.addMapping(&le, 0);
  mapper.setCurrentRow(0);
  /*
   * Initial state
   */
  QVERIFY(le.isEnabled());
  /*
   * Unset enabled flag
   */
  // Line edit
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  model.setItemEnabled(index, false);
  mapper.setCurrentRow(0);
  QVERIFY(!le.isEnabled());
  /*
   * Set enabled flag again
   */
  // Line edit
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  model.setItemEnabled(index, true);
  mapper.setCurrentRow(0);
  QVERIFY(le.isEnabled());
}

void DataWidgetMapperTest::itemEnabledEditableFlagTest()
{
  DataWidgetMapper mapper;
  VariantTableModel model;
  QModelIndex index;
  QLineEdit le;
  QWidget w;
  /*
   * Setup
   */
  model.populate(1, 2);
  mapper.setModel(&model);
  mapper.addMapping(&le, 0);
  mapper.addMapping(&w, 1);
  mapper.setCurrentRow(0);
  /*
   * Initial state
   */
  QVERIFY(le.isEnabled());
  QVERIFY(!le.isReadOnly());
  QVERIFY(w.isEnabled());
  /*
   * Unset editable flag
   */
  // Line edit
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  model.setItemEditable(index, false);
  mapper.setCurrentRow(0);
  QVERIFY(le.isEnabled());
  QVERIFY(le.isReadOnly());
  // Widget without editable property
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  model.setItemEditable(index, false);
  mapper.setCurrentRow(0);
  QVERIFY(!w.isEnabled());
  /*
   * Unset enabled flag
   */
  // Line edit
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  model.setItemEnabled(index, false);
  mapper.setCurrentRow(0);
  QVERIFY(!le.isEnabled());
  // Widget without editable property
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  model.setItemEnabled(index, false);
  mapper.setCurrentRow(0);
  QVERIFY(!w.isEnabled());
  /*
   * Set editable flag
   */
  // Line edit
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  model.setItemEditable(index, true);
  mapper.setCurrentRow(0);
  QVERIFY(!le.isEnabled());
  // Widget without editable property
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  model.setItemEditable(index, true);
  mapper.setCurrentRow(0);
  QVERIFY(!w.isEnabled());
  /*
   * Set enabled flag
   */
  // Line edit
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  model.setItemEnabled(index, true);
  mapper.setCurrentRow(0);
  QVERIFY(le.isEnabled());
  QVERIFY(!le.isReadOnly());
  // Widget without editable property
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  model.setItemEnabled(index, true);
  mapper.setCurrentRow(0);
  QVERIFY(w.isEnabled());
}

void DataWidgetMapperTest::formatRoleTest()
{
  DataWidgetMapper mapper;
  VariantTableModel model;
  FormatProxyModel proxyModel;
  QLineEdit editor0;
  /*
   * Setup
   */
  model.populate(1, 1);
  proxyModel.setSourceModel(&model);
  mapper.setModel(&proxyModel);
  mapper.addMapping(&editor0, 0);
  mapper.setCurrentRow(0);
  /*
   * Initial state
   */
  QCOMPARE(editor0.alignment(), Qt::Alignment(Qt::AlignLeft | Qt::AlignVCenter));
  QVERIFY(editor0.styleSheet().isEmpty());
  /*
   * Set formats
   */
  proxyModel.setTextAlignmentForColumn(0, Qt::AlignCenter);
  proxyModel.setTextFontForColumn(0, QFont("Helvetica", 14));
  proxyModel.setTextColorForColumn(0, QColor(0,255,0));
  proxyModel.setBackgroundColorForColumn(0, QColor(255,0,0));
  QCOMPARE(editor0.alignment(), Qt::AlignCenter);
  QVERIFY(editor0.styleSheet().contains("font-size:14pt"));
  QVERIFY(editor0.styleSheet().contains("color:#00ff00"));
  QVERIFY(editor0.styleSheet().contains("background-color:#ff0000"));
  /*
   * Clear formats
   */
  proxyModel.clearTextAlignmentForColumn(0);
  proxyModel.clearTextFontForColumn(0);
  proxyModel.clearTextColorForColumn(0);
  proxyModel.clearBackgroundBrushForColumn(0);
  QCOMPARE(editor0.alignment(), Qt::Alignment(Qt::AlignLeft | Qt::AlignVCenter));
  QVERIFY(editor0.styleSheet().isEmpty());
}

void DataWidgetMapperTest::editionDoneTest()
{
  DataWidgetMapper mapper;
  VariantTableModel model;
  QLineEdit editor0;
  QLineEdit editor1;
  QSignalSpy editDoneSpy(&mapper, &DataWidgetMapper::dataEditionDone);
  QVERIFY(editDoneSpy.isValid());
  /*
   * Setup model and mapper
   */
  model.resize(1, 2);
  mapper.setModel(&model);
  mapper.addMapping(&editor0, 0);
  mapper.addMapping(&editor1, 1);
  mapper.setCurrentRow(0);
  QCOMPARE(mapper.currentRow(), 0);
  /*
   * Start editing then submit from data widget mapper
   */
  editDoneSpy.clear();
  editor0.clear();
  editor1.clear();
  QTest::keyClick(&editor0, 'A');
  QTest::keyClick(&editor1, '1');
  QVERIFY(mapper.setDataToModel());
  QCOMPARE(editDoneSpy.count(), 1);
  QCOMPARE(getModelData(model, 0, 0), QVariant("A"));
  QCOMPARE(getModelData(model, 0, 1), QVariant("1"));
  /*
   * Start editing then change data directly in model
   *
   * DataWidgetMapper cannot do anything when data changed from model,
   * it must simply display the new content and discard editing mode.
   * (Bug discovered 20170315)
   */
  editDoneSpy.clear();
  editor0.clear();
  QTest::keyClick(&editor0, 'B');
  QVERIFY(setModelData(model, 0, 0, "C"));
  QCOMPARE(editDoneSpy.count(), 1);
  QCOMPARE(getModelData(model, 0, 0), QVariant("C"));
  QCOMPARE(editor0.text(), QString("C"));
}

// void DataWidgetMapperTest::setModelTest()
// {
//   DataWidgetMapper mapper;
//   VariantTableModel model;
//   QModelIndex index;
//   QLineEdit edit0, edit1;
// 
//   QSignalSpy rowChangedSpy(&mapper, &DataWidgetMapper::currentRowChanged);
//   QVERIFY(rowChangedSpy.isValid());
//   QSignalSpy editStartedSpy(&mapper, &DataWidgetMapper::dataEditionStarted);
//   QVERIFY(editStartedSpy.isValid());
//   /*
//    * Initial state
//    */
//   QVERIFY(mapper.model() == nullptr);
//   QVERIFY(mapper.itemDelegate() != nullptr);
//   QCOMPARE(mapper.currentRow(), -1);
//   QCOMPARE(rowChangedSpy.count(), 0);
//   QCOMPARE(editStartedSpy.count(), 0);
//   /*
//    * Set a empty model
//    */
//   QCOMPARE(model.rowCount(), 0);
//   mapper.setModel(&model);
//   QVERIFY(mapper.model() == &model);
//   QCOMPARE(mapper.currentRow(), -1);
//   QCOMPARE(rowChangedSpy.count(), 0);
//   QCOMPARE(editStartedSpy.count(), 0);
//   /*
//    * Map widgets
//    */
//   mapper.addMapping(&edit0, 0);
//   mapper.addMapping(&edit1, 1);
//   QVERIFY(!edit0.isEnabled());
//   QVERIFY(!edit1.isEnabled());
//   QVERIFY(edit0.text().isEmpty());
//   QVERIFY(edit1.text().isEmpty());
//   /*
//    * Populate model with 1 column
//    *
//    * Note: widget mapper allways goes to row -1 after a model reset.
//    *       The controller then set a appropriate current row.
//    */
//   model.populate(3, 1);
//   QCOMPARE(mapper.currentRow(), -1);
//   QCOMPARE(rowChangedSpy.count(), 0);
//   QCOMPARE(editStartedSpy.count(), 0);
//   QVERIFY(!edit0.isEnabled());
//   QVERIFY(!edit1.isEnabled());
//   QVERIFY(edit0.text().isEmpty());
//   QVERIFY(edit1.text().isEmpty());
//   // Now, change to first row
//   mapper.setCurrentRow(0);
//   QCOMPARE(mapper.currentRow(), 0);
//   // Check that current row changed was signaled
//   QCOMPARE(rowChangedSpy.count(), 1);
//   QCOMPARE(rowChangedSpy.takeFirst().at(0).toInt(), 0);
//   // Check that no edition started was signaled
//   QCOMPARE(editStartedSpy.count(), 0);
//   // Check editors state
//   QVERIFY(edit0.isEnabled());
//   QVERIFY(!edit1.isEnabled());
//   index = model.index(0, 0);
//   QVERIFY(index.isValid());
//   QCOMPARE(edit0.text(), model.data(index).toString());
//   QVERIFY(edit1.text().isEmpty());
//   /*
//    * Populate model with 2 columns
//    *
//    * Note: widget mapper allways goes to row -1 after a model reset.
//    *       The controller then set a appropriate current row.
//    */
//   model.populate(3, 2);
//   QCOMPARE(mapper.currentRow(), -1);
//   QCOMPARE(rowChangedSpy.count(), 1);
//   QCOMPARE(rowChangedSpy.takeFirst().at(0).toInt(), -1);
//   QCOMPARE(editStartedSpy.count(), 0);
//   // Go to first row
//   mapper.setCurrentRow(0);
//   // Check that current row changed was signaled
//   QCOMPARE(rowChangedSpy.count(), 1);
//   QCOMPARE(rowChangedSpy.takeFirst().at(0).toInt(), 0);
//   // Check that no edition started was signaled
//   QCOMPARE(editStartedSpy.count(), 0);
//   // Check that editors state was updated
//   QVERIFY(edit0.isEnabled());
//   QVERIFY(edit1.isEnabled());
//   index = model.index(0, 0);
//   QVERIFY(index.isValid());
//   QCOMPARE(edit0.text(), model.data(index).toString());
//   index = model.index(0, 1);
//   QVERIFY(index.isValid());
//   QCOMPARE(edit1.text(), model.data(index).toString());
//   /*
//    * Clear mapping
//    */
//   mapper.clearMapping();
//   QVERIFY(edit0.isEnabled());
//   QVERIFY(edit1.isEnabled());
//   QVERIFY(edit0.text().isEmpty());
//   QVERIFY(edit1.text().isEmpty());
//   /*
//    * Set allready populated model and map widgets once we are at first row
//    *
//    * Widget mapper must go to row -1 after model was set.
//    * The controller will tell it which row to go later.
//    */
//   VariantTableModel model2;
//   model2.populate(4, 2);
//   mapper.setModel(&model2);
//   QCOMPARE(mapper.currentRow(), -1);
//   // Check that current row changed was signaled
//   QCOMPARE(rowChangedSpy.count(), 1);
//   QCOMPARE(rowChangedSpy.takeFirst().at(0).toInt(), -1);
//   // Check that no edition started was signaled
//   QCOMPARE(editStartedSpy.count(), 0);
//   // Go to first row
//   mapper.setCurrentRow(0);
//   QCOMPARE(mapper.currentRow(), 0);
//   // Check that current row changed was signaled
//   QCOMPARE(rowChangedSpy.count(), 1);
//   QCOMPARE(rowChangedSpy.takeFirst().at(0).toInt(), 0);
//   // Check that no edition started was signaled
//   QCOMPARE(editStartedSpy.count(), 0);
//   // Map widgets
//   mapper.addMapping(&edit0, 0);
//   mapper.addMapping(&edit1, 1);
//   // Check editors state
//   QVERIFY(edit0.isEnabled());
//   QVERIFY(edit1.isEnabled());
//   index = model2.index(0, 0);
//   QVERIFY(index.isValid());
//   QCOMPARE(edit0.text(), model2.data(index).toString());
//   index = model2.index(0, 1);
//   QVERIFY(index.isValid());
//   QCOMPARE(edit1.text(), model2.data(index).toString());
// }

void DataWidgetMapperTest::setCurrentRowWidgetTest()
{
  DataWidgetMapper mapper;
  VariantTableModel model;
  QLineEdit edit0, edit1;
  /*
   * Setup
   */
  mapper.addMapping(&edit0, 0);
  mapper.addMapping(&edit1, 1);
  /*
   * Try to change current row without model set
   */
  QCOMPARE(mapper.currentRow(), -1);
  mapper.setCurrentRow(0);
  QCOMPARE(mapper.currentRow(), -1);
  /*
   * Set model
   */
  model.resize(3, 2);
  model.populateColumn(0, {1,2,3});
  model.populateColumn(1, {"A","B","C"});
  mapper.setModel(&model);
  /*
   * Change current row
   */
  mapper.setCurrentRow(0);
  QCOMPARE(mapper.currentRow(), 0);
  // Check editors state
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  QCOMPARE(edit0.text(), QString("1"));
  QCOMPARE(edit1.text(), QString("A"));
  /*
   * Change current row to a invalid one
   */
  mapper.setCurrentRow(-1);
  QCOMPARE(mapper.currentRow(), -1);
  // Check editors state
  QVERIFY(!edit0.isEnabled());
  QVERIFY(!edit1.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.text().isEmpty());
  /*
   * Change current row
   */
  mapper.setCurrentRow(1);
  QCOMPARE(mapper.currentRow(), 1);
  // Check editors state
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  QCOMPARE(edit0.text(), QString("2"));
  QCOMPARE(edit1.text(), QString("B"));
}

void DataWidgetMapperTest::setCurrentRowSignalTest()
{
  DataWidgetMapper mapper;
  VariantTableModel model;
  QModelIndex index;
  QLineEdit edit0, edit1;

  QSignalSpy rowChangedSpy(&mapper, &DataWidgetMapper::currentRowChanged);
  QVERIFY(rowChangedSpy.isValid());
  QSignalSpy editStartedSpy(&mapper, &DataWidgetMapper::dataEditionStarted);
  QVERIFY(editStartedSpy.isValid());
  /*
   * Setup
   */
  model.populate(3, 2);
  mapper.setModel(&model);
  mapper.addMapping(&edit0, 0);
  mapper.addMapping(&edit1, 1);
  rowChangedSpy.clear();
  editStartedSpy.clear();
  /*
   * Change current row
   */
  mapper.setCurrentRow(1);
  QCOMPARE(mapper.currentRow(), 1);
  // Check that current row changed was signaled
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(rowChangedSpy.takeFirst().at(0).toInt(), 1);
  // Check that no edition started was signaled
  QCOMPARE(editStartedSpy.count(), 0);
  /*
   * Change to a invalid row
   */
  mapper.setCurrentRow(-1);
  QCOMPARE(mapper.currentRow(), -1);
  // Check that current row changed was signaled
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(rowChangedSpy.takeFirst().at(0).toInt(), -1);
  // Check that no edition started was signaled
  QCOMPARE(editStartedSpy.count(), 0);
  /*
   * Change current row
   */
  mapper.setCurrentRow(0);
  QCOMPARE(mapper.currentRow(), 0);
  // Check that current row changed was signaled
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(rowChangedSpy.takeFirst().at(0).toInt(), 0);
  // Check that no edition started was signaled
  QCOMPARE(editStartedSpy.count(), 0);
}

void DataWidgetMapperTest::insertWidgetTest()
{
  DataWidgetMapper mapper;
  VariantTableModel model;
  QLineEdit edit0, edit1;
  /*
   * Setup
   */
  model.resize(0, 2);
  mapper.setModel(&model);
  mapper.addMapping(&edit0, 0);
  mapper.addMapping(&edit1, 1);
  // Check editors state
  QVERIFY(!edit0.isEnabled());
  QVERIFY(!edit1.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.text().isEmpty());
  /*
   * Insert at end
   */
  model.appendRow();
  mapper.setCurrentRow(0);
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  edit0.setText("2");
  edit1.setText("B");
  QVERIFY(mapper.setDataToModel());
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.data(0, 0), QVariant("2"));
  QCOMPARE(model.data(0, 1), QVariant("B"));
  /*
   * Insert at beginning
   * Note:
   *  when a controller inserts a row, it will allways call setCurrentRow().
   *  Widget mapper must also repopulate its widgets from model,
   *  eben if current row did not change.
   */
  model.prependRow();
  mapper.setCurrentRow(0);
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.text().isEmpty());
  edit0.setText("1");
  edit1.setText("A");
  QVERIFY(mapper.setDataToModel());
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(model.data(0, 0), QVariant("1"));
  QCOMPARE(model.data(0, 1), QVariant("A"));
  QCOMPARE(model.data(1, 0), QVariant("2"));
  QCOMPARE(model.data(1, 1), QVariant("B"));
}

void DataWidgetMapperTest::insertSignalTest()
{
  DataWidgetMapper mapper;
  VariantTableModel model;
  QLineEdit edit0, edit1;
  QSignalSpy rowChangedSpy(&mapper, &DataWidgetMapper::currentRowChanged);
  QVERIFY(rowChangedSpy.isValid());
  QSignalSpy editStartedSpy(&mapper, &DataWidgetMapper::dataEditionStarted);
  QVERIFY(editStartedSpy.isValid());
  /*
   * Setup
   */
  model.resize(0, 2);
  mapper.setModel(&model);
  mapper.addMapping(&edit0, 0);
  mapper.addMapping(&edit1, 1);
  rowChangedSpy.clear();
  editStartedSpy.clear();
  QCOMPARE(mapper.currentRow(), -1);
  /*
   * Insert at end
   */
  model.appendRow();
  mapper.setCurrentRow(0);
  // Check that current row changed was signaled
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(rowChangedSpy.takeFirst().at(0).toInt(), 0);
  // Check that no edition started was signaled
  QCOMPARE(editStartedSpy.count(), 0);
  /*
   * Insert at beginning
   */
  model.prependRow();
  mapper.setCurrentRow(0);
  // Check that current row changed was not signaled
  QCOMPARE(rowChangedSpy.count(), 0);
  // Check that no edition started was signaled
  QCOMPARE(editStartedSpy.count(), 0);
}

void DataWidgetMapperTest::insertFromModelWidgetTest()
{
  DataWidgetMapper mapper;
  VariantTableModel model;
  QLineEdit edit0, edit1;
  /*
   * Setup
   */
  model.resize(1, 2);
  model.populateColumn(0, {0});
  model.populateColumn(1, {"_"});
  mapper.setModel(&model);
  mapper.addMapping(&edit0, 0);
  mapper.addMapping(&edit1, 1);
  mapper.setCurrentRow(0);
  QCOMPARE(mapper.currentRow(), 0);
  // Check editors state
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  QCOMPARE(edit0.text(), QString("0"));
  QCOMPARE(edit1.text(), QString("_"));
  /*
   * Begin editing
   */
  edit0.clear();
  edit1.clear();
  QTest::keyClick(&edit0, Qt::Key_1);
  QTest::keyClick(&edit1, 'A');
  /*
   * Insert a row before current row
   * Then, go to next row, which contains row that we are editing.
   * (Keeping current row coherent during insertion is done by the controller)
   */
  model.prependRow();
  mapper.setCurrentRow(1);
  QCOMPARE(mapper.currentRow(), 1);
  QCOMPARE(edit0.text(), QString("1"));
  QCOMPARE(edit1.text(), QString("A"));
  /*
   * Submit
   */
  QVERIFY(mapper.setDataToModel());
  QCOMPARE(mapper.currentRow(), 1);
  QCOMPARE(edit0.text(), QString("1"));
  QCOMPARE(edit1.text(), QString("A"));
  QCOMPARE(model.data(1, 0), QVariant(1));
  QCOMPARE(model.data(1, 1), QVariant("A"));
}

template<typename Editor0, typename Editor1>
void editStartDoneSignalTestImpl(Editor0 & editor0, Editor1 & editor1)
{
  DataWidgetMapper mapper;
  VariantTableModel model;
  QModelIndex index;

  QSignalSpy editStartedSpy(&mapper, &DataWidgetMapper::dataEditionStarted);
  QVERIFY(editStartedSpy.isValid());
  QSignalSpy editDoneSpy(&mapper, &DataWidgetMapper::dataEditionDone);
  QVERIFY(editDoneSpy.isValid());

  /*
   * Setup
   */
  model.populate(3, 2);
  mapper.setModel(&model);
  mapper.addMapping(&editor0, 0);
  mapper.addMapping(&editor1, 1);
  mapper.setCurrentRow(0);
  editStartedSpy.clear();
  editDoneSpy.clear();

  /*
   * Start edit with key on editor 0
   */
  qDebug() << "TEST: edit 0 ..";
//   editor0.show();
//   QTest::qWait(20000);
  
  QTest::keyClick(&editor0, Qt::Key_1);
  QCOMPARE(editStartedSpy.count(), 1);
  editStartedSpy.clear();
  QCOMPARE(editDoneSpy.count(), 0);
  // Continue typing must not emit signal again
  QTest::keyClick(&editor0, Qt::Key_1);
  QCOMPARE(editStartedSpy.count(), 0);
  QCOMPARE(editDoneSpy.count(), 0);
  // Editing on other widget must also not signal anymore
  QTest::keyClick(&editor1, Qt::Key_1);
  QCOMPARE(editStartedSpy.count(), 0);
  QCOMPARE(editDoneSpy.count(), 0);
  /*
   * Submit
   */
  QVERIFY(mapper.setDataToModel());
  QCOMPARE(editStartedSpy.count(), 0);
  QCOMPARE(editDoneSpy.count(), 1);
  editDoneSpy.clear();
  // After a call of submit, start edit must be signaled again
  QTest::keyClick(&editor0, Qt::Key_1);
  QCOMPARE(editStartedSpy.count(), 1);
  editStartedSpy.clear();
  QCOMPARE(editDoneSpy.count(), 0);
  QTest::keyClick(&editor0, Qt::Key_1);
  QCOMPARE(editStartedSpy.count(), 0);
  QCOMPARE(editDoneSpy.count(), 0);
  /*
   * Revert
   */
  mapper.revertDataFromModel();
  QCOMPARE(editStartedSpy.count(), 0);
  QCOMPARE(editDoneSpy.count(), 1);
  editDoneSpy.clear();
  // After a call of revert, start edit must be signaled again
  QTest::keyClick(&editor0, Qt::Key_1);
  QCOMPARE(editStartedSpy.count(), 1);
  editStartedSpy.clear();
  QCOMPARE(editDoneSpy.count(), 0);
  QTest::keyClick(&editor0, Qt::Key_1);
  QCOMPARE(editStartedSpy.count(), 0);
  QCOMPARE(editDoneSpy.count(), 0);
  /*
   * Submit (for next test)
   */
  QVERIFY(mapper.setDataToModel());
  QCOMPARE(editStartedSpy.count(), 0);
  QCOMPARE(editDoneSpy.count(), 1);
  editDoneSpy.clear();
  /*
   * Start edit with key on editor 1
   */
  QTest::keyClick(&editor1, Qt::Key_1);
  QCOMPARE(editStartedSpy.count(), 1);
  editStartedSpy.clear();
  QCOMPARE(editDoneSpy.count(), 0);
  // Continue typing must not emit signal again
  QTest::keyClick(&editor1, Qt::Key_1);
  QCOMPARE(editStartedSpy.count(), 0);
  QCOMPARE(editDoneSpy.count(), 0);
  // Editing on other widget must also not signal anymore
  QTest::keyClick(&editor0, Qt::Key_1);
  QCOMPARE(editStartedSpy.count(), 0);
  QCOMPARE(editDoneSpy.count(), 0);
}

void DataWidgetMapperTest::editStartDoneSignalQLineEditTest()
{
  QLineEdit le0, le1;
  editStartDoneSignalTestImpl(le0, le1);
}

void DataWidgetMapperTest::editStartDoneSignalQSpinBoxTest()
{
  QSpinBox sb0;
  sb0.setMaximum(10000);
  QSpinBox sb1;
  sb1.setMaximum(10000);
  editStartDoneSignalTestImpl(sb0, sb1);
}

void DataWidgetMapperTest::editStartDoneSignalQComboBoxTest()
{
  QComboBox cb0;

  qDebug() << "cb0 editable (1): " << cb0.isEditable();
  
//   cb0.move(200, 200);
//   cb0.resize(500, 200);
//   cb0.setGeometry(0, 0, 100, 100);
//   cb0.show();
//   QTest::qWait(2000);
  
  cb0.addItem("1");
  
  cb0.addItem("11");
  cb0.addItem("1111");
  cb0.addItem("11111");
  
//   QTest::qWait(1000);
  cb0.setEditable(false);
//   cb0.setEditable(true);
  qDebug() << "cb0 editable (2): " << cb0.isEditable();
  QComboBox cb1;
  cb1.addItem("1");
  cb1.addItem("11");
  cb1.addItem("1111");
  cb1.addItem("11111");
  editStartDoneSignalTestImpl(cb0, cb1);
}

void DataWidgetMapperTest::editStartDoneSignalQPlainTextEditTest()
{
  QPlainTextEdit pte0, pte1;
  editStartDoneSignalTestImpl(pte0, pte1);
}

void DataWidgetMapperTest::editStartDoneSignalQTextEditTest()
{
  QTextEdit te0, te1;
  editStartDoneSignalTestImpl(te0, te1);
}

// void DataWidgetMapperTest::editStartDoneSignalTest()
// {
//   DataWidgetMapper mapper;
//   VariantTableModel model;
//   QModelIndex index;
//   QFETCH(QWidget*, editor0);
//   QFETCH(QWidget*, editor1);
// 
//   QScopedPointer<QWidget> editor0Gard(editor0);
//   QScopedPointer<QWidget> editor1Gard(editor1);
//   QSignalSpy editStartedSpy(&mapper, &DataWidgetMapper::dataEditionStarted);
//   QVERIFY(editStartedSpy.isValid());
//   QSignalSpy editDoneSpy(&mapper, &DataWidgetMapper::dataEditionDone);
//   QVERIFY(editDoneSpy.isValid());
// 
//   /*
//    * Setup
//    */
//   model.populate(3, 2);
//   mapper.setModel(&model);
//   mapper.addMapping(editor0, 0);
//   mapper.addMapping(editor1, 1);
//   mapper.setCurrentRow(0);
//   editStartedSpy.clear();
//   editDoneSpy.clear();
// 
//   /*
//    * Start edit with key on editor 0
//    */
//   QTest::keyClick(editor0, Qt::Key_1);
//   QCOMPARE(editStartedSpy.count(), 1);
//   editStartedSpy.clear();
//   QCOMPARE(editDoneSpy.count(), 0);
//   // Continue typing must not emit signal again
//   QTest::keyClick(editor0, Qt::Key_1);
//   QCOMPARE(editStartedSpy.count(), 0);
//   QCOMPARE(editDoneSpy.count(), 0);
//   // Editing on other widget must also not signal anymore
//   QTest::keyClick(editor1, Qt::Key_1);
//   QCOMPARE(editStartedSpy.count(), 0);
//   QCOMPARE(editDoneSpy.count(), 0);
//   /*
//    * Submit
//    */
//   QVERIFY(mapper.setDataToModel());
//   QCOMPARE(editStartedSpy.count(), 0);
//   QCOMPARE(editDoneSpy.count(), 1);
//   editDoneSpy.clear();
//   // After a call of submit, start edit must be signaled again
//   QTest::keyClick(editor0, Qt::Key_1);
//   QCOMPARE(editStartedSpy.count(), 1);
//   editStartedSpy.clear();
//   QCOMPARE(editDoneSpy.count(), 0);
//   QTest::keyClick(editor0, Qt::Key_1);
//   QCOMPARE(editStartedSpy.count(), 0);
//   QCOMPARE(editDoneSpy.count(), 0);
//   /*
//    * Revert
//    */
//   mapper.revertDataFromModel();
//   QCOMPARE(editStartedSpy.count(), 0);
//   QCOMPARE(editDoneSpy.count(), 1);
//   editDoneSpy.clear();
//   // After a call of revert, start edit must be signaled again
//   QTest::keyClick(editor0, Qt::Key_1);
//   QCOMPARE(editStartedSpy.count(), 1);
//   editStartedSpy.clear();
//   QCOMPARE(editDoneSpy.count(), 0);
//   QTest::keyClick(editor0, Qt::Key_1);
//   QCOMPARE(editStartedSpy.count(), 0);
//   QCOMPARE(editDoneSpy.count(), 0);
//   /*
//    * Submit (for next test)
//    */
//   QVERIFY(mapper.setDataToModel());
//   QCOMPARE(editStartedSpy.count(), 0);
//   QCOMPARE(editDoneSpy.count(), 1);
//   editDoneSpy.clear();
//   /*
//    * Start edit with key on editor 1
//    */
//   QTest::keyClick(editor1, Qt::Key_1);
//   QCOMPARE(editStartedSpy.count(), 1);
//   editStartedSpy.clear();
//   QCOMPARE(editDoneSpy.count(), 0);
//   // Continue typing must not emit signal again
//   QTest::keyClick(editor1, Qt::Key_1);
//   QCOMPARE(editStartedSpy.count(), 0);
//   QCOMPARE(editDoneSpy.count(), 0);
//   // Editing on other widget must also not signal anymore
//   QTest::keyClick(editor0, Qt::Key_1);
//   QCOMPARE(editStartedSpy.count(), 0);
//   QCOMPARE(editDoneSpy.count(), 0);
// }

// void DataWidgetMapperTest::editStartDoneSignalTest_data()
// {
//   QTest::addColumn<QWidget*>("editor0");
//   QTest::addColumn<QWidget*>("editor1");
// 
//   QWidget *widget0, *widget1;
// 
//   widget0 = new QLineEdit;
//   widget1 = new QLineEdit;
//   QTest::newRow("QLineEdit") << widget0 << widget1;
// 
//   QSpinBox *sb;
//   sb = new QSpinBox;
//   sb->setMaximum(10000);
//   widget0 = sb;
//   sb = new QSpinBox;
//   sb->setMaximum(10000);
//   widget1 = sb;
//   QTest::newRow("QSpinBox") << widget0 << widget1;
// 
//   QComboBox *cb;
//   cb = new QComboBox;
//   cb->addItem("1");
//   cb->addItem("11");
//   cb->addItem("1111");
//   cb->addItem("11111");
//   widget0 = cb;
//   cb = new QComboBox;
//   cb->addItem("1");
//   cb->addItem("11");
//   cb->addItem("1111");
//   cb->addItem("11111");
//   widget1 = cb;
//   QTest::newRow("QComboBox") << widget0 << widget1;
// 
//   widget0 = new QPlainTextEdit;
//   widget1 = new QPlainTextEdit;
//   QTest::newRow("QPlainTextEdit") << widget0 << widget1;
// 
//   widget0 = new QTextEdit;
//   widget1 = new QTextEdit;
//   QTest::newRow("QTextEdit") << widget0 << widget1;
// }

void DataWidgetMapperTest::setDataFromModelQLineEditTest()
{
  using Mdt::ItemEditor::DataWidgetMapper;

  DataWidgetMapper mapper;
  VariantTableModel model;
  QModelIndex index;
  QLineEdit editor0;
  QLineEdit editor1;

  /*
   * Setup
   */
  model.populate(3, 2);
  mapper.setModel(&model);
  mapper.addMapping(&editor0, 0);
  mapper.addMapping(&editor1, 1);
  mapper.setCurrentRow(0);
  /*
   * Update model data for mapper's current row
   */
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QVERIFY(model.setData(index, "ABCD"));
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  QVERIFY(model.setData(index, "1234"));
  /*
   * Check that editors are updated
   */
  QCOMPARE(editor0.text(), QString("ABCD"));
  QCOMPARE(editor1.text(), QString("1234"));
  /*
   * Update model data for a other row
   */
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  QVERIFY(model.setData(index, "EFGH"));
  index = model.index(1, 1);
  QVERIFY(index.isValid());
  QVERIFY(model.setData(index, "5678"));
  /*
   * Check that editors still display the data of row 0
   */
  QCOMPARE(editor0.text(), QString("ABCD"));
  QCOMPARE(editor1.text(), QString("1234"));
}

void DataWidgetMapperTest::submitDataQLineEditTest()
{
  using Mdt::ItemEditor::DataWidgetMapper;

  DataWidgetMapper mapper;
  VariantTableModel model;
  QModelIndex index;
  QLineEdit editor0;
  QLineEdit editor1;

  /*
   * Setup with a 2 column model
   */
  model.populate(3, 2);
  mapper.setModel(&model);
  mapper.addMapping(&editor0, 0);
  mapper.addMapping(&editor1, 1);
  mapper.setCurrentRow(0);
  /*
   * Edit
   */
  editor0.setText("ABCD");
  editor1.setText("1234");
  /*
   * Submit and check
   */
  QVERIFY(mapper.setDataToModel());
  QCOMPARE(getModelData(model, 0, 0), QVariant("ABCD"));
  QCOMPARE(getModelData(model, 0, 1), QVariant("1234"));
  /*
   * Populate model with 1 column
   */
  model.populate(3, 1);
  mapper.setCurrentRow(0);
  QVERIFY(!editor1.isEnabled());
  /*
   * Edit
   */
  editor0.setText("EFGH");
  /*
   * Submit and check
   */
  QVERIFY(mapper.setDataToModel());
  QCOMPARE(getModelData(model, 0, 0), QVariant("EFGH"));
}

void DataWidgetMapperTest::revertDataQLineEditTest()
{
  DataWidgetMapper mapper;
  VariantTableModel model;
  QModelIndex index;
  QLineEdit editor0;
  QLineEdit editor1;
  QString originalText0;
  QString originalText1;

  /*
   * Setup
   */
  model.populate(3, 2);
  mapper.setModel(&model);
  mapper.addMapping(&editor0, 0);
  mapper.addMapping(&editor1, 1);
  mapper.setCurrentRow(0);
  // get original texts
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  originalText0 = index.data().toString();
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  originalText1 = index.data().toString();
  /*
   * Edit
   */
  editor0.setText("ABCD");
  editor1.setText("1234");
  /*
   * Revert and check
   */
  mapper.revertDataFromModel();
  // Check mapped widgets
  QCOMPARE(editor0.text(), originalText0);
  QCOMPARE(editor1.text(), originalText1);
  // Check model
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(index.data().toString(), originalText0);
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(index.data().toString(), originalText1);
}

void DataWidgetMapperTest::editStartDoneSignalTestReadOnlyQComboBoxTest()
{
  QModelIndex index;

  DataWidgetMapper mapper;
  VariantTableModel model;
  model.populate(3, 2);
  mapper.setModel(&model);

  QComboBox cb;
  QStringListModel cbModel;
  cb.setModel(&cbModel);
  mapper.addMapping(&cb, 0);

  QSignalSpy editStartedSpy(&mapper, &DataWidgetMapper::dataEditionStarted);
  QVERIFY(editStartedSpy.isValid());
  QSignalSpy editDoneSpy(&mapper, &DataWidgetMapper::dataEditionDone);
  QVERIFY(editDoneSpy.isValid());

  QCOMPARE(editStartedSpy.count(), 0);
  QCOMPARE(editDoneSpy.count(), 0);

  mapper.setCurrentRow(0);
  QCOMPARE(editStartedSpy.count(), 0);
  QCOMPARE(editDoneSpy.count(), 0);

  appendRowToModel(cbModel);
  setModelData(cbModel, 0, 0, "A");
  QCOMPARE(editStartedSpy.count(), 0);
  QCOMPARE(editDoneSpy.count(), 0);

  cbModel.setStringList({"D","E"});
  QCOMPARE(editStartedSpy.count(), 0);
  QCOMPARE(editDoneSpy.count(), 0);

  mouseSelectRow(cb, 1);
  QCOMPARE(editStartedSpy.count(), 1);
  QCOMPARE(editDoneSpy.count(), 0);

  mapper.setDataToModel();
  QCOMPARE(getModelData(model, 0, 0), QVariant("E"));
  QCOMPARE(editStartedSpy.count(), 1);
  QCOMPARE(editDoneSpy.count(), 1);

  editStartedSpy.clear();
  editDoneSpy.clear();
}

// void DataWidgetMapperTest::QComboBoxSandbox()
// {
// //   DataWidgetMapper mapper;
//   
//   QComboBox cb;
//   
//   cb.setEditable(true);
// 
//   const auto outTextActivated = [](const QString & text){
//     qDebug() << "activated(): " << text;
//   };
//   connect( &cb, QOverload<const QString &>::of(&QComboBox::activated), outTextActivated );
// 
//   const auto outTextCurrentTextChanged = [](const QString & text){
//     qDebug() << "currentTextChanged(): " << text;
//   };
//   connect( &cb, QOverload<const QString &>::of(&QComboBox::currentTextChanged), outTextCurrentTextChanged );
// 
//   qDebug() << "Test: set model to cb";
//   QStringListModel model({"A","B","C"});
//   cb.setModel(&model);
//   
//   
//   qDebug() << "Test: set current text";
//   cb.setCurrentText("B");
//   
// //   cb.show();
// //   while(cb.isVisible()){
// //     QTest::qWait(500);
// //   }
//   
// 
//   QFAIL("SB");
// }

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  DataWidgetMapperTest test;

  return QTest::qExec(&test, argc, argv);
}
