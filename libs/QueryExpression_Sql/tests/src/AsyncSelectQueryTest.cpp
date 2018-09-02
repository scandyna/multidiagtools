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
#include "AsyncSelectQueryTest.h"
#include "AsyncTestQueryReceiver.h"
#include "Mdt/QueryExpression/SqlAsyncSelectQuery.h"
#include "Mdt/QueryExpression/SqlAsyncSelectQueryFactory.h"
#include "Mdt/Entity/EntitySelectStatement.h"
#include "Mdt/Container/VariantRecord.h"
#include <memory>

using namespace Mdt::Entity;
using namespace Mdt::QueryExpression;
using Mdt::Container::VariantRecord;

/*
 * Test use case
 */

CollectPersons::CollectPersons(QObject* parent)
 : QObject(parent)
{
}

void CollectPersons::setSelectQuery(std::unique_ptr<AsyncSelectQuery> && query)
{
  Q_ASSERT(query.get() != nullptr);

  mSelectQuery = std::move(query);
  connect(mSelectQuery.get(), &AsyncSelectQuery::newRecordAvailable, this, &CollectPersons::storeNewRecord);
  connect(mSelectQuery.get(), &AsyncSelectQuery::errorOccured, this, &CollectPersons::setLastError);
}

void CollectPersons::getAllPerson()
{
  EntitySelectStatement<PersonEntity> stm;
  const auto id = stm.makeSelectField( stm.def().id() );
  const auto firstName = stm.makeSelectField( stm.def().firstName() );
  const auto age = stm.makeSelectField( stm.def().age(), "PA" );
  const auto remarks = stm.makeSelectField( stm.def().remarks() );
  stm.addField(id);
  stm.addField(firstName);
  stm.addField(age);
  stm.addField(remarks);

  mSelectQuery->submitStatement(stm);
}

void CollectPersons::storeNewRecord(const Mdt::Container::VariantRecord& record)
{
  mRecordList.push_back(record);
}

void CollectPersons::setLastError(const Mdt::Error& error)
{
  mLastError = error;
}

/*
 * Helpers
 */

void setupReceiver(AsyncTestQueryReceiver & receiver, const AbstractAsyncSelectQuery *query)
{
  QObject::connect(query, &AbstractAsyncSelectQuery::newRecordAvailable, &receiver, &AsyncTestQueryReceiver::storeNewRecord);
}

void setupReceiver(AsyncTestQueryReceiver & receiver, const std::unique_ptr<AbstractAsyncSelectQuery> & query)
{
  setupReceiver(receiver, query.get());
}

// void setupReceiver(AsyncTestQueryReceiver & receiver, const AbstractAsyncSelectQuery & query)
// {
//   setupReceiver(receiver, &query);
// }

/*
 * Init / cleanup
 */

void AsyncSelectQueryTest::initTestCase()
{
  QVERIFY(initDatabaseSqlite());
  QVERIFY(createTestSchema());
}

void AsyncSelectQueryTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void AsyncSelectQueryTest::execQueryTest()
{
  QVERIFY(cleanupPersonTable());
  QVERIFY(insertPerson(1, "P1", 10, "R1"));
  QVERIFY(insertPerson(2, "P2", 20, "R2"));

  const int timeout = 1000;

  SqlAsyncSelectQueryFactory factory;
  QVERIFY(factory.setup( connectionParameters() ));
  auto query = factory.createSelectQuery();
  AsyncTestQueryReceiver receiver;
  setupReceiver(receiver, query);

  EntitySelectStatement<PersonEntity> stm;
  stm.selectAllFields();

  query->submitStatement(stm);
  QTRY_COMPARE_WITH_TIMEOUT(receiver.recordCount(), 2, timeout);
}

void AsyncSelectQueryTest::execQueryMaxRowsTest()
{
  const int timeout = 1000;

  QVERIFY(cleanupPersonTable());
  QVERIFY(insertPerson(1, "P1", 10, "R1"));
  QVERIFY(insertPerson(2, "P2", 20, "R2"));

  SqlAsyncSelectQueryFactory factory;
  QVERIFY(factory.setup( connectionParameters() ));
  auto query = factory.createSelectQuery();
  AsyncTestQueryReceiver receiver;
  setupReceiver(receiver, query);

  EntitySelectStatement<PersonEntity> stm;
  stm.selectAllFields();

  query->submitStatement(stm, 1);
  QTRY_VERIFY_WITH_TIMEOUT(receiver.hasResult(), timeout);
  QTest::qWait(1000);
  QCOMPARE(receiver.recordCount(), 1);
}

