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
#include "SelectFieldTest.h"
#include "Mdt/QueryExpression/SelectField.h"
#include "Mdt/QueryExpression/QueryField.h"

#include "Mdt/QueryExpression/EntityName.h"
#include "Mdt/QueryExpression/FieldName.h"

#include <QLatin1String>
#include <boost/variant/get.hpp>
#include <boost/proto/matches.hpp>

using namespace Mdt::QueryExpression;

/*
 * Runtime tests
 */

void SelectFieldTest::entityNameTest()
{
  EntityName e0("");
  QVERIFY(e0.isNull());

  EntityName e1("A");
  QVERIFY(!e1.isNull());
  QCOMPARE(e1.toString(), QString("A"));

  EntityName e2(" B ");
  QVERIFY(!e2.isNull());
  QCOMPARE(e2.toString(), QString("B"));

  EntityName e3(" ");
  QVERIFY(e3.isNull());
}

void SelectFieldTest::fieldNameTest()
{
  FieldName f0("");
  QVERIFY(f0.isNull());

  FieldName f1("A");
  QVERIFY(!f1.isNull());
  QCOMPARE(f1.toString(), QString("A"));

  FieldName f2(" B ");
  QVERIFY(!f2.isNull());
  QCOMPARE(f2.toString(), QString("B"));

  FieldName f3(" ");
  QVERIFY(f3.isNull());
}

void SelectFieldTest::selectEntityTest()
{
  SelectEntity person1( EntityName("Person1") );
  QCOMPARE(person1.name(), QString("Person1"));
  QVERIFY(person1.alias().isEmpty());
  QCOMPARE(person1.aliasOrName(), QString("Person1"));

  SelectEntity person2( EntityName("Person2"), "P2" );
  QCOMPARE(person2.name(), QString("Person2"));
  QCOMPARE(person2.alias(), QString("P2"));
  QCOMPARE(person2.aliasOrName(), QString("P2"));

  SelectEntity se;
  QVERIFY(se.isNull());
  se.setNameAndAlias( EntityName("Entity"), "E" );
  QVERIFY(!se.isNull());
  QCOMPARE(se.name(), QString("Entity"));
  QCOMPARE(se.alias(), QString("E"));
  QCOMPARE(se.aliasOrName(), QString("E"));
  se.clear();
  QVERIFY(se.isNull());
  QVERIFY(se.name().isEmpty());
  QVERIFY(se.alias().isEmpty());
  QVERIFY(se.aliasOrName().isEmpty());
}

void SelectFieldTest::selectAllFieldTest()
{
  SelectAllField f1;
  QVERIFY(f1.entityAliasOrName().isEmpty());

  QueryEntity person("Person", EntityAlias("P"));
  SelectAllField f2(person);
  QCOMPARE(f2.entityAliasOrName(), QLatin1String("P"));
}

