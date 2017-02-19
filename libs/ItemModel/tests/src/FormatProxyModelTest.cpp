/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "FormatProxyModelTest.h"
#include "qtmodeltest.h"
#include "Mdt/ItemModel/FormatProxyModel.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include <QSignalSpy>
#include <QVariantList>
#include <QVector>

using namespace Mdt::ItemModel;

/*
 * Init/cleanup
 */

void FormatProxyModelTest::initTestCase()
{
}

void FormatProxyModelTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void FormatProxyModelTest::textAlignmentTest()
{
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(3, 2);
  model.populateColumn(0, {1,2,3});
  model.populateColumn(1, {"A","B","C"});
  /*
   * Setup proxy model
   */
  FormatProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  /*
   * Initial state
   */
  QCOMPARE(proxyModel.rowCount(), 3);
  // Check data for DisplayRole
  QCOMPARE(getModelData(proxyModel, 0, 0, Qt::DisplayRole), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 0, 1, Qt::DisplayRole), QVariant("A"));
  // Check getter
  QVERIFY(proxyModel.textAlignment(0, 0).isNull());
  QVERIFY(proxyModel.textAlignment(0, 1).isNull());
  // Check data for TextAlignmentRole
  QVERIFY(getModelData(proxyModel, 0, 0, Qt::TextAlignmentRole).isNull());
  QVERIFY(getModelData(proxyModel, 0, 1, Qt::TextAlignmentRole).isNull());
  /*
   * Setup text alignments:
   *                          AlignCenter
   *           ---------------------------
   *           | AlignRight |            |
   *           ---------------------------
   * AlignLeft |            |            |
   *           ---------------------------
   */
  proxyModel.setTextAlignmentForIndex(0, 0, Qt::AlignRight);
  proxyModel.setTextAlignmentForRow(1, Qt::AlignLeft);
  proxyModel.setTextAlignmentForColumn(1, Qt::AlignCenter);
  // Check data for DisplayRole
  QCOMPARE(getModelData(proxyModel, 0, 0, Qt::DisplayRole), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 0, 1, Qt::DisplayRole), QVariant("A"));
  QCOMPARE(getModelData(proxyModel, 1, 0, Qt::DisplayRole), QVariant(2));
  // Check getter
  QCOMPARE(proxyModel.textAlignment(0, 0), QVariant(Qt::AlignRight));
  QCOMPARE(proxyModel.textAlignment(0, 1), QVariant(Qt::AlignCenter));
  QCOMPARE(proxyModel.textAlignment(1, 0), QVariant(Qt::AlignLeft));
  // Check data for TextAlignmentRole
  QCOMPARE(getModelData(proxyModel, 0, 0, Qt::TextAlignmentRole), QVariant(Qt::AlignRight));
  QCOMPARE(getModelData(proxyModel, 0, 1, Qt::TextAlignmentRole), QVariant(Qt::AlignCenter));
  QCOMPARE(getModelData(proxyModel, 1, 0, Qt::TextAlignmentRole), QVariant(Qt::AlignLeft));
  /*
   * Clear text alignments:
   *
   *           ---------------------------
   *           |            |            |
   *           ---------------------------
   *           |            |            |
   *           ---------------------------
   */
  proxyModel.clearTextAlignmentForIndex(0, 0);
  proxyModel.clearTextAlignmentForRow(1);
  proxyModel.clearTextAlignmentForColumn(1);
  // Check data for DisplayRole
  QCOMPARE(getModelData(proxyModel, 0, 0, Qt::DisplayRole), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 0, 1, Qt::DisplayRole), QVariant("A"));
  QCOMPARE(getModelData(proxyModel, 1, 0, Qt::DisplayRole), QVariant(2));
  // Check getter
  QVERIFY(proxyModel.textAlignment(0, 0).isNull());
  QVERIFY(proxyModel.textAlignment(0, 1).isNull());
  QVERIFY(proxyModel.textAlignment(1, 0).isNull());
  // Check data for TextAlignmentRole
  QVERIFY(getModelData(proxyModel, 0, 0, Qt::TextAlignmentRole).isNull());
  QVERIFY(getModelData(proxyModel, 0, 1, Qt::TextAlignmentRole).isNull());
  QVERIFY(getModelData(proxyModel, 1, 0, Qt::TextAlignmentRole).isNull());
}

