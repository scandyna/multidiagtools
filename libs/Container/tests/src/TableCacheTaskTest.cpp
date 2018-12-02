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
#include "TableCacheTaskTest.h"
#include "Mdt/Container/TableCacheTaskMap.h"

using namespace Mdt::Container;

/*
 * Tests
 */

void TableCacheTaskTest::taskTest()
{
  TableCacheTask t0;
  QCOMPARE(t0.id(), 0);
  QVERIFY(t0.isNull());

  TableCacheTask t1(1);
  QCOMPARE(t1.id(), 1);
  QVERIFY(!t1.isNull());
}

void TableCacheTaskTest::rowTaskTest()
{
  TableCacheRowTask t0;
  QCOMPARE(t0.row(), -1);
  QCOMPARE(t0.taskId(), 0);
  QVERIFY(t0.isNull());

  TableCacheRowTask t1(0, TableCacheTask(1));
  QCOMPARE(t1.row(), 0);
  QCOMPARE(t1.taskId(), 1);
  QVERIFY(!t1.isNull());
}

void TableCacheTaskTest::mapItemTest()
{
  TableCacheTaskMapItem i1(2, TableCacheTask(1), TableCacheTaskState::None);
  QCOMPARE(i1.row(), 2);
  QCOMPARE(i1.taskId(), 1);
  QVERIFY(!i1.isPending());
  QVERIFY(!i1.isFailed());

  QCOMPARE(i1.row(), 2);
  i1.shiftRow(2);
  QCOMPARE(i1.row(), 4);
  i1.shiftRow(-1);
  QCOMPARE(i1.row(), 3);

  TableCacheTaskMapItem i2(5, TableCacheTask(2), TableCacheTaskState::Pending);
  QCOMPARE(i2.row(), 5);
  QCOMPARE(i2.taskId(), 2);
  QVERIFY(i2.isPending());
  QVERIFY(!i2.isFailed());

  TableCacheTaskMapItem i3(7, TableCacheTask(3), TableCacheTaskState::Failed);
  QCOMPARE(i3.row(), 7);
  QCOMPARE(i3.taskId(), 3);
  QVERIFY(!i3.isPending());
  QVERIFY(i3.isFailed());
}

void TableCacheTaskTest::beginRowTaskTest()
{
  TableCacheRowTask task;

  TableCacheTaskMap map;
  QCOMPARE(map.taskCount(), 0);
  QVERIFY(map.isEmpty());

  task = map.beginRowTask(2);
  QCOMPARE(task.row(), 2);
  QCOMPARE(task.taskId(), 1);
  QCOMPARE(map.taskCount(), 1);
  QVERIFY(!map.isEmpty());

  task = map.beginRowTask(5);
  QCOMPARE(task.row(), 5);
  QCOMPARE(task.taskId(), 2);
  QCOMPARE(map.taskCount(), 2);

  map.clear();
  QCOMPARE(map.taskCount(), 0);

  task = map.beginRowTask(6);
  QCOMPARE(task.row(), 6);
  QCOMPARE(task.taskId(), 1);
  QCOMPARE(map.taskCount(), 1);
}

void TableCacheTaskTest::beginRowTaskListTest()
{
  TableCacheRowTaskList rowTaskList;

  TableCacheTaskMap map;
  QCOMPARE(map.taskCount(), 0);

  rowTaskList = map.beginRowTasks(RowList{0,3});
  QCOMPARE(rowTaskList.size(), 2);
  QCOMPARE(rowTaskList.at(0).row(), 0);
  QCOMPARE(rowTaskList.at(0).taskId(), 1);
  QCOMPARE(rowTaskList.at(1).row(), 3);
  QCOMPARE(rowTaskList.at(1).taskId(), 2);
  QCOMPARE(map.taskCount(), 2);
}

