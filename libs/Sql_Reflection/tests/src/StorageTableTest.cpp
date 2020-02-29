/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#include "StorageTableTest.h"
#include "Mdt/Sql/ReflectionStorageTableWithId.h"
#include "Mdt/Sql/ReflectionAsyncStorageTableWithId.h"
#include "Mdt/Sql/Schema/Reflection.h"
#include "Mdt/Sql/Schema/Driver.h"
// #include "Mdt/Sql/Schema/Reflection/FieldAlgorithm.h"
// #include "Mdt/Sql/Schema/Field.h"
#include "Mdt/Reflection/ReflectStruct.h"
#include "Mdt/Reflection/AutoIncrementIdPrimaryKey.h"
#include "Mdt/Numeric/IntegralUniqueIdTemplate.h"
#include "Mdt/Container/Vector.h"
#include <QString>
#include <QLatin1String>
#include <type_traits>
#include <QDebug>

using Mdt::Sql::ReflectionStorageTableWithId;
using namespace Mdt::Reflection;
using Mdt::Container::VariantRecord;

/*
 * We assume that SQLite is compiled thread safe
 * (Compile SQLite without threads support is optionnal: https://www.sqlite.org/threadsafe.html)
 *
 * The same database file will be accessed by 2 threads:
 *  - Main thread ( initDatabaseSqlite(), cleanupClientTable(), ... )
 *  - Separate thread ( initDatabaseSqliteAsync(), asyncQueryConnection(), ... )
 *
 * This way the tests have less code.
 * If this should cause problems,
 * the tests should be updated to open/close the 2 connections
 */

void StorageTableTest::initTestCase()
{
  QVERIFY(initDatabaseSqlite());
  createDatabaseSchema();
  QVERIFY(initDatabaseSqliteAsync());
}

void StorageTableTest::cleanupTestCase()
{
}

void StorageTableTest::cleanup()
{
  QVERIFY(cleanupPersonTable());
}

/*
 * Person
 */

struct PersonDataStruct
{
  int id;
  QString firstName;
  QString lastName;
};

MDT_REFLECT_STRUCT(
  (PersonDataStruct),
  Person,
  (id),
  (firstName, FieldFlag::IsRequired),
  (lastName, FieldFlag::HasDefaultValue)
)

class PersonId : public Mdt::Numeric::IntegralUniqueIdTemplate<PersonId, int>
{
 public:

  using IntegralUniqueIdTemplate::IntegralUniqueIdTemplate;
};

using PersonPrimaryKey = Mdt::Reflection::AutoIncrementIdPrimaryKey<PersonDef::id>;

/*
 * Entities
 */

struct TestCaseDataStruct
{
  qlonglong id = 0;
  QString name;
  double qty = 1.0;
};

MDT_REFLECT_STRUCT(
  (TestCaseDataStruct),
  TestCase,
  (id),
  (name, FieldFlag::IsRequired, FieldMaxLength(100)),
  (qty, FieldFlag::HasDefaultValue)
)

struct MyStruct
{
  int i = 0;
};
Q_DECLARE_METATYPE(MyStruct)

struct FieldTypeTestStruct
{
  int int_type;
  QString QString_type;
  MyStruct MyStruct_type;
};

MDT_REFLECT_STRUCT(
  (FieldTypeTestStruct),
  FieldTypeTest,
  (int_type),
  (QString_type),
  (MyStruct_type)
)

using PersonTable = Mdt::Sql::ReflectionStorageTableWithId<PersonDef, PersonPrimaryKey, PersonId::value_type>;
using AsyncPersonTable = Mdt::Sql::ReflectionAsyncStorageTableWithId<PersonDef, PersonPrimaryKey, PersonId::value_type>;

/*
 * Tests
 */