void FormatProxyModelTest::textAlignmentIndexSignalTest()
{
  QVariantList arguments;
  QModelIndex index;
  QVector<int> roles;
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(2, 2);
  model.populateColumn(0, {1,2});
  model.populateColumn(1, {"A","B"});
  /*
   * Setup proxy model
   */
  FormatProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  /*
   * Setup signal spy
   */
  QSignalSpy dataChangedSpy(&proxyModel, &FormatProxyModel::dataChanged);
  QVERIFY(dataChangedSpy.isValid());
  /*
   * Set alignment for index 0, 1
   */
  proxyModel.setTextAlignmentForIndex(0, 1, Qt::AlignCenter);
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  index = arguments.at(0).toModelIndex(); // topLeft
  QCOMPARE(index.row(), 0);
  QCOMPARE(index.column(), 1);
  index = arguments.at(1).toModelIndex(); // bottomRight
  QCOMPARE(index.row(), 0);
  QCOMPARE(index.column(), 1);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::TextAlignmentRole);
  /*
   * Clear alignment for index 0, 1
   */
  proxyModel.clearTextAlignmentForIndex(0, 1);
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  index = arguments.at(0).toModelIndex(); // topLeft
  QCOMPARE(index.row(), 0);
  QCOMPARE(index.column(), 1);
  index = arguments.at(1).toModelIndex(); // bottomRight
  QCOMPARE(index.row(), 0);
  QCOMPARE(index.column(), 1);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::TextAlignmentRole);
}

void FormatProxyModelTest::textAlignmentRowSignalTest()
{
  QVariantList arguments;
  QModelIndex index;
  QVector<int> roles;
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(2, 2);
  model.populateColumn(0, {1,2});
  model.populateColumn(1, {"A","B"});
  /*
   * Setup proxy model
   */
  FormatProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  /*
   * Setup signal spy
   */
  QSignalSpy dataChangedSpy(&proxyModel, &FormatProxyModel::dataChanged);
  QVERIFY(dataChangedSpy.isValid());
  /*
   * Set alignment for row 1
   */
  proxyModel.setTextAlignmentForRow(1, Qt::AlignCenter);
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  index = arguments.at(0).toModelIndex(); // topLeft
  QCOMPARE(index.row(), 1);
  QCOMPARE(index.column(), 0);
  index = arguments.at(1).toModelIndex(); // bottomRight
  QCOMPARE(index.row(), 1);
  QCOMPARE(index.column(), 1);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::TextAlignmentRole);
  /*
   * Clear alignment for row 1
   */
  proxyModel.clearTextAlignmentForRow(1);
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  index = arguments.at(0).toModelIndex(); // topLeft
  QCOMPARE(index.row(), 1);
  QCOMPARE(index.column(), 0);
  index = arguments.at(1).toModelIndex(); // bottomRight
  QCOMPARE(index.row(), 1);
  QCOMPARE(index.column(), 1);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::TextAlignmentRole);  
}

void FormatProxyModelTest::textAlignmentColumnSignalTest()
{
  QVariantList arguments;
  QModelIndex index;
  QVector<int> roles;
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(2, 2);
  model.populateColumn(0, {1,2});
  model.populateColumn(1, {"A","B"});
  /*
   * Setup proxy model
   */
  FormatProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  /*
   * Setup signal spy
   */
  QSignalSpy dataChangedSpy(&proxyModel, &FormatProxyModel::dataChanged);
  QVERIFY(dataChangedSpy.isValid());
  /*
   * Set alignment for column 1
   */
  proxyModel.setTextAlignmentForColumn(1, Qt::AlignCenter);
  QCOMPARE(dataChangedSpy.count(), 2);
  // Check for row 0
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  index = arguments.at(0).toModelIndex(); // topLeft
  QCOMPARE(index.row(), 0);
  QCOMPARE(index.column(), 1);
  index = arguments.at(1).toModelIndex(); // bottomRight
  QCOMPARE(index.row(), 0);
  QCOMPARE(index.column(), 1);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::TextAlignmentRole);
  // Check for row 1
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  index = arguments.at(0).toModelIndex(); // topLeft
  QCOMPARE(index.row(), 1);
  QCOMPARE(index.column(), 1);
  index = arguments.at(1).toModelIndex(); // bottomRight
  QCOMPARE(index.row(), 1);
  QCOMPARE(index.column(), 1);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::TextAlignmentRole);
  /*
   * Clear alignment of column 1
   */
  proxyModel.clearTextAlignmentForColumn(1);
  QCOMPARE(dataChangedSpy.count(), 2);
  // Check for row 0
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  index = arguments.at(0).toModelIndex(); // topLeft
  QCOMPARE(index.row(), 0);
  QCOMPARE(index.column(), 1);
  index = arguments.at(1).toModelIndex(); // bottomRight
  QCOMPARE(index.row(), 0);
  QCOMPARE(index.column(), 1);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::TextAlignmentRole);
  // Check for row 1
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  index = arguments.at(0).toModelIndex(); // topLeft
  QCOMPARE(index.row(), 1);
  QCOMPARE(index.column(), 1);
  index = arguments.at(1).toModelIndex(); // bottomRight
  QCOMPARE(index.row(), 1);
  QCOMPARE(index.column(), 1);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::TextAlignmentRole);
}