void SelectFieldTest::constructGetTest()
{
  QueryEntity person("Person");
  QueryEntity address("Address", EntityAlias("ADR"));

  SelectField nullSelectField;
  QVERIFY( nullSelectField.isNull() );
  QVERIFY( boost::get<SelectAllField>(&nullSelectField.internalVariant().internalVariant()) == nullptr );
  QVERIFY( boost::get<EntityAndField>(&nullSelectField.internalVariant().internalVariant()) == nullptr );

  SelectField selectAll1(SelectAllField{});
  QVERIFY( !selectAll1.isNull() );
  QVERIFY( boost::get<SelectAllField>(&selectAll1.internalVariant().internalVariant()) != nullptr );
  QVERIFY( boost::get<EntityAndField>(&selectAll1.internalVariant().internalVariant()) == nullptr );
  const auto selectAllField1 = boost::get<SelectAllField>(selectAll1.internalVariant().internalVariant());
  QVERIFY(selectAllField1.entityAliasOrName().isEmpty());

  SelectField selectAll2(SelectAllField{person});
  QVERIFY( !selectAll2.isNull() );
  QVERIFY( boost::get<SelectAllField>(&selectAll2.internalVariant().internalVariant()) != nullptr );
  QVERIFY( boost::get<EntityAndField>(&selectAll2.internalVariant().internalVariant()) == nullptr );
  const auto selectAllField2 = boost::get<SelectAllField>(selectAll2.internalVariant().internalVariant());
  QCOMPARE(selectAllField2.entityAliasOrName(), QString("Person"));

  SelectField name("name");
  QVERIFY( !name.isNull() );
  QVERIFY( boost::get<SelectAllField>(&name.internalVariant().internalVariant()) == nullptr );
  QVERIFY( boost::get<EntityAndField>(&name.internalVariant().internalVariant()) != nullptr );
  const auto nameEaF = boost::get<EntityAndField>(name.internalVariant().internalVariant());
  QCOMPARE(nameEaF.fieldName(), QString("name"));
  QCOMPARE(nameEaF.fieldAliasOrName(), QString("name"));

  SelectField age( "age", FieldAlias("A") );
  QVERIFY( boost::get<SelectAllField>(&age.internalVariant().internalVariant()) == nullptr );
  QVERIFY( boost::get<EntityAndField>(&age.internalVariant().internalVariant()) != nullptr );
  const auto ageEaF = boost::get<EntityAndField>(age.internalVariant().internalVariant());
  QCOMPARE(ageEaF.fieldName(), QString("age"));
  QCOMPARE(ageEaF.fieldAlias(), QString("A"));
  QCOMPARE(ageEaF.fieldAliasOrName(), QString("A"));

  SelectField personName(person, "name");
  QVERIFY( boost::get<SelectAllField>(&personName.internalVariant().internalVariant()) == nullptr );
  QVERIFY( boost::get<EntityAndField>(&personName.internalVariant().internalVariant()) != nullptr );
  const auto personNameEaF = boost::get<EntityAndField>(personName.internalVariant().internalVariant());
  QCOMPARE(personNameEaF.entityName(), QString("Person"));
  QCOMPARE(personNameEaF.entityAliasOrName(), QString("Person"));
  QCOMPARE(personNameEaF.fieldName(), QString("name"));
  QCOMPARE(personNameEaF.fieldAliasOrName(), QString("name"));

  SelectField addressStreet( address, "street", FieldAlias("AddressStreet") );
  QVERIFY( boost::get<SelectAllField>(&addressStreet.internalVariant().internalVariant()) == nullptr );
  QVERIFY( boost::get<EntityAndField>(&addressStreet.internalVariant().internalVariant()) != nullptr );
  const auto addressStreetEaF = boost::get<EntityAndField>(addressStreet.internalVariant().internalVariant());
  QCOMPARE(addressStreetEaF.entityName(), QString("Address"));
  QCOMPARE(addressStreetEaF.entityAlias(), QString("ADR"));
  QCOMPARE(addressStreetEaF.entityAliasOrName(), QString("ADR"));
  QCOMPARE(addressStreetEaF.fieldName(), QString("street"));
  QCOMPARE(addressStreetEaF.fieldAlias(), QString("AddressStreet"));
  QCOMPARE(addressStreetEaF.fieldAliasOrName(), QString("AddressStreet"));

  QueryField addressStreet2QF( address, "street2", FieldAlias("AddressStreet2") );
  SelectField addressStreet2(addressStreet2QF);
  QVERIFY( boost::get<SelectAllField>(&addressStreet2.internalVariant().internalVariant()) == nullptr );
  QVERIFY( boost::get<EntityAndField>(&addressStreet2.internalVariant().internalVariant()) != nullptr );
  const auto addressStreetEaF2 = boost::get<EntityAndField>(addressStreet2.internalVariant().internalVariant());
  QCOMPARE(addressStreetEaF2.entityName(), QLatin1String("Address"));
  QCOMPARE(addressStreetEaF2.entityAlias(), QLatin1String("ADR"));
  QCOMPARE(addressStreetEaF2.entityAliasOrName(), QLatin1String("ADR"));
  QCOMPARE(addressStreetEaF2.fieldName(), QLatin1String("street2"));
  QCOMPARE(addressStreetEaF2.fieldAlias(), QLatin1String("AddressStreet2"));
  QCOMPARE(addressStreetEaF2.fieldAliasOrName(), QLatin1String("AddressStreet2"));
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SelectFieldTest test;

  return QTest::qExec(&test, argc, argv);
}
