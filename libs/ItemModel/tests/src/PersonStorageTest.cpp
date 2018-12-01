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
#include "PersonStorageTest.h"
#include "PersonStorage.h"

/*
 * Tests
 */

void PersonStorageTest::addGetTest()
{
  int id;
  Person person;
  PersonStorage storage;

  QCOMPARE(storage.count(), 0);
  QVERIFY(!storage.hasId(1));

  person.name = "A";
  QCOMPARE(person.id, 0);
  id = storage.add(person);
  QCOMPARE(id, 1);
  QCOMPARE(storage.count(), 1);
  QVERIFY(storage.hasId(1));
  person = storage.getById(1);
  QCOMPARE(person.id, 1);
  QCOMPARE(person.name, QString("A"));

  person.name = "B";
  person.id = 0;
  id = storage.add(person);
  QCOMPARE(id, 2);
  QCOMPARE(storage.count(), 2);
  QVERIFY(storage.hasId(1));
  QVERIFY(storage.hasId(2));
  person = storage.getById(2);
  QCOMPARE(person.id, 2);
  QCOMPARE(person.name, QString("B"));

  person.name = "E";
  person.id = 5;
  id = storage.add(person);
  QCOMPARE(id, 5);
  QCOMPARE(storage.count(), 3);
  QVERIFY(storage.hasId(1));
  QVERIFY(storage.hasId(2));
  QVERIFY(!storage.hasId(3));
  QVERIFY(!storage.hasId(4));
  QVERIFY(storage.hasId(5));
  person = storage.getById(5);
  QCOMPARE(person.id, 5);
  QCOMPARE(person.name, QString("E"));

  person.name = "F";
  person.id = 0;
  id = storage.add(person);
  QCOMPARE(id, 6);
  QCOMPARE(storage.count(), 4);
  person = storage.getById(6);
  QCOMPARE(person.id, 6);
  QCOMPARE(person.name, QString("F"));
}

void PersonStorageTest::populateTest()
{
  PersonStorage storage;
  QCOMPARE(storage.count(), 0);

  storage.populate({{11,"A"},{12,"B"}});
  QCOMPARE(storage.count(), 2);
  QCOMPARE(storage.nameById(11), QString("A"));
  QCOMPARE(storage.nameById(12), QString("B"));

  storage.populate({{13,"C"}});
  QCOMPARE(storage.count(), 1);
  QCOMPARE(storage.nameById(13), QString("C"));
}

void PersonStorageTest::populateByNamesTest()
{
  PersonStorage storage;
  QCOMPARE(storage.count(), 0);

  storage.populateByNames({"A","B","C"});
  QCOMPARE(storage.count(), 3);
  QCOMPARE(storage.nameById(1), QString("A"));
  QCOMPARE(storage.nameById(2), QString("B"));
  QCOMPARE(storage.nameById(3), QString("C"));

  storage.populateByNames({"D","E"});
  QCOMPARE(storage.count(), 2);
  QCOMPARE(storage.nameById(1), QString("D"));
  QCOMPARE(storage.nameById(2), QString("E"));
}

void PersonStorageTest::getCountPersonsTest()
{
  PersonStorage storage;
  std::vector<Person> list;

  storage.populateByNames({"A","B","C"});

  list = storage.getCountPersons(1);
  QCOMPARE(list.size(), 1ul);
  QCOMPARE(list.at(0).name, QString("A"));

  list = storage.getCountPersons(2);
  QCOMPARE(list.size(), 2ul);
  QCOMPARE(list.at(0).name, QString("A"));
  QCOMPARE(list.at(1).name, QString("B"));

  list = storage.getCountPersons(3);
  QCOMPARE(list.size(), 3ul);
  QCOMPARE(list.at(0).name, QString("A"));
  QCOMPARE(list.at(1).name, QString("B"));
  QCOMPARE(list.at(2).name, QString("C"));
}

void PersonStorageTest::updateTest()
{
  Person person;
  PersonStorage storage;

  QCOMPARE(person.id, 0);
  person.name = "A";
  storage.add(person);
  QCOMPARE(person.id, 0);
  person.name = "B";
  storage.add(person);

  QCOMPARE(storage.nameById(1), QString("A"));
  QCOMPARE(storage.nameById(2), QString("B"));

  person = storage.getById(1);
  person.name = "uA";
  storage.update(person);

  QCOMPARE(storage.nameById(1), QString("uA"));
  QCOMPARE(storage.nameById(2), QString("B"));
}

void PersonStorageTest::removeTest()
{
  Person person;
  PersonStorage storage;

  QCOMPARE(person.id, 0);
  person.name = "A";
  storage.add(person);
  QCOMPARE(person.id, 0);
  person.name = "B";
  storage.add(person);

  QCOMPARE(storage.nameById(1), QString("A"));
  QCOMPARE(storage.nameById(2), QString("B"));

  storage.remove(1);
  QCOMPARE(storage.count(), 1);
  QVERIFY(!storage.hasId(1));
  QVERIFY(storage.hasId(2));
  QCOMPARE(storage.nameById(2), QString("B"));

  storage.remove(2);
  QCOMPARE(storage.count(), 0);
  QVERIFY(!storage.hasId(1));
  QVERIFY(!storage.hasId(2));
}

// void PersonStorageTest::pendingTasksTest()
// {
//   Person person;
//   PersonPendingTasks tasks;
// 
//   person.id = 10;
//   person.name = "A";
//   tasks.submitTask(1, person);
//   QVERIFY(tasks.hasTask(1));
//   QVERIFY(!tasks.hasTask(2));
// 
//   person.id = 11;
//   person.name = "B";
//   tasks.submitTask(2, person);
//   QVERIFY(tasks.hasTask(1));
//   QVERIFY(tasks.hasTask(2));
// 
//   person = tasks.takeByTaskId(1);
//   QCOMPARE(person.id, 10);
//   QCOMPARE(person.name, QString("A"));
//   QVERIFY(!tasks.hasTask(1));
//   QVERIFY(tasks.hasTask(2));
// 
//   person = tasks.takeByTaskId(2);
//   QCOMPARE(person.id, 11);
//   QCOMPARE(person.name, QString("B"));
//   QVERIFY(!tasks.hasTask(1));
//   QVERIFY(!tasks.hasTask(2));
// }

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  PersonStorageTest test;

  return QTest::qExec(&test, argc, argv);
}
