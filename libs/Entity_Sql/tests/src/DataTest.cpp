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
#include "DataTest.h"
#include "Mdt/Entity/SqlData.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/DataTemplate.h"
#include <QString>
#include <QSqlRecord>
#include <QSqlField>

// #include <QDebug>

using namespace Mdt::Entity;

/*
 * Entities
 */

struct PersonDataStruct
{
  int id;
  QString firstName;
};

MDT_ENTITY_DEF(
  (PersonDataStruct),
  Person,
  (id, FieldFlag::IsPrimaryKey),
  (firstName, FieldMaxLength(250))
)

class PersonData : public DataTemplate<PersonEntity>
{
 public:

  int id() const
  {
    return constDataStruct().id;
  }

  QString firstName() const
  {
    return constDataStruct().firstName;
  }
};

/*
 * Helpers
 */

QSqlRecord buildPersonSqlRecord(int id, const QString & firstName)
{
  QSqlRecord record;

  QSqlField idField("id", QVariant::Int);
  idField.setValue(id);
  QSqlField firstNameField("firstName", QVariant::String);
  firstNameField.setValue(firstName);
  record.append(idField);
  record.append(firstNameField);

  return record;
}

/*
 * Tests
 */

void DataTest::entityDataFromQSqlRecordTest()
{
  QSqlRecord sqlRecord;
  PersonData personData;

  sqlRecord = buildPersonSqlRecord(1, "A");
  personData = SqlData::entityDataFromQSqlRecord<PersonData>(sqlRecord);
  QCOMPARE(personData.id(), 1);
  QCOMPARE(personData.firstName(), QString("A"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  DataTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}