void StorageTableTest::tableWithIdTest()
{
  PersonTable table(connection());
  Mdt::Expected<int> idEx;
  Mdt::Expected<PersonDataStruct> personEx;
  PersonDataStruct person;

  /*
   * Add
   */

  person.id = 1;
  person.firstName = QLatin1String("Fn1");
  person.lastName = QLatin1String("Ln1");
  idEx = table.add(person);
  QVERIFY(idEx);
  QCOMPARE(*idEx, 1);

  // Check auto increment
  person.id = 0;
  person.firstName = QLatin1String("Fn2");
  person.lastName = QLatin1String("Ln2");
  idEx = table.add(person);
  QVERIFY(idEx);
  QCOMPARE(*idEx, 2);
  
  /// \todo Check UniqueConstraintError

  /*
   * Get
   */

  personEx = table.get(1);
  QVERIFY(personEx);
  QCOMPARE(personEx->id, 1);
  QCOMPARE(personEx->firstName, QLatin1String("Fn1"));
  QCOMPARE(personEx->lastName, QLatin1String("Ln1"));

  personEx = table.get(5);
  QVERIFY(!personEx);
  QVERIFY(personEx.error().isError(Mdt::ErrorCode::NotFound));

  /*
   * Update
   */

  person.id = 1;
  person.firstName = QLatin1String("UFn1");
  person.lastName = QLatin1String("ULn1");
  QVERIFY(table.update(person));
  personEx = table.get(1);
  QVERIFY(personEx);
  QCOMPARE(personEx->id, 1);
  QCOMPARE(personEx->firstName, QLatin1String("UFn1"));
  QCOMPARE(personEx->lastName, QLatin1String("ULn1"));

  person.id = 3;
  person.firstName = QLatin1String("Fn3");
  person.lastName = QLatin1String("Ln3");
  idEx = table.add(person);
  QVERIFY(idEx);

  /*
   * Remove
   */

  personEx = table.get(1);
  QVERIFY(personEx);
  QCOMPARE(personEx->id, 1);
  personEx = table.get(2);
  QVERIFY(personEx);
  QCOMPARE(personEx->id, 2);
  personEx = table.get(3);
  QVERIFY(personEx);
  QCOMPARE(personEx->id, 3);

  QVERIFY(table.remove(2));
  personEx = table.get(1);
  QVERIFY(personEx);
  QCOMPARE(personEx->id, 1);
  personEx = table.get(2);
  QVERIFY(!personEx);
  personEx = table.get(3);
  QVERIFY(personEx);
  QCOMPARE(personEx->id, 3);

  QVERIFY(table.removeAll());
  personEx = table.get(1);
  QVERIFY(!personEx);
  personEx = table.get(2);
  QVERIFY(!personEx);
  personEx = table.get(3);
  QVERIFY(!personEx);
}

void StorageTableTest::asyncTableWithIdAddAsyncTest()
{
  AsyncPersonTable table(asyncQueryConnection());
  QVariant id;
  Mdt::Error error;

  const auto setNewId = [&id](const QVariant & newId){
    id = newId;
  };
  connect(&table, &AsyncPersonTable::newRecordAdded, setNewId);

  const auto setError = [&error](const Mdt::Error & err){
    error = err;
  };
  connect(&table, &AsyncPersonTable::errorOccurred, setError);

  PersonDataStruct person;
  person.id = 15;
  person.firstName = QLatin1String("Fn15");
  person.lastName = QLatin1String("Ln15");

  table.addAsync(person);
  QTRY_VERIFY(!id.isNull());
  QCOMPARE(id, QVariant(15));
  QVERIFY(error.isNull());

  // Adding person with same ID must fail
  id.clear();
  table.addAsync(person);
  QTRY_VERIFY(!error.isNull());
  QVERIFY(error.isError(Mdt::ErrorCode::UniqueConstraintError));
  QVERIFY(table.lastError().isError(Mdt::ErrorCode::UniqueConstraintError));
  QVERIFY(id.isNull());

  const auto personList = table.getAll< Mdt::Container::Vector<PersonDataStruct> >();
  QVERIFY(personList);
  QCOMPARE(personList->size(), 1);
  QCOMPARE(personList->at(0).id, 15);
  QCOMPARE(personList->at(0).firstName, QLatin1String("Fn15"));
  QCOMPARE(personList->at(0).lastName, QLatin1String("Ln15"));
}

void StorageTableTest::asyncTableWithIdAddTest()
{
  AsyncPersonTable table(asyncQueryConnection());

  PersonDataStruct person;
  person.id = 22;
  person.firstName = QLatin1String("Fn22");
  person.lastName = QLatin1String("Ln22");

  auto id = table.add(person);
  QVERIFY(id);
  QCOMPARE(*id, 22);

  // Adding person with same ID must fail
  id = table.add(person);
  QVERIFY(!id);
  QVERIFY(id.error().isError(Mdt::ErrorCode::UniqueConstraintError));
  QVERIFY(table.lastError().isError(Mdt::ErrorCode::UniqueConstraintError));

  const auto personList = table.getAll< Mdt::Container::Vector<PersonDataStruct> >();
  QVERIFY(personList);
  QCOMPARE(personList->size(), 1);
  QCOMPARE(personList->at(0).id, 22);
  QCOMPARE(personList->at(0).firstName, QLatin1String("Fn22"));
  QCOMPARE(personList->at(0).lastName, QLatin1String("Ln22"));
}

