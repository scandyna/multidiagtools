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
#include "FormatMapTest.h"
#include "Mdt/ItemModel/IndexFormatMap.h"
#include "Mdt/ItemModel/RowFormatMap.h"
#include "Mdt/ItemModel/ColumnFormatMap.h"
#include "Mdt/ItemModel/FormatMap.h"
#include <QFont>
#include <Qt>
#include <QBrush>
#include <QColor>

using namespace Mdt::ItemModel;

/*
 * Init/Cleanup
 */

void FormatMapTest::initTestCase()
{
}

void FormatMapTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void FormatMapTest::indexItemQFontTest()
{
  QFont font("Times", 10);
  IndexFormatMapItem item(1, 2, font);
  QCOMPARE(item.row(), 1);
  QCOMPARE(item.column(), 2);
  QCOMPARE(item.value().value<QFont>().pointSize(), 10);
}

void FormatMapTest::indexItemQFlagsEnumTest()
{
  IndexFormatMapItem item(1, 2, Qt::AlignCenter);
  QCOMPARE(item.row(), 1);
  QCOMPARE(item.column(), 2);
  QCOMPARE(item.value(), QVariant(Qt::AlignCenter));
}

void FormatMapTest::indexItemQBrushTest()
{
  QBrush brush(QColor(50, 100, 200));
  IndexFormatMapItem item(1, 2, brush);
  QCOMPARE(item.row(), 1);
  QCOMPARE(item.column(), 2);
  QCOMPARE(item.value().value<QBrush>().color().red(), 50);
  QCOMPARE(item.value().value<QBrush>().color().green(), 100);
  QCOMPARE(item.value().value<QBrush>().color().blue(), 200);
}

void FormatMapTest::indexMapQFontTest()
{
  IndexFormatMap map;
  QFont font;
  font.setBold(true);
  font.setPointSize(10);
  map.setFormatForIndex(1, 2, font);
  QVERIFY(map.formatForIndex(1, 1).isNull());
  QCOMPARE(map.formatForIndex(1, 2).value<QFont>().bold(), true);
  QCOMPARE(map.formatForIndex(1, 2).value<QFont>().pointSize(), 10);
}

/*
 * This is the complete test for index format map
 */
void FormatMapTest::indexMapQFlagsEnumTest()
{
  /*
   * Initial state
   */
  IndexFormatMap map;
  QVERIFY(map.formatForIndex(1, 1).isNull());
  QVERIFY(map.formatForIndex(1, 2).isNull());
  QVERIFY(map.formatForIndex(2, 3).isNull());
  /*
   * Set alignments
   */
  // Set for index 1, 1
  map.setFormatForIndex(1, 1, Qt::AlignCenter);
  QCOMPARE(map.formatForIndex(1, 1), QVariant(Qt::AlignCenter));
  QVERIFY(map.formatForIndex(1, 2).isNull());
  QVERIFY(map.formatForIndex(2, 3).isNull());
  // Set for index 1, 2
  map.setFormatForIndex(1, 2, Qt::AlignRight);
  QCOMPARE(map.formatForIndex(1, 1), QVariant(Qt::AlignCenter));
  QCOMPARE(map.formatForIndex(1, 2), QVariant(Qt::AlignRight));
  QVERIFY(map.formatForIndex(2, 3).isNull());
  // Set for index 2, 3
  map.setFormatForIndex(2, 3, Qt::AlignLeft);
  QCOMPARE(map.formatForIndex(1, 1), QVariant(Qt::AlignCenter));
  QCOMPARE(map.formatForIndex(1, 2), QVariant(Qt::AlignRight));
  QCOMPARE(map.formatForIndex(2, 3), QVariant(Qt::AlignLeft));
  /*
   * Change for index 2, 3
   */
  map.setFormatForIndex<>(2, 3, Qt::AlignBottom);
  QCOMPARE(map.formatForIndex(1, 1), QVariant(Qt::AlignCenter));
  QCOMPARE(map.formatForIndex(1, 2), QVariant(Qt::AlignRight));
  QCOMPARE(map.formatForIndex(2, 3), QVariant(Qt::AlignBottom));
  /*
   * Clear for index 2, 3
   */
  map.clearFormatForIndex(2, 3);
  QCOMPARE(map.formatForIndex(1, 1), QVariant(Qt::AlignCenter));
  QCOMPARE(map.formatForIndex(1, 2), QVariant(Qt::AlignRight));
  QVERIFY(map.formatForIndex(2, 3).isNull());
  // Try to clear for same index agin
  map.clearFormatForIndex(2, 3);
  QCOMPARE(map.formatForIndex(1, 1), QVariant(Qt::AlignCenter));
  QCOMPARE(map.formatForIndex(1, 2), QVariant(Qt::AlignRight));
  QVERIFY(map.formatForIndex(2, 3).isNull());
  /*
   * Clear for a non existing index
   */
  map.clearFormatForIndex(25, 30);
  QCOMPARE(map.formatForIndex(1, 1), QVariant(Qt::AlignCenter));
  QCOMPARE(map.formatForIndex(1, 2), QVariant(Qt::AlignRight));
  QVERIFY(map.formatForIndex(2, 3).isNull());
  /*
   * Clear for index 1, 1
   */
  map.clearFormatForIndex(1, 1);
  QVERIFY(map.formatForIndex(1, 1).isNull());
  QCOMPARE(map.formatForIndex(1, 2), QVariant(Qt::AlignRight));
  QVERIFY(map.formatForIndex(2, 3).isNull());
  /*
   * Clear for column 1, 2
   */
  map.clearFormatForIndex(1, 2);
  QVERIFY(map.formatForIndex(1, 1).isNull());
  QVERIFY(map.formatForIndex(1, 2).isNull());
  QVERIFY(map.formatForIndex(2, 3).isNull());
}

