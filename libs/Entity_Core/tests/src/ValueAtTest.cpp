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
#include "ValueAtTest.h"
#include "Mdt/Entity/ValueAt.h"
#include <boost/fusion/container.hpp>
#include <QString>
#include <QMetaType>

using namespace Mdt::Entity;

void ValueAtTest::initTestCase()
{
}

void ValueAtTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void ValueAtTest::fusionVectorGetSetTest()
{
  auto v = boost::fusion::make_vector(1,QString("A"),3.2,'b');
  QCOMPARE(valueAt(v, 0), QVariant(1));
  QCOMPARE(valueAt(v, 1), QVariant("A"));
  QCOMPARE(valueAt(v, 2), QVariant(3.2));
  QCOMPARE(valueAt(v, 3), QVariant('b'));
  setValueAt(v, 0, 2);
  setValueAt(v, 1, QString("B"));
  setValueAt(v, 2, 4.2);
  setValueAt(v, 3, 'c');
  QCOMPARE(valueAt(v, 0), QVariant(2));
  QCOMPARE(valueAt(v, 1), QVariant("B"));
  QCOMPARE(valueAt(v, 2), QVariant(4.2));
  QCOMPARE(valueAt(v, 3), QVariant('c'));
}

class PersonId
{
 public:

  PersonId() = default;
  PersonId(qulonglong id)
   : mId(id)
  {
  }

  qulonglong value() const
  {
    return mId;
  }

 private:

  qulonglong mId = 0;
};
Q_DECLARE_METATYPE(PersonId)

void ValueAtTest::customTypeTest()
{
  auto v = boost::fusion::make_vector( PersonId(1), QString("A") );
  QCOMPARE(valueAt(v, 0).value<PersonId>().value(), 1ull);
  QCOMPARE(valueAt(v, 1), QVariant("A"));
  setValueAt(v, 0, PersonId(2));
  setValueAt(v, 1, QString("B"));
  QCOMPARE(valueAt(v, 0).value<PersonId>().value(), 2ull);
  QCOMPARE(valueAt(v, 1), QVariant("B"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  ValueAtTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