void StorageTableTest::asyncTableWithIdGetAsyncTest()
{
  QVERIFY(addPerson(1,QLatin1String("Fn1"),QLatin1String("Ln1")));
  QVERIFY(addPerson(2,QLatin1String("Fn2"),QLatin1String("Ln2")));
  QVERIFY(addPerson(3,QLatin1String("Fn3"),QLatin1String("Ln3")));

  AsyncPersonTable table(asyncQueryConnection());
  VariantRecord record;
  Mdt::Error error;

  const auto setRecord = [&record](const VariantRecord & rec){
    record = rec;
  };
  connect(&table, &AsyncPersonTable::newRecordFetched, setRecord);

  const auto setError = [&error](const Mdt::Error & err){
    error = err;
  };
  connect(&table, &AsyncPersonTable::errorOccurred, setError);

  table.getAsync(1);
  QTRY_COMPARE(record.columnCount(), 3);
  QCOMPARE(record.value(0).toInt(), 1);
  QCOMPARE(record.value(1).toString(), QLatin1String("Fn1"));
  QCOMPARE(record.value(2).toString(), QLatin1String("Ln1"));
  QVERIFY(error.isNull());

  record.clear();
  table.getAsync(6);
  QTRY_VERIFY(!error.isNull());
  QVERIFY(error.isError(Mdt::ErrorCode::NotFound));
  QVERIFY(table.lastError().isError(Mdt::ErrorCode::NotFound));
  QCOMPARE(record.columnCount(), 0);
}

void StorageTableTest::asyncTableWithIdGetTest()
{
  QVERIFY(addPerson(1,QLatin1String("Fn1"),QLatin1String("Ln1")));
  QVERIFY(addPerson(2,QLatin1String("Fn2"),QLatin1String("Ln2")));
  QVERIFY(addPerson(3,QLatin1String("Fn3"),QLatin1String("Ln3")));

  AsyncPersonTable table(asyncQueryConnection());

  auto person = table.get(1);
  QVERIFY(person);
  QCOMPARE(person->id, 1);
  QCOMPARE(person->firstName, QLatin1String("Fn1"));
  QCOMPARE(person->lastName, QLatin1String("Ln1"));

  person = table.get(15);
  QVERIFY(!person);
  QVERIFY(person.error().isError(Mdt::ErrorCode::NotFound));
  QVERIFY(table.lastError().isError(Mdt::ErrorCode::NotFound));
}

void StorageTableTest::asyncTableWithIdGetAllAsyncTest()
{
  QVERIFY(addPerson(1,QLatin1String("Fn1"),QLatin1String("Ln1")));
  QVERIFY(addPerson(2,QLatin1String("Fn2"),QLatin1String("Ln2")));
  QVERIFY(addPerson(3,QLatin1String("Fn3"),QLatin1String("Ln3")));
  QVERIFY(addPerson(4,QLatin1String("Fn4"),QLatin1String("Ln4")));

  AsyncPersonTable table(asyncQueryConnection());
  Mdt::Container::Vector<VariantRecord> recordList;
  const auto addRecordToList = [&recordList](const VariantRecord & record){
    recordList.append(record);
  };
  connect(&table, &AsyncPersonTable::newRecordFetched, addRecordToList);

  table.getAllAsync();
  QTRY_COMPARE(recordList.size(), 4);
  QCOMPARE(recordList.at(0).columnCount(), 3);
  QCOMPARE(recordList.at(0).value(0).toInt(), 1);
  QCOMPARE(recordList.at(0).value(1).toString(), QLatin1String("Fn1"));
  QCOMPARE(recordList.at(0).value(2).toString(), QLatin1String("Ln1"));
  QCOMPARE(recordList.at(3).columnCount(), 3);
  QCOMPARE(recordList.at(3).value(0).toInt(), 4);
  QCOMPARE(recordList.at(3).value(1).toString(), QLatin1String("Fn4"));
  QCOMPARE(recordList.at(3).value(2).toString(), QLatin1String("Ln4"));
}

