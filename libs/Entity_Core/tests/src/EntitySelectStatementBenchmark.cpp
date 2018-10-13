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
#include "EntitySelectStatementBenchmark.h"
#include "Mdt/Entity/EntitySelectStatement.h"
#include "Mdt/Entity/QueryEntity.h"
#include "Mdt/Entity/Def.h"

using namespace Mdt::Entity;

/*
 * Entities
 */

struct PersonDataStruct
{
  qulonglong id;
  QString firstName;
  int age;
  QString remarks;
};

MDT_ENTITY_DEF(
  (PersonDataStruct),
  Person,
  (id, FieldFlag::IsPrimaryKey),
  (firstName, FieldMaxLength(5)),
  (age),
  (remarks)
)

struct AddressDataStruct
{
  qulonglong id;
  QString street;
  qulonglong personId;
  QString remarks;
};

MDT_ENTITY_DEF(
  (AddressDataStruct),
  Address,
  (id, FieldFlag::IsPrimaryKey),
  (street, FieldMaxLength(20)),
  (personId),
  (remarks)
)

/*
 * Benchmarks
 */

void EntitySelectStatementBenchmark::fieldIndexBenchmark()
{
  QueryEntity<PersonEntity> person;
  QueryEntity<AddressEntity> address("ADR");

  const auto personId = person.makeSelectField( person.def().id() );
  const auto personName = person.makeSelectField( person.def().firstName() );
  const auto addressPersonId = address.makeSelectField( address.def().personId() );
  const auto addressStreet = address.makeSelectField( address.def().street(), "AddressStreet" );

  EntitySelectStatement<PersonEntity> stm;
  stm.addField( personName );
  stm.addField( person, person.def().remarks() );
  stm.addField( addressStreet );
  stm.addField( address, address.def().remarks(), "AddressRemarks" );

  int index;
  QBENCHMARK{
    index = stm.fieldIndex( address, address.def().street() );
  }
  QCOMPARE(index, 2);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  EntitySelectStatementBenchmark test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
