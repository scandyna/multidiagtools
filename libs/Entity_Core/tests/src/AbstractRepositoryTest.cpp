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
#include "AbstractRepositoryTest.h"
#include "Mdt/Entity/AbstractRepository.h"
#include <QSignalSpy>
#include <QVariantList>

using namespace Mdt::Entity;

/*
 * Repository
 */

class PersonRepository : public AbstractRepository
{
  using ParentClass = AbstractRepository;

 public:

  using ParentClass::emitOperationAtRowsChanged;
  using ParentClass::emitOperationAtRowChanged;
  using ParentClass::emitDataAtRowsChanged;
  using ParentClass::emitDataAtRowChanged;
};

/*
 * Tests
 */

void AbstractRepositoryTest::emitOperationAtRowsChangedTest()
{
  PersonRepository repository;
  QSignalSpy spy(&repository, &PersonRepository::operationAtRowsChanged);
  QVERIFY(spy.isValid());
  QVariantList arguments;

  QCOMPARE(spy.count(), 0);

  repository.emitOperationAtRowsChanged(0, 3);
  QCOMPARE(spy.count(), 1);
  arguments = spy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(0)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(3)); // lastRow

  repository.emitOperationAtRowChanged(1);
  QCOMPARE(spy.count(), 1);
  arguments = spy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(1)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(1)); // lastRow
}

void AbstractRepositoryTest::emitDataAtRowsChangedTest()
{
  PersonRepository repository;
  QSignalSpy spy(&repository, &PersonRepository::dataAtRowsChanged);
  QVERIFY(spy.isValid());
  QVariantList arguments;

  QCOMPARE(spy.count(), 0);

  repository.emitDataAtRowsChanged(0, 3);
  QCOMPARE(spy.count(), 1);
  arguments = spy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(0)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(3)); // lastRow

  repository.emitDataAtRowChanged(1);
  QCOMPARE(spy.count(), 1);
  arguments = spy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(1)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(1)); // lastRow
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  AbstractRepositoryTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