void StorageTableTest::asyncTableWithIdGetAllTest()
{
  QVERIFY(addPerson(1,QLatin1String("Fn1"),QLatin1String("Ln1")));
  QVERIFY(addPerson(2,QLatin1String("Fn2"),QLatin1String("Ln2")));

  AsyncPersonTable table(asyncQueryConnection());

  const auto personList = table.getAll< Mdt::Container::Vector<PersonDataStruct> >();
  QVERIFY(personList);
  QCOMPARE(personList->size(), 2);
  QCOMPARE(personList->at(0).id, 1);
  QCOMPARE(personList->at(0).firstName, QLatin1String("Fn1"));
  QCOMPARE(personList->at(0).lastName, QLatin1String("Ln1"));
  QCOMPARE(personList->at(1).id, 2);
  QCOMPARE(personList->at(1).firstName, QLatin1String("Fn2"));
  QCOMPARE(personList->at(1).lastName, QLatin1String("Ln2"));
}

void StorageTableTest::asyncTableWithIdUpdateAsyncTest()
{
  QVERIFY(addPerson(1,QLatin1String("Fn1"),QLatin1String("Ln1")));
  QVERIFY(addPerson(2,QLatin1String("Fn2"),QLatin1String("Ln2")));

  AsyncPersonTable table(asyncQueryConnection());

//   QVariant id;
//   const auto setUpdatedId = [&id](const QVariant & updatedId){
//     id = updatedId;
//   };
//   connect(&table, &AsyncPersonTable::updateRecordDone, setUpdatedId);

  bool finished = false;
  const auto setFinished = [&finished](){
    finished = true;
  };
  connect(&table, &AsyncPersonTable::updateRecordDone, setFinished);

  Mdt::Error error;
  const auto setError = [&error](const Mdt::Error & err){
    error = err;
  };
  connect(&table, &AsyncPersonTable::errorOccurred, setError);

  PersonDataStruct person;
  person.id = 1;
  person.firstName = QLatin1String("uFn1");
  person.lastName = QLatin1String("uLn1");
  table.updateAsync(person);
  QTRY_VERIFY(finished);
  QVERIFY(error.isNull());
//   QTRY_VERIFY(!id.isNull());
//   QCOMPARE(id, QVariant(1));

  person.id = 5;
  person.firstName = QLatin1String("uFn5");
  person.lastName = QLatin1String("uLn5");
  table.updateAsync(person);
  QTRY_VERIFY(!error.isNull());
  QVERIFY(error.isError(Mdt::ErrorCode::NotFound));
  QVERIFY(table.lastError().isError(Mdt::ErrorCode::NotFound));

  const auto personList = table.getAll< Mdt::Container::Vector<PersonDataStruct> >();
  QVERIFY(personList);
  QCOMPARE(personList->size(), 2);
  QCOMPARE(personList->at(0).id, 1);
  QCOMPARE(personList->at(0).firstName, QLatin1String("uFn1"));
  QCOMPARE(personList->at(0).lastName, QLatin1String("uLn1"));
  QCOMPARE(personList->at(1).id, 2);
  QCOMPARE(personList->at(1).firstName, QLatin1String("Fn2"));
  QCOMPARE(personList->at(1).lastName, QLatin1String("Ln2"));
}

void StorageTableTest::asyncTableWithIdUpdateTest()
{
  QVERIFY(addPerson(1,QLatin1String("Fn1"),QLatin1String("Ln1")));
  QVERIFY(addPerson(2,QLatin1String("Fn2"),QLatin1String("Ln2")));

  AsyncPersonTable table(asyncQueryConnection());

  PersonDataStruct person;
  person.id = 1;
  person.firstName = QLatin1String("uFn1");
  person.lastName = QLatin1String("uLn1");
  QVERIFY(table.update(person));

  person.id = 5;
  person.firstName = QLatin1String("uFn5");
  person.lastName = QLatin1String("uLn5");
  QVERIFY(!table.update(person));
  QVERIFY(table.lastError().isError(Mdt::ErrorCode::NotFound));

  const auto personList = table.getAll< Mdt::Container::Vector<PersonDataStruct> >();
  QVERIFY(personList);
  QCOMPARE(personList->size(), 2);
  QCOMPARE(personList->at(0).id, 1);
  QCOMPARE(personList->at(0).firstName, QLatin1String("uFn1"));
  QCOMPARE(personList->at(0).lastName, QLatin1String("uLn1"));
  QCOMPARE(personList->at(1).id, 2);
  QCOMPARE(personList->at(1).firstName, QLatin1String("Fn2"));
  QCOMPARE(personList->at(1).lastName, QLatin1String("Ln2"));
}

