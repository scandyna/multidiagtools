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
#include "StorageTableBenchmark.h"
#include "Mdt/Reflection/ReflectStruct.h"
#include "Mdt/Container/VariantRecord.h"

using Mdt::Container::VariantRecord;

void StorageTableBenchmark::initTestCase()
{
//   QVERIFY(initDatabaseSqlite());
//   createDatabaseSchema();
//   QVERIFY(initDatabaseSqliteAsync());
}

void StorageTableBenchmark::cleanupTestCase()
{
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

class Person
{
 public:

  int id() const
  {
    return mDataStruct.id;
  }

  QString firstName() const
  {
    return mDataStruct.firstName;
  }

  QString lastName() const
  {
    return mDataStruct.lastName;
  }

  const PersonDataStruct & dataStruct() const
  {
    return mDataStruct;
  }

  static Person fromDataStruct(const PersonDataStruct & dataStruct)
  {
    Person person;

    person.mDataStruct = dataStruct;

    return person;
  }

 private:

  PersonDataStruct mDataStruct;
};

struct GetPersonRecord
{
  int id;
  QString firstName;
  QString lastName;
};

/*
 * Data conversion helpers
 */

PersonDataStruct personDataStructFromVariantRecord(const VariantRecord & record)
{
  Q_ASSERT(record.columnCount() == 3);

  PersonDataStruct dataStruct;

  dataStruct.id = record.value(0).toInt();
  dataStruct.firstName = record.value(1).toString();
  dataStruct.lastName = record.value(2).toString();

  return dataStruct;
}

GetPersonRecord getPersonRecordFromPerson(const Person & person)
{
  GetPersonRecord record;

  record.id = person.id();
  record.firstName = person.firstName();
  record.lastName = person.lastName();

  return record;
}

/*
 * Benchmarks
 */

void StorageTableBenchmark::vRecordToDataStructToDomainObjectToDtoBenchmark()
{
  VariantRecord variantRecord{1,QLatin1String("Fn1"),QLatin1String("Ln1")};
  GetPersonRecord getPersonRecord;

  QBENCHMARK{
    auto dataStruct = personDataStructFromVariantRecord(variantRecord);
    auto person = Person::fromDataStruct(dataStruct);
    getPersonRecord = getPersonRecordFromPerson(person);
  }

  QCOMPARE(getPersonRecord.id, 1);
  QCOMPARE(getPersonRecord.firstName, QLatin1String("Fn1"));
  QCOMPARE(getPersonRecord.lastName, QLatin1String("Ln1"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  StorageTableBenchmark test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