void FormatProxyModelTest::textFontTest()
{
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(3, 2);
  model.populateColumn(0, {1,2,3});
  model.populateColumn(1, {"A","B","C"});
  /*
   * Setup proxy model
   */
  FormatProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  /*
   * Setup fonts
   */
  QFont font12;
  font12.setFamily("Times");
  font12.setPointSize(12);
  QFont font14;
  font14.setFamily("Times");
  font14.setPointSize(14);
  QFont font16;
  font16.setFamily("Times");
  font16.setPointSize(16);
  /*
   * Setup text fonts (we check only sizes):
   *           16
   *    -----------
   *    | 12 |    |
   *    -----------
   * 14 |    |    |
   *    -----------
   */
  proxyModel.setTextFontForIndex(0, 0, font12);
  proxyModel.setTextFontForRow(1, font14);
  proxyModel.setTextFontForColumn(1, font16);
  // Check data for DisplayRole
  QCOMPARE(getModelData(proxyModel, 0, 0, Qt::DisplayRole), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 0, 1, Qt::DisplayRole), QVariant("A"));
  QCOMPARE(getModelData(proxyModel, 1, 0, Qt::DisplayRole), QVariant(2));
  // Check getter
  QCOMPARE(proxyModel.textFont(0, 0).value<QFont>().pointSize(), 12);
  QCOMPARE(proxyModel.textFont(0, 1).value<QFont>().pointSize(), 16);
  QCOMPARE(proxyModel.textFont(1, 0).value<QFont>().pointSize(), 14);
  // Check data for FontRole
  QCOMPARE(getModelData(proxyModel, 0, 0, Qt::FontRole).value<QFont>().pointSize(), 12);
  QCOMPARE(getModelData(proxyModel, 0, 1, Qt::FontRole).value<QFont>().pointSize(), 16);
  QCOMPARE(getModelData(proxyModel, 1, 0, Qt::FontRole).value<QFont>().pointSize(), 14);
  /*
   * Clear text fonts:
   *
   *    -----------
   *    |    |    |
   *    -----------
   *    |    |    |
   *    -----------
   */
  proxyModel.clearTextFontForIndex(0, 0);
  proxyModel.clearTextFontForRow(1);
  proxyModel.clearTextFontForColumn(1);
  // Check data for DisplayRole
  QCOMPARE(getModelData(proxyModel, 0, 0, Qt::DisplayRole), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 0, 1, Qt::DisplayRole), QVariant("A"));
  QCOMPARE(getModelData(proxyModel, 1, 0, Qt::DisplayRole), QVariant(2));
  // Check getter
  QVERIFY(proxyModel.textFont(0, 0).isNull());
  QVERIFY(proxyModel.textFont(0, 1).isNull());
  QVERIFY(proxyModel.textFont(1, 0).isNull());
  // Check data for FontRole
  QVERIFY(getModelData(proxyModel, 0, 0, Qt::FontRole).isNull());
  QVERIFY(getModelData(proxyModel, 0, 1, Qt::FontRole).isNull());
  QVERIFY(getModelData(proxyModel, 1, 0, Qt::FontRole).isNull());
}

void FormatProxyModelTest::textFontSignalTest()
{
  QVariantList arguments;
  QModelIndex index;
  QVector<int> roles;
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(1, 2);
  model.populateColumn(0, {1});
  model.populateColumn(1, {"A"});
  /*
   * Setup proxy model
   */
  FormatProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  /*
   * Setup signal spy
   */
  QSignalSpy dataChangedSpy(&proxyModel, &FormatProxyModel::dataChanged);
  QVERIFY(dataChangedSpy.isValid());
  /*
   * Set font for index 0, 1
   */
  proxyModel.setTextFontForIndex(0, 1, QFont("Times", 14));
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::FontRole);
  /*
   * Clear font of index 0, 1
   */
  proxyModel.clearTextFontForIndex(0, 1);
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::FontRole);
  /*
   * Set font for row 0
   */
  proxyModel.setTextFontForRow(0, QFont("Times", 14));
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::FontRole);
  /*
   * Clear font of row 0
   */
  proxyModel.clearTextFontForRow(0);
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::FontRole);
  /*
   * Set font for column 1
   */
  proxyModel.setTextFontForColumn(1, QFont("Times", 14));
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::FontRole);
  /*
   * Clear font of column 1
   */
  proxyModel.clearTextFontForColumn(1);
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::FontRole);
}

