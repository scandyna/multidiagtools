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
#include "QueryFieldTest.h"
#include "Mdt/QueryExpression/QueryField.h"
#include "Mdt/QueryExpression/FieldAlias.h"
#include "Mdt/QueryExpression/QueryEntity.h"
#include "Mdt/QueryExpression/EntityAndField.h"
#include "Mdt/QueryExpression/QueryFieldVariant.h"
#include <boost/variant/get.hpp>

using namespace Mdt::QueryExpression;

/*
 * Tests
 */

void QueryFieldTest::entityAliasTest()
{
  EntityAlias ea0;
  QVERIFY(ea0.isNull());
  QVERIFY(ea0.toString().isEmpty());

  EntityAlias ea1("A");
  QVERIFY(!ea1.isNull());
  QCOMPARE(ea1.toString(), QString("A"));

  EntityAlias ea2(" B ");
  QVERIFY(!ea2.isNull());
  QCOMPARE(ea2.toString(), QString("B"));
}

void QueryFieldTest::fieldAliasTest()
{
  FieldAlias fa0;
  QVERIFY(fa0.isNull());
  QVERIFY(fa0.toString().isEmpty());

  FieldAlias fa1("A");
  QVERIFY(!fa1.isNull());
  QCOMPARE(fa1.toString(), QString("A"));

  FieldAlias fa2(" B ");
  QVERIFY(!fa2.isNull());
  QCOMPARE(fa2.toString(), QString("B"));
}

void QueryFieldTest::queryEntityTest()
{
  QueryEntity person1("Person");
  QVERIFY(!person1.isNull());
  QCOMPARE(person1.name(), QString("Person"));
  QVERIFY(!person1.hasAlias());
  QVERIFY(person1.alias().isEmpty());
  QCOMPARE(person1.aliasOrName(), QString("Person"));

  QueryEntity address1("Address", EntityAlias("ADR"));
  QVERIFY(!address1.isNull());
  QCOMPARE(address1.name(), QString("Address"));
  QVERIFY(address1.hasAlias());
  QCOMPARE(address1.alias(), QString("ADR"));
  QCOMPARE(address1.aliasOrName(), QString("ADR"));

  QueryEntity qe;
  QVERIFY(qe.isNull());
  QVERIFY(qe.name().isEmpty());
  QVERIFY(!qe.hasAlias());
  QVERIFY(qe.alias().isEmpty());
  QVERIFY(qe.aliasOrName().isEmpty());

  qe.setNameAndAlias("Ea", EntityAlias("A"));
  QVERIFY(!qe.isNull());
  QCOMPARE(qe.name(), QString("Ea"));
  QVERIFY(qe.hasAlias());
  QCOMPARE(qe.alias(), QString("A"));
  QCOMPARE(qe.aliasOrName(), QString("A"));

  qe.clear();
  QVERIFY(qe.isNull());
  QVERIFY(qe.name().isEmpty());
  QVERIFY(!qe.hasAlias());
  QVERIFY(qe.alias().isEmpty());
  QVERIFY(qe.aliasOrName().isEmpty());
}

void QueryFieldTest::entityAndFieldTest()
{
  EntityAndField ef1("fn1");
  QVERIFY(ef1.entityName().isEmpty());
  QVERIFY(ef1.entityAlias().isEmpty());
  QVERIFY(ef1.entityAliasOrName().isEmpty());
  QCOMPARE(ef1.fieldName(), QString("fn1"));
  QVERIFY(ef1.fieldAlias().isEmpty());
  QCOMPARE(ef1.fieldAliasOrName(), QString("fn1"));

  EntityAndField ef2(" fn2 ", FieldAlias("F2") );
  QVERIFY(ef2.entityName().isEmpty());
  QVERIFY(ef2.entityAlias().isEmpty());
  QVERIFY(ef2.entityAliasOrName().isEmpty());
  QCOMPARE(ef2.fieldName(), QString("fn2"));
  QCOMPARE(ef2.fieldAlias(), QString("F2"));
  QCOMPARE(ef2.fieldAliasOrName(), QString("F2"));

  QueryEntity e3( "en3", EntityAlias("E3") );
  EntityAndField ef3( e3, "fn3", FieldAlias("F3") );
  QCOMPARE(ef3.entityName(), QString("en3"));
  QCOMPARE(ef3.entityAlias(), QString("E3"));
  QCOMPARE(ef3.entityAliasOrName(), QString("E3"));
  QCOMPARE(ef3.fieldName(), QString("fn3"));
  QCOMPARE(ef3.fieldAlias(), QString("F3"));
  QCOMPARE(ef3.fieldAliasOrName(), QString("F3"));
}