void TableCacheTaskTest::setTaskDoneFailedTest()
{
  TableCacheTaskMap map;

  map.beginRowTask(11);
  map.beginRowTask(12);
  map.beginRowTask(13);

  QCOMPARE(map.taskCount(), 3);
  QVERIFY(map.isTaskPendingForRow(11));
  QVERIFY(map.isTaskPendingForRow(12));
  QVERIFY(map.isTaskPendingForRow(13));
  QVERIFY(!map.isTaskFailedForRow(11));
  QVERIFY(!map.isTaskFailedForRow(12));
  QVERIFY(!map.isTaskFailedForRow(13));

  map.setTaskDoneForRow(11);
  QCOMPARE(map.taskCount(), 2);
  QVERIFY(!map.isTaskPendingForRow(11));
  QVERIFY(map.isTaskPendingForRow(12));
  QVERIFY(map.isTaskPendingForRow(13));
  QVERIFY(!map.isTaskFailedForRow(11));
  QVERIFY(!map.isTaskFailedForRow(12));
  QVERIFY(!map.isTaskFailedForRow(13));

  map.setTaskFailedForRow(12);
  QCOMPARE(map.taskCount(), 2);
  QVERIFY(!map.isTaskPendingForRow(11));
  QVERIFY(!map.isTaskPendingForRow(12));
  QVERIFY(map.isTaskPendingForRow(13));
  QVERIFY(!map.isTaskFailedForRow(11));
  QVERIFY(map.isTaskFailedForRow(12));
  QVERIFY(!map.isTaskFailedForRow(13));

  map.setTaskDoneForRow(12);
  QCOMPARE(map.taskCount(), 1);
  QVERIFY(!map.isTaskPendingForRow(11));
  QVERIFY(!map.isTaskPendingForRow(12));
  QVERIFY(map.isTaskPendingForRow(13));
  QVERIFY(!map.isTaskFailedForRow(11));
  QVERIFY(!map.isTaskFailedForRow(12));
  QVERIFY(!map.isTaskFailedForRow(13));
}

void TableCacheTaskTest::getRowForTaskTest()
{
  TableCacheTaskMap map;

  const auto rowTask1 = map.beginRowTask(11);
  const auto rowTask2 = map.beginRowTask(12);
  const auto rowTask3 = map.beginRowTask(13);

  QCOMPARE(map.getRowForTask(rowTask1.task()), 11);
  QCOMPARE(map.getRowForTask(rowTask2.task()), 12);
  QCOMPARE(map.getRowForTask(rowTask3.task()), 13);

  map.setTaskDoneForRow(11);
  QCOMPARE(map.getRowForTask(rowTask1.task()), -1);
  QCOMPARE(map.getRowForTask(rowTask2.task()), 12);
  QCOMPARE(map.getRowForTask(rowTask3.task()), 13);
}

void TableCacheTaskTest::shiftRowsTest()
{
  TableCacheTaskMap map;
  QCOMPARE(map.taskCount(), 0);

  const auto rowTask1 = map.beginRowTask(1);
  const auto rowTask2 = map.beginRowTask(3);
  const auto rowTask3 = map.beginRowTask(5);
  const auto rowTask4 = map.beginRowTask(6);

  QCOMPARE(map.getRowForTask(rowTask1.task()), 1);
  QCOMPARE(map.getRowForTask(rowTask2.task()), 3);
  QCOMPARE(map.getRowForTask(rowTask3.task()), 5);
  QCOMPARE(map.getRowForTask(rowTask4.task()), 6);

  map.shiftRows(3, 1);
  QCOMPARE(map.getRowForTask(rowTask1.task()), 1);
  QCOMPARE(map.getRowForTask(rowTask2.task()), 4);
  QCOMPARE(map.getRowForTask(rowTask3.task()), 6);
  QCOMPARE(map.getRowForTask(rowTask4.task()), 7);

  map.shiftRows(3, -1);
  QCOMPARE(map.getRowForTask(rowTask1.task()), 1);
  QCOMPARE(map.getRowForTask(rowTask2.task()), 3);
  QCOMPARE(map.getRowForTask(rowTask3.task()), 5);
  QCOMPARE(map.getRowForTask(rowTask4.task()), 6);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
    TableCacheTaskTest test;

  return QTest::qExec(&test, argc, argv);
}
