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
#include "ColumnFormatTest.h"
#include "Mdt/ItemModel/ColumnFormatMap.h"
#include <QFont>
#include <Qt>
#include <QBrush>
#include <QColor>

using namespace Mdt::ItemModel;

/*
 * Init/Cleanup
 */

void ColumnFormatTest::initTestCase()
{
}

void ColumnFormatTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void ColumnFormatTest::mapItemQFontTest()
{
  QFont font("Times", 10);
  ColumnFormatMapItem item(0, font);
  QCOMPARE(item.column(), 0);
  QCOMPARE(item.value().value<QFont>().pointSize(), 10);
}

void ColumnFormatTest::mapItemQFlagsEnumTest()
{
  ColumnFormatMapItem item(0, Qt::AlignCenter);
  QCOMPARE(item.column(), 0);
  QCOMPARE(item.value(), QVariant(Qt::AlignCenter));
}

void ColumnFormatTest::mapItemQBrushTest()
{
  QBrush brush(QColor(50, 100, 200));
  ColumnFormatMapItem item(0, brush);
  QCOMPARE(item.column(), 0);
  QCOMPARE(item.value().value<QBrush>().color().red(), 50);
  QCOMPARE(item.value().value<QBrush>().color().green(), 100);
  QCOMPARE(item.value().value<QBrush>().color().blue(), 200);
}

void ColumnFormatTest::mapQFontTest()
{
  ColumnFormatMap map;
  QFont font;
  font.setBold(true);
  font.setPointSize(10);
  map.setFormatForColumn(1, font);
  QVERIFY(map.formatForColumn(0).isNull());
  QCOMPARE(map.formatForColumn(1).value<QFont>().bold(), true);
  QCOMPARE(map.formatForColumn(1).value<QFont>().pointSize(), 10);
}

/*
 * This is the complete test
 */
void ColumnFormatTest::mapQFlagsEnumTest()
{
  /*
   * Initial state
   */
  ColumnFormatMap map;
  QVERIFY(map.formatForColumn(0).isNull());
  QVERIFY(map.formatForColumn(3).isNull());
  QVERIFY(map.formatForColumn(1).isNull());
  /*
   * Set alignments
   */
  // Set for column 0
  map.setFormatForColumn(0, Qt::AlignCenter);
  QCOMPARE(map.formatForColumn(0), QVariant(Qt::AlignCenter));
  QVERIFY(map.formatForColumn(3).isNull());
  QVERIFY(map.formatForColumn(1).isNull());
  // Set for column 3
  map.setFormatForColumn(3, Qt::AlignRight);
  QCOMPARE(map.formatForColumn(0), QVariant(Qt::AlignCenter));
  QCOMPARE(map.formatForColumn(3), QVariant(Qt::AlignRight));
  QVERIFY(map.formatForColumn(1).isNull());
  // Set for column 1
  map.setFormatForColumn(1, Qt::AlignLeft);
  QCOMPARE(map.formatForColumn(0), QVariant(Qt::AlignCenter));
  QCOMPARE(map.formatForColumn(3), QVariant(Qt::AlignRight));
  QCOMPARE(map.formatForColumn(1), QVariant(Qt::AlignLeft));
  /*
   * Change for column 1
   */
  map.setFormatForColumn(1, Qt::AlignBottom);
  QCOMPARE(map.formatForColumn(0), QVariant(Qt::AlignCenter));
  QCOMPARE(map.formatForColumn(3), QVariant(Qt::AlignRight));
  QCOMPARE(map.formatForColumn(1), QVariant(Qt::AlignBottom));
  /*
   * Clear for column 1
   */
  map.clearFormatForColumn(1);
  QCOMPARE(map.formatForColumn(0), QVariant(Qt::AlignCenter));
  QCOMPARE(map.formatForColumn(3), QVariant(Qt::AlignRight));
  QVERIFY(map.formatForColumn(1).isNull());
  // Try to clear for same column agin
  map.clearFormatForColumn(1);
  QCOMPARE(map.formatForColumn(0), QVariant(Qt::AlignCenter));
  QCOMPARE(map.formatForColumn(3), QVariant(Qt::AlignRight));
  QVERIFY(map.formatForColumn(1).isNull());
  /*
   * Clear for a non existing column
   */
  map.clearFormatForColumn(25);
  QCOMPARE(map.formatForColumn(0), QVariant(Qt::AlignCenter));
  QCOMPARE(map.formatForColumn(3), QVariant(Qt::AlignRight));
  QVERIFY(map.formatForColumn(1).isNull());
  /*
   * Clear for column 0
   */
  map.clearFormatForColumn(0);
  QVERIFY(map.formatForColumn(0).isNull());
  QCOMPARE(map.formatForColumn(3), QVariant(Qt::AlignRight));
  QVERIFY(map.formatForColumn(1).isNull());
  /*
   * Clear for column 3
   */
  map.clearFormatForColumn(3);
  QVERIFY(map.formatForColumn(0).isNull());
  QVERIFY(map.formatForColumn(3).isNull());
  QVERIFY(map.formatForColumn(1).isNull());
}

void ColumnFormatTest::mapQBrushTest()
{
  QFAIL("Not complete");
}

void ColumnFormatTest::mapQFontBenchmark()
{
  ColumnFormatMap map;
  QFont font1;
  font1.setPointSize(11);
  QFont font2;
  font2.setPointSize(12);
  QFont font3;
  font3.setPointSize(13);
  QBENCHMARK{
    map.setFormatForColumn(1, font1);
    map.setFormatForColumn(3, font2);
    map.setFormatForColumn(5, font3);
    QVERIFY(map.formatForColumn(0).isNull());
    QCOMPARE(map.formatForColumn(1).value<QFont>().pointSize(), 11);
    QVERIFY(map.formatForColumn(2).isNull());
    QCOMPARE(map.formatForColumn(3).value<QFont>().pointSize(), 12);
    QVERIFY(map.formatForColumn(4).isNull());
    QCOMPARE(map.formatForColumn(5).value<QFont>().pointSize(), 13);
  }
}

void ColumnFormatTest::mapQFlagsEnumBenchmark()
{
  ColumnFormatMap map;
  QBENCHMARK{
    map.setFormatForColumn(1, Qt::AlignLeft);
    map.setFormatForColumn(3, Qt::AlignCenter);
    map.setFormatForColumn(5, Qt::AlignRight);
    QVERIFY(map.formatForColumn(0).isNull());
    QCOMPARE(map.formatForColumn(1).toInt(), (int)Qt::AlignLeft);
    QVERIFY(map.formatForColumn(2).isNull());
    QCOMPARE(map.formatForColumn(3).toInt(), (int)Qt::AlignCenter);
    QVERIFY(map.formatForColumn(4).isNull());
    QCOMPARE(map.formatForColumn(5).toInt(), (int)Qt::AlignRight);
  }
}


/*
 * Main
 */
MDT_ITEM_MODEL_TEST_MAIN(ColumnFormatTest)
// MDT_ITEM_MODEL_TEST_MAIN_DEBUG_ENV(ColumnFormatTest)