void QueryFieldTest::queryFieldVariantTest()
{
  QueryFieldVariant nullField;
  QVERIFY( nullField.isNull() );
  QVERIFY( boost::get<SelectAllField>(&nullField.internalVariant()) == nullptr );
  QVERIFY( boost::get<EntityAndField>(&nullField.internalVariant()) == nullptr );

  QueryFieldVariant selectAll1(SelectAllField{});
  QVERIFY( !selectAll1.isNull() );
  QVERIFY( boost::get<SelectAllField>(&selectAll1.internalVariant()) != nullptr );
  QVERIFY( boost::get<EntityAndField>(&selectAll1.internalVariant()) == nullptr );
  const auto selectAllField1 = boost::get<SelectAllField>(selectAll1.internalVariant());
  QVERIFY(selectAllField1.entityAliasOrName().isEmpty());

  QueryFieldVariant selectAll2(SelectAllField(QueryEntity("Person")));
  QVERIFY( !selectAll2.isNull() );
  QVERIFY( boost::get<SelectAllField>(&selectAll2.internalVariant()) != nullptr );
  QVERIFY( boost::get<EntityAndField>(&selectAll2.internalVariant()) == nullptr );
  const auto selectAllField2 = boost::get<SelectAllField>(selectAll2.internalVariant());
  QCOMPARE(selectAllField2.entityAliasOrName(), QString("Person"));

  EntityAndField id("id");
  QueryFieldVariant idFieldVariant(id);
  QVERIFY( !idFieldVariant.isNull() );
  QVERIFY( boost::get<SelectAllField>(&idFieldVariant.internalVariant()) == nullptr );
  QVERIFY( boost::get<EntityAndField>(&idFieldVariant.internalVariant()) != nullptr );
  const auto idField = boost::get<EntityAndField>(idFieldVariant.internalVariant());
  QCOMPARE(idField.fieldAliasOrName(), QString("id"));
}

void QueryFieldTest::constructTest()
{
  QueryEntity person("Person");
  QueryEntity address("Address", EntityAlias("ADR"));

  QueryField nullField;
  QVERIFY( nullField.isNull() );
  QVERIFY( boost::get<SelectAllField>(&nullField.internalVariant().internalVariant()) == nullptr );
  QVERIFY( boost::get<EntityAndField>(&nullField.internalVariant().internalVariant()) == nullptr );

  QueryField selectAll1(SelectAllField{});
  QVERIFY( !selectAll1.isNull() );
  QVERIFY( boost::get<SelectAllField>(&selectAll1.internalVariant().internalVariant()) != nullptr );
  QVERIFY( boost::get<EntityAndField>(&selectAll1.internalVariant().internalVariant()) == nullptr );
  const auto selectAllField1 = boost::get<SelectAllField>(selectAll1.internalVariant().internalVariant());
  QVERIFY(selectAllField1.entityAliasOrName().isEmpty());

  QueryField selectAll2(SelectAllField{person});
  QVERIFY( !selectAll2.isNull() );
  QVERIFY( boost::get<SelectAllField>(&selectAll2.internalVariant().internalVariant()) != nullptr );
  QVERIFY( boost::get<EntityAndField>(&selectAll2.internalVariant().internalVariant()) == nullptr );
  const auto selectAllField2 = boost::get<SelectAllField>(selectAll2.internalVariant().internalVariant());
  QCOMPARE(selectAllField2.entityAliasOrName(), QString("Person"));

  QueryField name("name");
  QVERIFY( !name.isNull() );
  QVERIFY( boost::get<SelectAllField>(&name.internalVariant().internalVariant()) == nullptr );
  QVERIFY( boost::get<EntityAndField>(&name.internalVariant().internalVariant()) != nullptr );
  const auto nameEaF = boost::get<EntityAndField>(name.internalVariant().internalVariant());
  QVERIFY(!nameEaF.hasEntity());
  QCOMPARE(nameEaF.fieldName(), QString("name"));
  QCOMPARE(nameEaF.fieldAliasOrName(), QString("name"));

  QueryField age("age", FieldAlias("A"));
  QVERIFY( !age.isNull() );
  QVERIFY( boost::get<SelectAllField>(&age.internalVariant().internalVariant()) == nullptr );
  QVERIFY( boost::get<EntityAndField>(&age.internalVariant().internalVariant()) != nullptr );
  const auto ageEaF = boost::get<EntityAndField>(age.internalVariant().internalVariant());
  QVERIFY(!ageEaF.hasEntity());
  QCOMPARE(ageEaF.fieldName(), QString("age"));
  QCOMPARE(ageEaF.fieldAliasOrName(), QString("A"));

  QueryField personName(person, "name");
  QVERIFY( !personName.isNull() );
  QVERIFY( boost::get<SelectAllField>(&personName.internalVariant().internalVariant()) == nullptr );
  QVERIFY( boost::get<EntityAndField>(&personName.internalVariant().internalVariant()) != nullptr );
  const auto personNameEaF = boost::get<EntityAndField>(personName.internalVariant().internalVariant());
  QVERIFY(personNameEaF.hasEntity());
  QCOMPARE(personNameEaF.entityName(), QString("Person"));
  QCOMPARE(personNameEaF.entityAliasOrName(), QString("Person"));
  QCOMPARE(personNameEaF.fieldName(), QString("name"));
  QCOMPARE(personNameEaF.fieldAliasOrName(), QString("name"));

  QueryField addressStreet(address, "street", FieldAlias("AddressStreet"));
  QVERIFY( !addressStreet.isNull() );
  QVERIFY( boost::get<SelectAllField>(&addressStreet.internalVariant().internalVariant()) == nullptr );
  QVERIFY( boost::get<EntityAndField>(&addressStreet.internalVariant().internalVariant()) != nullptr );
  const auto addressStreetEaF = boost::get<EntityAndField>(addressStreet.internalVariant().internalVariant());
  QVERIFY(addressStreetEaF.hasEntity());
  QCOMPARE(addressStreetEaF.entityName(), QString("Address"));
  QCOMPARE(addressStreetEaF.entityAliasOrName(), QString("ADR"));
  QCOMPARE(addressStreetEaF.fieldName(), QString("street"));
  QCOMPARE(addressStreetEaF.fieldAliasOrName(), QString("AddressStreet"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  QueryFieldTest test;

  return QTest::qExec(&test, argc, argv);
}