void FormatProxyModelTest::textColorTest()
{
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(3, 2);
  model.populateColumn(0, {1,2,3});
  model.populateColumn(1, {"A","B","C"});
  /*
   * Setup proxy model
   */
  FormatProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  /*
   * Setup colors
   */
  QColor color10(10, 0, 0);
  QColor color20(20, 0, 0);
  QColor color30(30, 0, 0);
  /*
   * Setup text colors (we only check red part):
   *           30
   *    -----------
   *    | 10 |    |
   *    -----------
   * 20 |    |    |
   *    -----------
   */
  proxyModel.setTextColorForIndex(0, 0, color10);
  proxyModel.setTextColorForRow(1, color20);
  proxyModel.setTextColorForColumn(1, color30);
  // Check data for DisplayRole
  QCOMPARE(getModelData(proxyModel, 0, 0, Qt::DisplayRole), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 0, 1, Qt::DisplayRole), QVariant("A"));
  QCOMPARE(getModelData(proxyModel, 1, 0, Qt::DisplayRole), QVariant(2));
  // Check getter
  QCOMPARE(proxyModel.foregroundBrush(0, 0).value<QBrush>().color().red(), 10);
  QCOMPARE(proxyModel.foregroundBrush(0, 1).value<QBrush>().color().red(), 30);
  QCOMPARE(proxyModel.foregroundBrush(1, 0).value<QBrush>().color().red(), 20);
  // Check data for ForegroundRole
  QCOMPARE(getModelData(proxyModel, 0, 0, Qt::ForegroundRole).value<QBrush>().color().red(), 10);
  QCOMPARE(getModelData(proxyModel, 0, 1, Qt::ForegroundRole).value<QBrush>().color().red(), 30);
  QCOMPARE(getModelData(proxyModel, 1, 0, Qt::ForegroundRole).value<QBrush>().color().red(), 20);
  /*
   * Clear text fonts:
   *
   *    -----------
   *    |    |    |
   *    -----------
   *    |    |    |
   *    -----------
   */
  proxyModel.clearTextColorForIndex(0, 0);
  proxyModel.clearTextColorForRow(1);
  proxyModel.clearTextColorForColumn(1);
  // Check data for DisplayRole
  QCOMPARE(getModelData(proxyModel, 0, 0, Qt::DisplayRole), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 0, 1, Qt::DisplayRole), QVariant("A"));
  QCOMPARE(getModelData(proxyModel, 1, 0, Qt::DisplayRole), QVariant(2));
  // Check getter
  QVERIFY(proxyModel.foregroundBrush(0, 0).isNull());
  QVERIFY(proxyModel.foregroundBrush(0, 1).isNull());
  QVERIFY(proxyModel.foregroundBrush(1, 0).isNull());
  // Check data for ForegroundRole
  QVERIFY(getModelData(proxyModel, 0, 0, Qt::ForegroundRole).isNull());
  QVERIFY(getModelData(proxyModel, 0, 1, Qt::ForegroundRole).isNull());
  QVERIFY(getModelData(proxyModel, 1, 0, Qt::ForegroundRole).isNull());
}

void FormatProxyModelTest::textColorSignalTest()
{
  QVariantList arguments;
  QModelIndex index;
  QVector<int> roles;
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(1, 2);
  model.populateColumn(0, {1});
  model.populateColumn(1, {"A"});
  /*
   * Setup proxy model
   */
  FormatProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  /*
   * Setup signal spy
   */
  QSignalSpy dataChangedSpy(&proxyModel, &FormatProxyModel::dataChanged);
  QVERIFY(dataChangedSpy.isValid());
  /*
   * Set text color for index 0, 1
   */
  proxyModel.setTextColorForIndex(0, 1, QColor(Qt::red));
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::ForegroundRole);
  /*
   * Clear text color of index 0, 1
   */
  proxyModel.clearTextColorForIndex(0, 1);
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::ForegroundRole);
  /*
   * Set text color for row 0
   */
  proxyModel.setTextColorForRow(0, QColor(Qt::red));
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::ForegroundRole);
  /*
   * Clear text color of row 0
   */
  proxyModel.clearTextColorForRow(0);
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::ForegroundRole);
  /*
   * Set text color for column 1
   */
  proxyModel.setTextColorForColumn(1, QColor(Qt::red));
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::ForegroundRole);
  /*
   * Clear text color of column 1
   */
  proxyModel.clearTextColorForColumn(1);
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::ForegroundRole);
}