void FormatMapTest::indexMapQBrushTest()
{
  IndexFormatMap map;
  QBrush brush;
  brush.setColor(QColor(10, 20, 30));
  map.setFormatForIndex(1, 2, brush);
  QVERIFY(map.formatForIndex(1, 1).isNull());
  QCOMPARE(map.formatForIndex(1, 2).value<QBrush>().color().red(), 10);
  QCOMPARE(map.formatForIndex(1, 2).value<QBrush>().color().green(), 20);
  QCOMPARE(map.formatForIndex(1, 2).value<QBrush>().color().blue(), 30);
}

void FormatMapTest::rowOrColumnItemQFontTest()
{
  QFont font("Times", 10);
  RowOrColumnFormatMapItem item(0, font);
  QCOMPARE(item.index(), 0);
  QCOMPARE(item.value().value<QFont>().pointSize(), 10);
}

void FormatMapTest::rowOrColumnItemQFlagsEnumTest()
{
  RowOrColumnFormatMapItem item(0, Qt::AlignCenter);
  QCOMPARE(item.index(), 0);
  QCOMPARE(item.value(), QVariant(Qt::AlignCenter));
}

void FormatMapTest::rowOrColumnItemQBrushTest()
{
  QBrush brush(QColor(50, 100, 200));
  RowOrColumnFormatMapItem item(0, brush);
  QCOMPARE(item.index(), 0);
  QCOMPARE(item.value().value<QBrush>().color().red(), 50);
  QCOMPARE(item.value().value<QBrush>().color().green(), 100);
  QCOMPARE(item.value().value<QBrush>().color().blue(), 200);
}

void FormatMapTest::rowColumnMapBaseQFontTest()
{
  RowColumnFormatMapBase map;
  QFont font;
  font.setBold(true);
  font.setPointSize(10);
  map.setFormatVariantForIndex(1, font);
  QVERIFY(map.formatForIndex(0).isNull());
  QCOMPARE(map.formatForIndex(1).value<QFont>().bold(), true);
  QCOMPARE(map.formatForIndex(1).value<QFont>().pointSize(), 10);
}

/*
 * This is the complete test for row/column format map
 */
