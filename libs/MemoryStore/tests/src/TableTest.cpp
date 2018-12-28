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
#include "TableTest.h"
#include "Mdt/MemoryStore/Table.h"
#include "Mdt/MemoryStore/TableWithAutoId.h"
#include "Mdt/Numeric/IntegralUniqueIdTemplate.h"
#include <QString>

#include <QDebug>

using namespace Mdt::MemoryStore;

/*
 * Contact entity
 */

class ContactId : public Mdt::Numeric::IntegralUniqueIdTemplate<ContactId, int>
{
 public:

  using IntegralUniqueIdTemplate::IntegralUniqueIdTemplate;
};

class Contact
{
 public:

  Contact() = delete;

  Contact(const QString & type, const QString & phoneNumber)
   : mType(type),
     mPhoneNumber(phoneNumber)
  {
  }

  QString type() const
  {
    return mType;
  }

  QString phoneNumber() const
  {
    return mPhoneNumber;
  }

 private:

  QString mType;
  QString mPhoneNumber;
};

/*
 * Composed, non incrementable primary key
 */
struct ComposedPrimaryKey
{
  QString idA;
  QString idB;
};

bool operator<(const ComposedPrimaryKey & a, const ComposedPrimaryKey & b)
{
  return ( QString::compare(a.idA, b.idA, Qt::CaseSensitive) < 0 ) && ( QString::compare(a.idB, b.idB, Qt::CaseSensitive) < 0 );
}

/*
 * Tests
 */

void TableTest::addWithPkTest()
{
  Table<Contact, int> table;
  QVERIFY(!table.containsPrimaryKey(1));
  QVERIFY(!table.containsPrimaryKey(2));
  QVERIFY(!table.containsPrimaryKey(5));

  Contact ca("A", "123");
  Contact cb("B", "456");

  QVERIFY(table.add(ca, 1));
  QVERIFY(table.containsPrimaryKey(1));
  QVERIFY(!table.containsPrimaryKey(2));

  QVERIFY(table.add(cb, 5));
  QVERIFY(table.containsPrimaryKey(1));
  QVERIFY(!table.containsPrimaryKey(2));
  QVERIFY(table.containsPrimaryKey(5));

  QVERIFY(!table.add(ca, 1));
  QVERIFY(table.containsPrimaryKey(1));
  QVERIFY(!table.containsPrimaryKey(2));
  QVERIFY(table.containsPrimaryKey(5));
}

void TableTest::addGetByPkTest()
{
  Table<Contact, int> table;

  Contact ca("A", "123");
  Contact cb("B", "456");

  QVERIFY(table.add(ca, 1));
  QVERIFY(table.add(cb, 2));

  auto rec = table.getByPrimaryKey(1);
  QVERIFY(rec);
  QCOMPARE(rec->type(), QString("A"));
  QCOMPARE(rec->phoneNumber(), QString("123"));

  rec = table.getByPrimaryKey(2);
  QVERIFY(rec);
  QCOMPARE(rec->type(), QString("B"));
  QCOMPARE(rec->phoneNumber(), QString("456"));

  rec = table.getByPrimaryKey(3);
  QVERIFY(!rec);
}

void TableTest::nextIdTest()
{
  TableWithAutoId<Contact> table;

  Contact ca("A", "123");
  Contact cb("B", "456");
  Contact cc("C", "789");
  Contact cd("D", "012");
  Contact ce("E", "345");

  const int idA = table.add(ca);
  QCOMPARE(idA, 1);
  const int idB = table.add(cb);
  QCOMPARE(idB, 2);
  const int idC = table.add(cc);
  QCOMPARE(idC, 3);

  table.removeAll();

  const int idD = table.add(cd);
  QCOMPARE(idD, 1);
  const int idE = table.add(ce);
  QCOMPARE(idE, 2);
}

void TableTest::updateTest()
{
  Table<Contact, int> table;

  Contact ca("A", "123");
  Contact cb("B", "456");
  Contact cc("C", "789");

  QVERIFY(table.add(ca, 1));
  QVERIFY(table.add(cb, 2));
  QVERIFY(table.add(cc, 3));

  Contact cub("uB", "u456");
  QVERIFY(table.update(cub, 2));

  auto rec = table.getByPrimaryKey(1);
  QVERIFY(rec);
  QCOMPARE(rec->type(), QString("A"));
  QCOMPARE(rec->phoneNumber(), QString("123"));

  rec = table.getByPrimaryKey(2);
  QVERIFY(rec);
  QCOMPARE(rec->type(), QString("uB"));
  QCOMPARE(rec->phoneNumber(), QString("u456"));

  rec = table.getByPrimaryKey(3);
  QVERIFY(rec);
  QCOMPARE(rec->type(), QString("C"));
  QCOMPARE(rec->phoneNumber(), QString("789"));
}

void TableTest::removeTest()
{
  Table<Contact, int> table;

  Contact ca("A", "123");
  Contact cb("B", "456");
  Contact cc("C", "789");

  QVERIFY(table.add(ca, 1));
  QVERIFY(table.add(cb, 2));
  QVERIFY(table.add(cc, 3));

  table.remove(2);

  auto rec = table.getByPrimaryKey(1);
  QVERIFY(rec);
  QCOMPARE(rec->type(), QString("A"));
  QCOMPARE(rec->phoneNumber(), QString("123"));

  rec = table.getByPrimaryKey(2);
  QVERIFY(!rec);

  rec = table.getByPrimaryKey(3);
  QVERIFY(rec);
  QCOMPARE(rec->type(), QString("C"));
  QCOMPARE(rec->phoneNumber(), QString("789"));

  table.removeAll();

  rec = table.getByPrimaryKey(1);
  QVERIFY(!rec);

  rec = table.getByPrimaryKey(2);
  QVERIFY(!rec);

  rec = table.getByPrimaryKey(3);
  QVERIFY(!rec);
}

void TableTest::composedPkTest()
{
  Table<Contact, ComposedPrimaryKey> table;

  Contact ca("A", "123");
  ComposedPrimaryKey pka{"A","1"};

  QVERIFY(table.add(ca, pka));

  auto rec = table.getByPrimaryKey(pka);
  QVERIFY(rec);
  QCOMPARE(rec->type(), QString("A"));

  Contact cua("uA", "u123");
  table.update(cua, pka);

  rec = table.getByPrimaryKey(pka);
  QVERIFY(rec);
  QCOMPARE(rec->type(), QString("uA"));

  table.remove(pka);

  rec = table.getByPrimaryKey(pka);
  QVERIFY(!rec);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  TableTest test;

  return QTest::qExec(&test, argc, argv);
}
