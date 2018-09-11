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
#include "EntitySelectStatementTest.h"
#include "Mdt/Entity/EntitySelectStatement.h"
#include "Mdt/Entity/QueryEntity.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/QueryExpression/SelectField.h"
#include <boost/variant.hpp>

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
 * Query objects
 */

/*
 * Helper to get entity alias or name
 */

struct GetEntityAliasOrNameVisitor : public boost::static_visitor<>
{
  void operator()(const Mdt::QueryExpression::SelectAllField & field)
  {
    aliasOrName = field.entityAliasOrName();
  }

  void operator()(const Mdt::QueryExpression::EntityAndField & field)
  {
    aliasOrName = field.entityAliasOrName();
  }

  QString aliasOrName;
};

QString getEntityAliasOrName(const Mdt::QueryExpression::SelectField & field)
{
  GetEntityAliasOrNameVisitor visitor;
  boost::apply_visitor(visitor, field.internalVariant().internalVariant());
  return visitor.aliasOrName;
}

/*
 * Helper to get field alias or name
 */

struct GetFieldAliasOrNameVisitor : public boost::static_visitor<>
{
  void operator()(const Mdt::QueryExpression::SelectAllField &)
  {
  }

  void operator()(const Mdt::QueryExpression::EntityAndField & field)
  {
    aliasOrName = field.fieldAliasOrName();
  }

  QString aliasOrName;
};

QString getFieldAliasOrName(const Mdt::QueryExpression::SelectField & field)
{
  GetFieldAliasOrNameVisitor visitor;
  boost::apply_visitor(visitor, field.internalVariant().internalVariant());
  return visitor.aliasOrName;
}

/*
 * Tests
 */

void EntitySelectStatementTest::queryInstanceTest()
{
  EntitySelectStatement<PersonEntity> stm1;
  QCOMPARE(stm1.entity().aliasOrName(), QString("Person"));

  stm1.clear();
  QCOMPARE(stm1.entity().aliasOrName(), QString("Person"));

  EntitySelectStatement<AddressEntity> stm2("ADR");
  QCOMPARE(stm2.entity().aliasOrName(), QString("ADR"));

  stm2.clear();
  QCOMPARE(stm2.entity().aliasOrName(), QString("ADR"));
}

void EntitySelectStatementTest::addFieldTest()
{
  EntitySelectStatement<PersonEntity> stm;

  stm.addField( stm.def().firstName() );
  stm.addField( stm.def().age(), "A" );
  auto fieldList = stm.fieldList();
  QCOMPARE(fieldList.fieldCount(), 2);
  QCOMPARE(getEntityAliasOrName(fieldList.at(0)), QString("Person"));
  QCOMPARE(getFieldAliasOrName(fieldList.at(0)), QString("firstName"));
  QCOMPARE(getEntityAliasOrName(fieldList.at(1)), QString("Person"));
  QCOMPARE(getFieldAliasOrName(fieldList.at(1)), QString("A"));

  stm.selectAllFields();
  fieldList = stm.fieldList();
  QCOMPARE(fieldList.fieldCount(), 1);
  QVERIFY(getEntityAliasOrName(fieldList.at(0)).isEmpty());
  QVERIFY(getFieldAliasOrName(fieldList.at(0)).isEmpty());

  stm.clear();
  fieldList = stm.fieldList();
  QCOMPARE(fieldList.fieldCount(), 0);

  const auto firstName = stm.makeSelectField( stm.def().firstName() );
  const auto age = stm.makeSelectField( stm.def().age(), "A" );
  stm.addField(firstName);
  stm.addField(age);
  fieldList = stm.fieldList();
  QCOMPARE(fieldList.fieldCount(), 2);
  QCOMPARE(getEntityAliasOrName(fieldList.at(0)), QString("Person"));
  QCOMPARE(getFieldAliasOrName(fieldList.at(0)), QString("firstName"));
  QCOMPARE(getEntityAliasOrName(fieldList.at(1)), QString("Person"));
  QCOMPARE(getFieldAliasOrName(fieldList.at(1)), QString("A"));
}

void EntitySelectStatementTest::queryEntityTest()
{
  QueryEntity<PersonEntity> person;
  QCOMPARE(person.toSelectEntity().aliasOrName(), QString("Person"));

  const auto personId = person.makeSelectField( person.def().id() );
  QCOMPARE(getEntityAliasOrName(personId), QString("Person"));
  QCOMPARE(getFieldAliasOrName(personId), QString("id"));
  const auto firstName = person.makeSelectField( person.def().firstName(), "PersonFirstName" );
  QCOMPARE(getEntityAliasOrName(firstName), QString("Person"));
  QCOMPARE(getFieldAliasOrName(firstName), QString("PersonFirstName"));

  QueryEntity<AddressEntity> address("ADR");
  QCOMPARE(address.toSelectEntity().aliasOrName(), QString("ADR"));

  const auto addressPersonId = address.makeSelectField( address.def().personId() );
  QCOMPARE(getEntityAliasOrName(addressPersonId), QString("ADR"));
  QCOMPARE(getFieldAliasOrName(addressPersonId), QString("personId"));
  const auto addressStreet = address.makeSelectField( address.def().street(), "AddressStreet" );
  QCOMPARE(getEntityAliasOrName(addressStreet), QString("ADR"));
  QCOMPARE(getFieldAliasOrName(addressStreet), QString("AddressStreet"));
}

void EntitySelectStatementTest::addFieldMultiEntityTest()
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
  auto fieldList = stm.fieldList();
  QCOMPARE(fieldList.fieldCount(), 4);
  QCOMPARE(getEntityAliasOrName(fieldList.at(0)), QString("Person"));
  QCOMPARE(getFieldAliasOrName(fieldList.at(0)), QString("firstName"));
  QCOMPARE(getEntityAliasOrName(fieldList.at(1)), QString("Person"));
  QCOMPARE(getFieldAliasOrName(fieldList.at(1)), QString("remarks"));
  QCOMPARE(getEntityAliasOrName(fieldList.at(2)), QString("ADR"));
  QCOMPARE(getFieldAliasOrName(fieldList.at(2)), QString("AddressStreet"));
  QCOMPARE(getEntityAliasOrName(fieldList.at(3)), QString("ADR"));
  QCOMPARE(getFieldAliasOrName(fieldList.at(3)), QString("AddressRemarks"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  EntitySelectStatementTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