void FormatProxyModelTest::backgroundColorTest()
{
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(3, 2);
  model.populateColumn(0, {1,2,3});
  model.populateColumn(1, {"A","B","C"});
  /*
   * Setup proxy model
   */
  FormatProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  /*
   * Setup colors
   */
  QColor color10(10, 0, 0);
  QColor color20(20, 0, 0);
  QColor color30(30, 0, 0);
  /*
   * Setup background colors (we only check red part):
   *           30
   *    -----------
   *    | 10 |    |
   *    -----------
   * 20 |    |    |
   *    -----------
   */
  proxyModel.setBackgroundColorForIndex(0, 0, color10);
  proxyModel.setBackgroundColorForRow(1, color20);
  proxyModel.setBackgroundBrushForColumn(1, color30);
  // Check data for DisplayRole
  QCOMPARE(getModelData(proxyModel, 0, 0, Qt::DisplayRole), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 0, 1, Qt::DisplayRole), QVariant("A"));
  QCOMPARE(getModelData(proxyModel, 1, 0, Qt::DisplayRole), QVariant(2));
  // Check getter
  QCOMPARE(proxyModel.backgroundBrush(0, 0).value<QBrush>().color().red(), 10);
  QCOMPARE(proxyModel.backgroundBrush(0, 1).value<QBrush>().color().red(), 30);
  QCOMPARE(proxyModel.backgroundBrush(1, 0).value<QBrush>().color().red(), 20);
  // Check data for ForegroundRole
  QCOMPARE(getModelData(proxyModel, 0, 0, Qt::BackgroundRole).value<QBrush>().color().red(), 10);
  QCOMPARE(getModelData(proxyModel, 0, 1, Qt::BackgroundRole).value<QBrush>().color().red(), 30);
  QCOMPARE(getModelData(proxyModel, 1, 0, Qt::BackgroundRole).value<QBrush>().color().red(), 20);
  /*
   * Clear text fonts:
   *
   *    -----------
   *    |    |    |
   *    -----------
   *    |    |    |
   *    -----------
   */
  proxyModel.clearBackgroundBrushForIndex(0, 0);
  proxyModel.clearBackgroundBrushForRow(1);
  proxyModel.clearBackgroundBrushForColumn(1);
  // Check data for DisplayRole
  QCOMPARE(getModelData(proxyModel, 0, 0, Qt::DisplayRole), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 0, 1, Qt::DisplayRole), QVariant("A"));
  QCOMPARE(getModelData(proxyModel, 1, 0, Qt::DisplayRole), QVariant(2));
  // Check getter
  QVERIFY(proxyModel.backgroundBrush(0, 0).isNull());
  QVERIFY(proxyModel.backgroundBrush(0, 1).isNull());
  QVERIFY(proxyModel.backgroundBrush(1, 0).isNull());
  // Check data for ForegroundRole
  QVERIFY(getModelData(proxyModel, 0, 0, Qt::BackgroundRole).isNull());
  QVERIFY(getModelData(proxyModel, 0, 1, Qt::BackgroundRole).isNull());
  QVERIFY(getModelData(proxyModel, 1, 0, Qt::BackgroundRole).isNull());
}

