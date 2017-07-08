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
#include "RecordListTableModelTest.h"
#include "Mdt/PlainText/RecordListTableModel.h"
#include "Mdt/PlainText/RecordList.h"
#include "qtmodeltest.h"

using namespace Mdt::PlainText;

void RecordListTableModelTest::initTestCase()
{
}

void RecordListTableModelTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void RecordListTableModelTest::columnCountTest()
{
  QFETCH(RecordList, sourceData);
  QFETCH(int, expectedColumnCount);

  RecordListTableModel model;
  model.setRecordList(sourceData);
  QCOMPARE(model.columnCount(), expectedColumnCount);
}

void RecordListTableModelTest::columnCountTest_data()
{
  QTest::addColumn<RecordList>("sourceData");
  QTest::addColumn<int>("expectedColumnCount");

  RecordList sourceData;
  int expectedColumnCount;

  // Empty
  sourceData = {};
  expectedColumnCount = 0;
  QTest::newRow("") << sourceData << expectedColumnCount;
  // 1 record
  sourceData = {{"A"}};
  expectedColumnCount = 1;
  QTest::newRow("") << sourceData << expectedColumnCount;
  // Multiple records, allways same column count
  sourceData = {{"0A","0B"},{"1A","1B"}};
  expectedColumnCount = 2;
  QTest::newRow("") << sourceData << expectedColumnCount;
  // Multiple records, step column count up
  sourceData = {{"0A"},{"1A","1B"},{"2A","2B","2C"}};
  expectedColumnCount = 3;
  QTest::newRow("") << sourceData << expectedColumnCount;
  // Multiple records, step column count down
  sourceData = {{"0A","0B","0C"},{"1A","1B"},{"2A"}};
  expectedColumnCount = 3;
  QTest::newRow("") << sourceData << expectedColumnCount;
  // Multiple records, multiple column count
  sourceData = {{"0A"},{"1A","1B","1C"},{"2A","2B"}};
  expectedColumnCount = 3;
  QTest::newRow("") << sourceData << expectedColumnCount;
}

void RecordListTableModelTest::dataTest()
{
  QFETCH(RecordList, sourceData);

  RecordListTableModel model;
  model.setRecordList(sourceData);
  QCOMPARE(model.rowCount(), sourceData.rowCount());
  for(int row = 0; row < model.rowCount(); ++row){
    for(int col = 0; col < model.columnCount(); ++col){
      if(col >= sourceData.columnCount(row)){
        QVERIFY(getModelData(model, row, col).isNull());
      }else{
        QCOMPARE(getModelData(model, row, col), sourceData.data(row, col));
      }
    }
  }
}

void RecordListTableModelTest::dataTest_data()
{
  QTest::addColumn<RecordList>("sourceData");

  RecordList sourceData;

  // Empty
  sourceData = {};
  QTest::newRow("") << sourceData;
  // 1 record
  sourceData = {{"A"}};
  QTest::newRow("") << sourceData;
  // Multiple records, allways same column count
  sourceData = {{"0A","0B"},{"1A","1B"}};
  QTest::newRow("") << sourceData;
  // Multiple records, step column count up
  sourceData = {{"0A"},{"1A","1B"},{"2A","2B","2C"}};
  QTest::newRow("") << sourceData;
  // Multiple records, step column count down
  sourceData = {{"0A","0B","0C"},{"1A","1B"},{"2A"}};
  QTest::newRow("") << sourceData;
  // Multiple records, multiple column count
  sourceData = {{"0A"},{"1A","1B","1C"},{"2A","2B"}};
  QTest::newRow("") << sourceData;
}

void RecordListTableModelTest::qtModelTest()
{
  /*
   * Setup model
   */
  RecordList data = {{"0A"},{"0B"},{"0C"},{"1A"},{"1B"},{"1C"}};
  RecordListTableModel model;
  model.setRecordList(data);
  /*
   * Test
   */
  QtModelTest mt(&model);
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  RecordListTableModelTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