void AsyncSelectQueryTest::fieldOrderTest()
{
  const int timeout = 1000;

  QVERIFY(cleanupPersonTable());
  QVERIFY(insertPerson(1, "P1", 10, "R1"));
  QVERIFY(insertPerson(2, "P2", 20, "R2"));

  SqlAsyncSelectQueryFactory factory;
  QVERIFY(factory.setup( connectionParameters() ));
  auto query = factory.createSelectQuery();
  AsyncTestQueryReceiver receiver;
  setupReceiver(receiver, query);
  VariantRecord record;

  EntitySelectStatement<PersonEntity> stm;
  const auto firstName = stm.makeSelectField( stm.def().firstName() );
  const auto age = stm.makeSelectField( stm.def().age(), "PA" );
  const auto remarks = stm.makeSelectField( stm.def().remarks() );
  stm.addField(age);
  stm.addField(remarks);
  stm.addField(firstName);
  query->submitStatement(stm);
  QTRY_COMPARE_WITH_TIMEOUT(receiver.recordCount(), 2, timeout);
  record = receiver.recordAt(0);
  QCOMPARE(record.columnCount(), 3);
  QCOMPARE(record.value(0), QVariant(10));
  QCOMPARE(record.value(1), QVariant("R1"));
  QCOMPARE(record.value(2), QVariant("P1"));
  record = receiver.recordAt(1);
  QCOMPARE(record.columnCount(), 3);
  QCOMPARE(record.value(0), QVariant(20));
  QCOMPARE(record.value(1), QVariant("R2"));
  QCOMPARE(record.value(2), QVariant("P2"));

  stm.clear();
  stm.addField(firstName);
  stm.addField(age);
  stm.addField(remarks);
  receiver.clear();
  query->submitStatement(stm);
  QTRY_COMPARE_WITH_TIMEOUT(receiver.recordCount(), 2, timeout);
  record = receiver.recordAt(0);
  QCOMPARE(record.columnCount(), 3);
  QCOMPARE(record.value(0), QVariant("P1"));
  QCOMPARE(record.value(1), QVariant(10));
  QCOMPARE(record.value(2), QVariant("R1"));
  record = receiver.recordAt(1);
  QCOMPARE(record.columnCount(), 3);
  QCOMPARE(record.value(0), QVariant("P2"));
  QCOMPARE(record.value(1), QVariant(20));
  QCOMPARE(record.value(2), QVariant("R2"));
}

void AsyncSelectQueryTest::collectPersonsUseCaseTest()
{
  const int timeout = 1000;

  QVERIFY(cleanupPersonTable());
  QVERIFY(insertPerson(1, "P1", 10, "R1"));
  QVERIFY(insertPerson(2, "P2", 20, "R2"));

  VariantRecord record;
  CollectPersons useCase;
  SqlAsyncSelectQueryFactory factory;
  QVERIFY(factory.setup( connectionParameters() ));
  useCase.setSelectQuery( factory.createSelectQuery() );

  useCase.getAllPerson();
  QTRY_COMPARE_WITH_TIMEOUT(useCase.recordCount(), 2, timeout);
  record = useCase.recordAt(0);
  QCOMPARE(record.columnCount(), 4);
  QCOMPARE(record.value(0), QVariant(1));
  QCOMPARE(record.value(1), QVariant("P1"));
  QCOMPARE(record.value(2), QVariant(10));
  QCOMPARE(record.value(3), QVariant("R1"));
  record = useCase.recordAt(1);
  QCOMPARE(record.columnCount(), 4);
  QCOMPARE(record.value(0), QVariant(2));
  QCOMPARE(record.value(1), QVariant("P2"));
  QCOMPARE(record.value(2), QVariant(20));
  QCOMPARE(record.value(3), QVariant("R2"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  AsyncSelectQueryTest test;

  return QTest::qExec(&test, argc, argv);
}