void FormatMapTest::rowColumnMapBaseQFlagsEnumTest()
{
  /*
   * Initial state
   */
  RowColumnFormatMapBase map;
  QVERIFY(map.formatForIndex(0).isNull());
  QVERIFY(map.formatForIndex(3).isNull());
  QVERIFY(map.formatForIndex(1).isNull());
  /*
   * Set alignments
   */
  // Set for column 0
  map.setFormatVariantForIndex(0, Qt::AlignCenter);
  QCOMPARE(map.formatForIndex(0), QVariant(Qt::AlignCenter));
  QVERIFY(map.formatForIndex(3).isNull());
  QVERIFY(map.formatForIndex(1).isNull());
  // Set for column 3
  map.setFormatVariantForIndex(3, Qt::AlignRight);
  QCOMPARE(map.formatForIndex(0), QVariant(Qt::AlignCenter));
  QCOMPARE(map.formatForIndex(3), QVariant(Qt::AlignRight));
  QVERIFY(map.formatForIndex(1).isNull());
  // Set for column 1
  map.setFormatVariantForIndex(1, Qt::AlignLeft);
  QCOMPARE(map.formatForIndex(0), QVariant(Qt::AlignCenter));
  QCOMPARE(map.formatForIndex(3), QVariant(Qt::AlignRight));
  QCOMPARE(map.formatForIndex(1), QVariant(Qt::AlignLeft));
  /*
   * Change for column 1
   */
  map.setFormatVariantForIndex(1, Qt::AlignBottom);
  QCOMPARE(map.formatForIndex(0), QVariant(Qt::AlignCenter));
  QCOMPARE(map.formatForIndex(3), QVariant(Qt::AlignRight));
  QCOMPARE(map.formatForIndex(1), QVariant(Qt::AlignBottom));
  /*
   * Clear for column 1
   */
  map.clearFormatForIndex(1);
  QCOMPARE(map.formatForIndex(0), QVariant(Qt::AlignCenter));
  QCOMPARE(map.formatForIndex(3), QVariant(Qt::AlignRight));
  QVERIFY(map.formatForIndex(1).isNull());
  // Try to clear for same column agin
  map.clearFormatForIndex(1);
  QCOMPARE(map.formatForIndex(0), QVariant(Qt::AlignCenter));
  QCOMPARE(map.formatForIndex(3), QVariant(Qt::AlignRight));
  QVERIFY(map.formatForIndex(1).isNull());
  /*
   * Clear for a non existing column
   */
  map.clearFormatForIndex(25);
  QCOMPARE(map.formatForIndex(0), QVariant(Qt::AlignCenter));
  QCOMPARE(map.formatForIndex(3), QVariant(Qt::AlignRight));
  QVERIFY(map.formatForIndex(1).isNull());
  /*
   * Clear for column 0
   */
  map.clearFormatForIndex(0);
  QVERIFY(map.formatForIndex(0).isNull());
  QCOMPARE(map.formatForIndex(3), QVariant(Qt::AlignRight));
  QVERIFY(map.formatForIndex(1).isNull());
  /*
   * Clear for column 3
   */
  map.clearFormatForIndex(3);
  QVERIFY(map.formatForIndex(0).isNull());
  QVERIFY(map.formatForIndex(3).isNull());
  QVERIFY(map.formatForIndex(1).isNull());
}

void FormatMapTest::rowColumnMapBaseQBrushTest()
{
  RowColumnFormatMapBase map;
  QBrush brush;
  brush.setColor(QColor(10, 20, 30));
  map.setFormatVariantForIndex(1, brush);
  QVERIFY(map.formatForIndex(0).isNull());
  QCOMPARE(map.formatForIndex(1).value<QBrush>().color().red(), 10);
  QCOMPARE(map.formatForIndex(1).value<QBrush>().color().green(), 20);
  QCOMPARE(map.formatForIndex(1).value<QBrush>().color().blue(), 30);
}

void FormatMapTest::rowMapQFlagsEnumTest()
{
  RowFormatMap map;
  map.setFormatForRow(1, Qt::AlignLeft);
  QCOMPARE(map.formatForRow(1), QVariant(Qt::AlignLeft));
  map.clearFormatForRow(1);
  QVERIFY(map.formatForRow(1).isNull());
}

void FormatMapTest::columnMapQFlagsEnumTest()
{
  ColumnFormatMap map;
  map.setFormatForColumn(2, Qt::AlignCenter);
  QCOMPARE(map.formatForColumn(2), QVariant(Qt::AlignCenter));
  map.clearFormatForColumn(2);
  QVERIFY(map.formatForColumn(2).isNull());
}

void FormatMapTest::mapIndexQFlagsEnumTest()
{
  FormatMap map;
  map.setFormatForIndex(1, 2, Qt::AlignCenter);
  QCOMPARE(map.formatForIndex(1, 2), QVariant(Qt::AlignCenter));
  map.clearFormatForIndex(1, 2);
  QVERIFY(map.formatForIndex(1, 2).isNull());
}

void FormatMapTest::mapRowQFlagsEnumTest()
{
  FormatMap map;
  map.setFormatForRow(1, Qt::AlignLeft);
  QCOMPARE(map.formatForIndex(1, 0), QVariant(Qt::AlignLeft));
  map.clearFormatForRow(1);
  QVERIFY(map.formatForIndex(1, 0).isNull());
}

void FormatMapTest::mapColumnQFlagsEnumTest()
{
  FormatMap map;
  map.setFormatForColumn(2, Qt::AlignCenter);
  QCOMPARE(map.formatForIndex(0, 2), QVariant(Qt::AlignCenter));
  map.clearFormatForColumn(2);
  QVERIFY(map.formatForIndex(0, 2).isNull());
}