void StorageTableTest::asyncTableWithIdRemoveAsyncTest()
{
  QVERIFY(addPerson(1,QLatin1String("Fn1"),QLatin1String("Ln1")));
  QVERIFY(addPerson(2,QLatin1String("Fn2"),QLatin1String("Ln2")));

  AsyncPersonTable table(asyncQueryConnection());

  bool finished = false;
  const auto setFinished = [&finished](){
    finished = true;
  };
  connect(&table, &AsyncPersonTable::removeRecordDone, setFinished);

  table.removeAsync(1);
  QTRY_VERIFY(finished);

  const auto personList = table.getAll< Mdt::Container::Vector<PersonDataStruct> >();
  QVERIFY(personList);
  QCOMPARE(personList->size(), 1);
  QCOMPARE(personList->at(0).id, 2);
  QCOMPARE(personList->at(0).firstName, QLatin1String("Fn2"));
  QCOMPARE(personList->at(0).lastName, QLatin1String("Ln2"));
}

void StorageTableTest::asyncTableWithIdRemoveTest()
{
  QVERIFY(addPerson(1,QLatin1String("Fn1"),QLatin1String("Ln1")));
  QVERIFY(addPerson(2,QLatin1String("Fn2"),QLatin1String("Ln2")));

  AsyncPersonTable table(asyncQueryConnection());

  QVERIFY(table.remove(1));

  const auto personList = table.getAll< Mdt::Container::Vector<PersonDataStruct> >();
  QVERIFY(personList);
  QCOMPARE(personList->size(), 1);
  QCOMPARE(personList->at(0).id, 2);
  QCOMPARE(personList->at(0).firstName, QLatin1String("Fn2"));
  QCOMPARE(personList->at(0).lastName, QLatin1String("Ln2"));
}

void StorageTableTest::asyncTableWithIdRemoveAllAsyncTest()
{
  QVERIFY(addPerson(1,QLatin1String("Fn1"),QLatin1String("Ln1")));
  QVERIFY(addPerson(2,QLatin1String("Fn2"),QLatin1String("Ln2")));

  AsyncPersonTable table(asyncQueryConnection());

  bool finished = false;
  const auto setFinished = [&finished](){
    finished = true;
  };
  connect(&table, &AsyncPersonTable::removeRecordDone, setFinished);

  table.removeAllAsync();
  QTRY_VERIFY(finished);

  const auto personList = table.getAll< Mdt::Container::Vector<PersonDataStruct> >();
  QVERIFY(personList);
  QCOMPARE(personList->size(), 0);
}

void StorageTableTest::asyncTableWithIdRemoveAllTest()
{
  QVERIFY(addPerson(1,QLatin1String("Fn1"),QLatin1String("Ln1")));
  QVERIFY(addPerson(2,QLatin1String("Fn2"),QLatin1String("Ln2")));

  AsyncPersonTable table(asyncQueryConnection());

  QVERIFY(table.removeAll());

  const auto personList = table.getAll< Mdt::Container::Vector<PersonDataStruct> >();
  QVERIFY(personList);
  QCOMPARE(personList->size(), 0);
}

/*
 * Helpers
 */

void StorageTableTest::createDatabaseSchema()
{
  Mdt::Sql::Schema::Driver driver(connection());
  QVERIFY(driver.isValid());

  auto personTable = Mdt::Sql::Schema::tableFromReflected<PersonDef, PersonPrimaryKey>();
  QVERIFY(driver.createTable(personTable));
}

bool StorageTableTest::addPerson(int id, const QString & firstName, const QString & lastName)
{
  Q_ASSERT(id > 0);

  PersonTable table(connection());
  PersonDataStruct person;
  person.id = id;
  person.firstName = firstName;
  person.lastName = lastName;

  if(!table.add(person)){
    qWarning() << "Add person '" << id << "," << firstName << "," << lastName << "' failed: " << table.lastError().text();
    return false;
  }

  return true;
}

bool StorageTableTest::cleanupPersonTable()
{
  PersonTable table(connection());

  if(!table.removeAll()){
    qWarning() << "Removing all person from database failed: " << table.lastError().text();
    return false;
  }

  return true;
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  StorageTableTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