void FormatProxyModelTest::backgroundColorSignalTest()
{
  QVariantList arguments;
  QModelIndex index;
  QVector<int> roles;
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(1, 2);
  model.populateColumn(0, {1});
  model.populateColumn(1, {"A"});
  /*
   * Setup proxy model
   */
  FormatProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  /*
   * Setup signal spy
   */
  QSignalSpy dataChangedSpy(&proxyModel, &FormatProxyModel::dataChanged);
  QVERIFY(dataChangedSpy.isValid());
  /*
   * Set background color for index 0, 1
   */
  proxyModel.setBackgroundColorForIndex(0, 1, QColor(Qt::red));
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::BackgroundRole);
  /*
   * Clear text color of index 0, 1
   */
  proxyModel.clearBackgroundBrushForIndex(0, 1);
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::BackgroundRole);
  /*
   * Set background color for row 0
   */
  proxyModel.setBackgroundColorForRow(0, QColor(Qt::red));
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::BackgroundRole);
  /*
   * Clear text color of row 0
   */
  proxyModel.clearBackgroundBrushForRow(0);
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::BackgroundRole);
  /*
   * Set background color for column 0
   */
  proxyModel.setBackgroundColorForColumn(0, QColor(Qt::red));
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::BackgroundRole);
  /*
   * Clear text color of column 0
   */
  proxyModel.clearBackgroundBrushForColumn(0);
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.size(), 1);
  QCOMPARE(roles.at(0), (int)Qt::BackgroundRole);
}

void FormatProxyModelTest::setModelTest()
{
  /*
   * Check that we can do setup before setting source model
   */
  FormatProxyModel proxyModel;
  proxyModel.setTextAlignmentForColumn(0, Qt::AlignCenter);
  proxyModel.setTextFontForColumn(1, QFont("Times", 12));
  QCOMPARE(proxyModel.rowCount(), 0);
  QCOMPARE(proxyModel.columnCount(), 0);
  /*
   * Set source model
   */
  VariantTableModel model;
  model.resize(1, 2);
  model.populateColumn(0, {1});
  model.populateColumn(1, {"A"});
  proxyModel.setSourceModel(&model);
  // Check that format are still correct
  QCOMPARE(getModelData(proxyModel, 0, 0, Qt::TextAlignmentRole), QVariant(Qt::AlignCenter));
  QCOMPARE(getModelData(proxyModel, 0, 1, Qt::FontRole).value<QFont>().pointSize(), 12);
}

void FormatProxyModelTest::qtModelTest()
{
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(3, 2);
  model.populateColumn(0, {1,2,3});
  model.populateColumn(1, {"A","B","C"});
  /*
   * Setup proxy model
   */
  FormatProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  /*
   * Test
   */
  QCOMPARE(proxyModel.rowCount(), 3);
  QtModelTest mt(&proxyModel);
  QtModelTest smt(&model);
}

void FormatProxyModelTest::getDiplayRoleDataBenchmark()
{
  QFETCH(int, n);
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(n, 2);
  model.populateColumnWithInt(0, 1);
  model.populateColumnWithInt(1, -1);
  /*
   * Setup proxy model
   */
  FormatProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  proxyModel.setTextAlignmentForColumn(0, Qt::AlignCenter);
  QBENCHMARK{
    for(int row = 0; row < n; ++row){
      QCOMPARE(proxyModel.data( proxyModel.index(row, 0) ).toInt(), row+1);
      QVERIFY(!proxyModel.data( proxyModel.index(row, 1) ).isNull());
    }
  }
}

void FormatProxyModelTest::getDiplayRoleDataBenchmark_data()
{
  QTest::addColumn<int>("n");

  QTest::newRow("10") << 10;
  QTest::newRow("100") << 100;
  QTest::newRow("1'000") << 1000;
  QTest::newRow("10'000") << 10000;
}

void FormatProxyModelTest::getTextAlignmentRoleDataBenchmark()
{
  QFETCH(int, n);
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(n, 2);
  model.populateColumnWithInt(0, 1);
  model.populateColumnWithInt(1, -1);
  /*
   * Setup proxy model
   */
  FormatProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  proxyModel.setTextAlignmentForColumn(0, Qt::AlignCenter);
  QBENCHMARK{
    for(int row = 0; row < n; ++row){
      QCOMPARE(proxyModel.data( proxyModel.index(row, 0) , Qt::TextAlignmentRole ).toInt(), (int)Qt::AlignCenter);
      QVERIFY(proxyModel.data( proxyModel.index(row, 1) , Qt::TextAlignmentRole).isNull());
    }
  }
}

void FormatProxyModelTest::getTextAlignmentRoleDataBenchmark_data()
{
  QTest::addColumn<int>("n");

  QTest::newRow("10") << 10;
  QTest::newRow("100") << 100;
  QTest::newRow("1'000") << 1000;
  QTest::newRow("10'000") << 10000;
}

/*
 * Main
 */
MDT_ITEM_MODEL_TEST_MAIN(FormatProxyModelTest)
// MDT_ITEM_MODEL_TEST_MAIN_DEBUG_ENV(FormatProxyModelTest)