void FormatMapTest::mapPriorityQFlagsEnumTest()
{
  FormatMap map;
  /*
   * Setup map:
   *                          AlignCenter
   *           ---------------------------
   *           |            |            |
   *           ---------------------------
   * AlignLeft |            |            |
   *           ---------------------------
   * AlignLeft |            | AlignRight |
   *           ---------------------------
   */
  map.setFormatForIndex(2, 1, Qt::AlignRight);
  map.setFormatForRow(1, Qt::AlignLeft);
  map.setFormatForRow(2, Qt::AlignLeft);
  map.setFormatForColumn(1, Qt::AlignCenter);
  // Check with priority: row, column, index
  map.setPriority({{FormatMapPriority::Row, FormatMapPriority::Column, FormatMapPriority::Index}});
  QVERIFY(map.formatForIndex(0, 0).isNull());
  QCOMPARE(map.formatForIndex(0, 1), QVariant(Qt::AlignCenter));
  QCOMPARE(map.formatForIndex(1, 0), QVariant(Qt::AlignLeft));
  QCOMPARE(map.formatForIndex(1, 1), QVariant(Qt::AlignLeft));
  QCOMPARE(map.formatForIndex(2, 0), QVariant(Qt::AlignLeft));
  QCOMPARE(map.formatForIndex(2, 1), QVariant(Qt::AlignLeft));
  // Check with priority: column, row, index
  map.setPriority({{FormatMapPriority::Column, FormatMapPriority::Row, FormatMapPriority::Index}});
  QVERIFY(map.formatForIndex(0, 0).isNull());
  QCOMPARE(map.formatForIndex(0, 1), QVariant(Qt::AlignCenter));
  QCOMPARE(map.formatForIndex(1, 0), QVariant(Qt::AlignLeft));
  QCOMPARE(map.formatForIndex(1, 1), QVariant(Qt::AlignCenter));
  QCOMPARE(map.formatForIndex(2, 0), QVariant(Qt::AlignLeft));
  QCOMPARE(map.formatForIndex(2, 1), QVariant(Qt::AlignCenter));
  // Check with priority: index, row, column
  map.setPriority({{FormatMapPriority::Index, FormatMapPriority::Row, FormatMapPriority::Column}});
  QVERIFY(map.formatForIndex(0, 0).isNull());
  QCOMPARE(map.formatForIndex(0, 1), QVariant(Qt::AlignCenter));
  QCOMPARE(map.formatForIndex(1, 0), QVariant(Qt::AlignLeft));
  QCOMPARE(map.formatForIndex(1, 1), QVariant(Qt::AlignLeft));
  QCOMPARE(map.formatForIndex(2, 0), QVariant(Qt::AlignLeft));
  QCOMPARE(map.formatForIndex(2, 1), QVariant(Qt::AlignRight));
  // Check with priority: index, column, row
  map.setPriority({{FormatMapPriority::Index, FormatMapPriority::Column, FormatMapPriority::Row}});
  QVERIFY(map.formatForIndex(0, 0).isNull());
  QCOMPARE(map.formatForIndex(0, 1), QVariant(Qt::AlignCenter));
  QCOMPARE(map.formatForIndex(1, 0), QVariant(Qt::AlignLeft));
  QCOMPARE(map.formatForIndex(1, 1), QVariant(Qt::AlignCenter));
  QCOMPARE(map.formatForIndex(2, 0), QVariant(Qt::AlignLeft));
  QCOMPARE(map.formatForIndex(2, 1), QVariant(Qt::AlignRight));
}

void FormatMapTest::mapQFontBenchmark()
{
  FormatMap map;
  QFont font1;
  font1.setPointSize(11);
  QFont font2;
  font2.setPointSize(12);
  QFont font3;
  font3.setPointSize(13);
  QBENCHMARK{
    map.setFormatForIndex(1, 2, font1);
    map.setFormatForRow(3, font2);
    map.setFormatForColumn(5, font3);
    QVERIFY(map.formatForIndex(0, 0).isNull());
    QCOMPARE(map.formatForIndex(1, 2).value<QFont>().pointSize(), 11);
    QVERIFY(map.formatForIndex(2, 0).isNull());
    QCOMPARE(map.formatForIndex(3, 0).value<QFont>().pointSize(), 12);
    QVERIFY(map.formatForIndex(4, 0).isNull());
    QCOMPARE(map.formatForIndex(0, 5).value<QFont>().pointSize(), 13);
  }
}

void FormatMapTest::mapQFlagsEnumBenchmark()
{
  FormatMap map;
  QBENCHMARK{
    map.setFormatForIndex(1, 2, Qt::AlignLeft);
    map.setFormatForRow(3, Qt::AlignCenter);
    map.setFormatForColumn(5, Qt::AlignRight);
    QVERIFY(map.formatForIndex(0, 0).isNull());
    QCOMPARE(map.formatForIndex(1, 2).toInt(), (int)Qt::AlignLeft);
    QVERIFY(map.formatForIndex(2, 0).isNull());
    QCOMPARE(map.formatForIndex(3, 0).toInt(), (int)Qt::AlignCenter);
    QVERIFY(map.formatForIndex(4, 0).isNull());
    QCOMPARE(map.formatForIndex(0, 5).toInt(), (int)Qt::AlignRight);
  }
}

/*
 * Main
 */
MDT_ITEM_MODEL_TEST_MAIN(FormatMapTest)
// MDT_ITEM_MODEL_TEST_MAIN_DEBUG_ENV(FormatMapTest)
