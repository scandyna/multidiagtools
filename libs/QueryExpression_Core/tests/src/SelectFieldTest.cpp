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
#include "SelectFieldTest.h"
#include "Mdt/QueryExpression/SelectField.h"
#include <boost/variant/get.hpp>
#include <boost/proto/matches.hpp>
// #include <boost/proto/literal.hpp>
// #include <boost/proto/transform/arg.hpp>

using namespace Mdt::QueryExpression;

/*
 * Compile time tests
 */

template<typename Expr, typename Grammar>
constexpr bool expressionMatchesGrammar()
{
  return boost::proto::matches< Expr, Grammar >::value;
}

void SelectFieldTest::terminalGrammarTest()
{

}

/*
 * Runtime tests
 */

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
}

void SelectFieldTest::entityAndFieldTest()
{
  EntityAndField ef1( FieldName("fn1") );
  QVERIFY(ef1.entityName().isEmpty());
  QVERIFY(ef1.entityAlias().isEmpty());
  QVERIFY(ef1.entityAliasOrName().isEmpty());
  QCOMPARE(ef1.fieldName(), QString("fn1"));
  QVERIFY(ef1.fieldAlias().isEmpty());
  QCOMPARE(ef1.fieldAliasOrName(), QString("fn1"));

  EntityAndField ef2( FieldName("fn2"), "F2" );
  QVERIFY(ef2.entityName().isEmpty());
  QVERIFY(ef2.entityAlias().isEmpty());
  QVERIFY(ef2.entityAliasOrName().isEmpty());
  QCOMPARE(ef2.fieldName(), QString("fn2"));
  QCOMPARE(ef2.fieldAlias(), QString("F2"));
  QCOMPARE(ef2.fieldAliasOrName(), QString("F2"));

  SelectEntity e3( EntityName("en3"), "E3" );
  EntityAndField ef3( e3, FieldName("fn3"), "F3" );
  QCOMPARE(ef3.entityName(), QString("en3"));
  QCOMPARE(ef3.entityAlias(), QString("E3"));
  QCOMPARE(ef3.entityAliasOrName(), QString("E3"));
  QCOMPARE(ef3.fieldName(), QString("fn3"));
  QCOMPARE(ef3.fieldAlias(), QString("F3"));
  QCOMPARE(ef3.fieldAliasOrName(), QString("F3"));
}

void SelectFieldTest::constructGetTest()
{
  SelectField selectAll(SelectAllField{});
  QVERIFY( boost::get<SelectAllField>(&selectAll.internalVariant()) != nullptr );
  QVERIFY( boost::get<EntityAndField>(&selectAll.internalVariant()) == nullptr );

  SelectField name( FieldName("name") );
  QVERIFY( boost::get<SelectAllField>(&name.internalVariant()) == nullptr );
  QVERIFY( boost::get<EntityAndField>(&name.internalVariant()) != nullptr );
  const auto nameEaF = boost::get<EntityAndField>(name.internalVariant());
  QCOMPARE(nameEaF.fieldName(), QString("name"));
  QCOMPARE(nameEaF.fieldAliasOrName(), QString("name"));

  SelectField age( FieldName("age"), "A" );
  QVERIFY( boost::get<SelectAllField>(&age.internalVariant()) == nullptr );
  QVERIFY( boost::get<EntityAndField>(&age.internalVariant()) != nullptr );
  const auto ageEaF = boost::get<EntityAndField>(age.internalVariant());
  QCOMPARE(ageEaF.fieldName(), QString("age"));
  QCOMPARE(ageEaF.fieldAlias(), QString("A"));
  QCOMPARE(ageEaF.fieldAliasOrName(), QString("A"));

  SelectEntity person( EntityName("Person") );
  SelectField personName( person, FieldName("name") );
  QVERIFY( boost::get<SelectAllField>(&personName.internalVariant()) == nullptr );
  QVERIFY( boost::get<EntityAndField>(&personName.internalVariant()) != nullptr );
  const auto personNameEaF = boost::get<EntityAndField>(personName.internalVariant());
  QCOMPARE(personNameEaF.entityName(), QString("Person"));
  QCOMPARE(personNameEaF.entityAliasOrName(), QString("Person"));
  QCOMPARE(personNameEaF.fieldName(), QString("name"));
  QCOMPARE(personNameEaF.fieldAliasOrName(), QString("name"));

  SelectEntity address( EntityName("Address"), "ADR");
  SelectField addressStreet( address, FieldName("street"), "AddressStreet" );
  QVERIFY( boost::get<SelectAllField>(&addressStreet.internalVariant()) == nullptr );
  QVERIFY( boost::get<EntityAndField>(&addressStreet.internalVariant()) != nullptr );
  const auto addressStreetEaF = boost::get<EntityAndField>(addressStreet.internalVariant());
  QCOMPARE(addressStreetEaF.entityName(), QString("Address"));
  QCOMPARE(addressStreetEaF.entityAlias(), QString("ADR"));
  QCOMPARE(addressStreetEaF.entityAliasOrName(), QString("ADR"));
  QCOMPARE(addressStreetEaF.fieldName(), QString("street"));
  QCOMPARE(addressStreetEaF.fieldAlias(), QString("AddressStreet"));
  QCOMPARE(addressStreetEaF.fieldAliasOrName(), QString("AddressStreet"));
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
