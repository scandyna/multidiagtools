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
#include "SelectStatementTest.h"
#include "Mdt/QueryExpression/ReflectionSelectStatement.h"
#include "Mdt/QueryExpression/Debug/ExpressionTreeToString.h"
#include "Mdt/Reflection/ReflectStruct.h"
#include "Mdt/Reflection/PrimaryKey.h"
#include "Mdt/Reflection/Relation.h"
#include <QLatin1String>
#include <boost/variant.hpp>

using namespace Mdt::QueryExpression;
using namespace Mdt::QueryExpression::Debug;

/*
 * Entities
 */

struct TeamDataStruct
{
  qlonglong id = 0;
  QString name;
};

MDT_REFLECT_STRUCT(
  (TeamDataStruct),
  Team,
  (id),
  (name)
)

struct PersonDataStruct
{
  qlonglong id = 0;
  qlonglong teamId = 0;
  QString name;
  double age = 0.0;
  QString remarks;
};

MDT_REFLECT_STRUCT(
  (PersonDataStruct),
  Person,
  (id),
  (teamId),
  (name),
  (age),
  (remarks)
)

struct AddressDataStruct
{
  qlonglong id = 0;
  qlonglong personId = 0;
  QString street;
  QString remarks;
};

MDT_REFLECT_STRUCT(
  (AddressDataStruct),
  Address,
  (id),
  (personId),
  (street),
  (remarks)
)

using TeamPrimaryKey = Mdt::Reflection::PrimaryKey<TeamDef::id>;
using PersonPrimaryKey = Mdt::Reflection::PrimaryKey<PersonDef::id>;
using AddressPrimaryKey = Mdt::Reflection::PrimaryKey<AddressDef::id>;

/*
 * Helper to get entity alias or name
 */

struct GetEntityAliasOrNameVisitor : public boost::static_visitor<>
{
  void operator()(const NullSelectField &)
  {
  }

  void operator()(const SelectAllField & field)
  {
    aliasOrName = field.entityAliasOrName();
  }

  void operator()(const EntityAndField & field)
  {
    aliasOrName = field.entityAliasOrName();
  }

  QString aliasOrName;
};

QString getEntityAliasOrName(const SelectField & field)
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
  void operator()(const NullSelectField & )
  {
  }

  void operator()(const SelectAllField &)
  {
  }

  void operator()(const EntityAndField & field)
  {
    aliasOrName = field.fieldAliasOrName();
  }

  QString aliasOrName;
};

QString getFieldAliasOrName(const SelectField & field)
{
  GetFieldAliasOrNameVisitor visitor;
  boost::apply_visitor(visitor, field.internalVariant().internalVariant());
  return visitor.aliasOrName;
}

/*
 * Helper to get a join constraint expression as string
 */

QString expressionToString(const Mdt::QueryExpression::JoinConstraintExpression & expr)
{
  Q_ASSERT(!expr.isNull());

  return expressionTreeToInfixString(expr.internalTree());
}

/*
 * Tests
 */

void SelectStatementTest::apiExamplesTest()
{
  using Like = LikeExpression;

  SelectFieldList fieldList;

  ReflectionSelectStatement<PersonDef> stm1;
  stm1.selectAllFields();
  QCOMPARE(stm1.entity().aliasOrName(), QLatin1String("Person"));
  fieldList = stm1.fieldList();
  QCOMPARE(fieldList.fieldCount(), 1);
  QVERIFY(getEntityAliasOrName(fieldList.at(0)).isEmpty());
  QVERIFY(getFieldAliasOrName(fieldList.at(0)).isEmpty());

  ReflectionSelectStatement<PersonDef> stm2( EntityAlias(QLatin1String("P")) );
  QCOMPARE(stm2.entity().name(), QLatin1String("Person"));
  QCOMPARE(stm2.entity().aliasOrName(), QLatin1String("P"));

  ReflectionSelectStatement<PersonDef> stm3;
  stm3.addField<PersonDef::name>();
  stm3.addField<PersonDef::age>( FieldAlias(QLatin1String("A")) );
  QCOMPARE(stm3.entity().aliasOrName(), QLatin1String("Person"));
  fieldList = stm3.fieldList();
  QCOMPARE(fieldList.fieldCount(), 2);
  QCOMPARE(getEntityAliasOrName(fieldList.at(0)), QLatin1String("Person"));
  QCOMPARE(getFieldAliasOrName(fieldList.at(0)), QLatin1String("name"));
  QCOMPARE(getEntityAliasOrName(fieldList.at(1)), QLatin1String("Person"));
  QCOMPARE(getFieldAliasOrName(fieldList.at(1)), QLatin1String("A"));

  ReflectionQueryField<PersonDef::name> name;
  ReflectionQueryField<PersonDef::age> age( FieldAlias(QLatin1String("A")) );

  ReflectionSelectStatement<PersonDef> stm4;
  stm4.addField(name);
  stm4.addField(age);
  stm4.addField<PersonDef::remarks>();
  QVERIFY(!stm4.hasFilter());
  stm4.setFilter( (name == Like(QLatin1String("A*"))) && (age > 29) );
  QVERIFY(stm4.hasFilter());
  QCOMPARE(stm4.entity().aliasOrName(), QLatin1String("Person"));
  fieldList = stm4.fieldList();
  QCOMPARE(fieldList.fieldCount(), 3);
  QCOMPARE(getEntityAliasOrName(fieldList.at(0)), QLatin1String("Person"));
  QCOMPARE(getFieldAliasOrName(fieldList.at(0)), QLatin1String("name"));
  QCOMPARE(getEntityAliasOrName(fieldList.at(1)), QLatin1String("Person"));
  QCOMPARE(getFieldAliasOrName(fieldList.at(1)), QLatin1String("A"));
  QCOMPARE(getEntityAliasOrName(fieldList.at(2)), QLatin1String("Person"));
  QCOMPARE(getFieldAliasOrName(fieldList.at(2)), QLatin1String("remarks"));

  stm4.clear();
  QCOMPARE(stm4.entity().aliasOrName(), QLatin1String("Person"));
  QCOMPARE(stm4.fieldCount(), 0);
  QVERIFY(!stm4.hasFilter());
}

void SelectStatementTest::addAllFieldsTest()
{
  SelectFieldList fieldList;

  ReflectionSelectStatement<PersonDef> stm;
  stm.addAllFields();
  fieldList = stm.fieldList();
  QCOMPARE(fieldList.fieldCount(), 5);
  QCOMPARE(getEntityAliasOrName(fieldList.at(0)), QLatin1String("Person"));
  QCOMPARE(getFieldAliasOrName(fieldList.at(0)), QLatin1String("id"));
  QCOMPARE(getEntityAliasOrName(fieldList.at(1)), QLatin1String("Person"));
  QCOMPARE(getFieldAliasOrName(fieldList.at(1)), QLatin1String("teamId"));
  QCOMPARE(getEntityAliasOrName(fieldList.at(2)), QLatin1String("Person"));
  QCOMPARE(getFieldAliasOrName(fieldList.at(2)), QLatin1String("name"));
  QCOMPARE(getEntityAliasOrName(fieldList.at(3)), QLatin1String("Person"));
  QCOMPARE(getFieldAliasOrName(fieldList.at(3)), QLatin1String("age"));
  QCOMPARE(getEntityAliasOrName(fieldList.at(4)), QLatin1String("Person"));
  QCOMPARE(getFieldAliasOrName(fieldList.at(4)), QLatin1String("remarks"));
}

void SelectStatementTest::joinTest()
{
  JoinClauseList joinList;
  QString expectedExpression;

  ReflectionQueryField<PersonDef::id> personId;
  ReflectionQueryField<PersonDef::name> personName;
  ReflectionQueryField<AddressDef::personId> addressPersonId;
  ReflectionQueryField<AddressDef::street> addressStreet( FieldAlias(QLatin1String("AddressStreet")) );

  ReflectionSelectStatement<PersonDef> stm1;
  stm1.addField(personName);
  stm1.addField<PersonDef::remarks>( FieldAlias(QLatin1String("PersonRemarks")) );
  stm1.addField(addressStreet);
  stm1.addField<AddressDef::remarks>( FieldAlias(QLatin1String("AddressRemarks")) );
  stm1.joinEntity<AddressDef>( addressPersonId == personId );
  stm1.setFilter( (personName == QString::fromLatin1("A")) || (addressStreet == QString::fromLatin1("Street name B")) );
  joinList = stm1.joinClauseList();
  QCOMPARE(joinList.clauseCount(), 1);
  QCOMPARE(joinList.clauseAt(0).entityAliasOrName(), QLatin1String("Address"));
  expectedExpression = QLatin1String("Address.personId==Person.id");
  QCOMPARE(expressionToString(joinList.clauseAt(0).joinConstraintExpression()), expectedExpression);
}

void SelectStatementTest::joinByRelationTest()
{
  using TeamPersonRelation = Mdt::Reflection::Relation<TeamPrimaryKey, PersonDef::teamId>;
  using PersonAddressRelation = Mdt::Reflection::Relation<PersonPrimaryKey, AddressDef::personId>;

  JoinClauseList joinList;
  QString expectedExpression;

  ReflectionSelectStatement<PersonDef> personStm;
  personStm.joinEntity<PersonAddressRelation>();
  joinList = personStm.joinClauseList();
  QCOMPARE(joinList.clauseCount(), 1);
  QCOMPARE(joinList.clauseAt(0).entityAliasOrName(), QLatin1String("Address"));
  expectedExpression = QLatin1String("Address.personId==Person.id");
  QCOMPARE(expressionToString(joinList.clauseAt(0).joinConstraintExpression()), expectedExpression);

  ReflectionSelectStatement<AddressDef> addressStm;
  addressStm.joinEntity<PersonAddressRelation>();
  joinList = addressStm.joinClauseList();
  QCOMPARE(joinList.clauseCount(), 1);
  QCOMPARE(joinList.clauseAt(0).entityAliasOrName(), QLatin1String("Person"));
  expectedExpression = QLatin1String("Address.personId==Person.id");
  QCOMPARE(expressionToString(joinList.clauseAt(0).joinConstraintExpression()), expectedExpression);

  ReflectionSelectStatement<TeamDef> teamStm;
  teamStm.joinEntity<TeamPersonRelation>();
  teamStm.joinEntity<PersonAddressRelation>();
  joinList = teamStm.joinClauseList();
  QCOMPARE(joinList.clauseCount(), 2);
  QCOMPARE(joinList.clauseAt(0).entityAliasOrName(), QLatin1String("Person"));
  expectedExpression = QLatin1String("Person.teamId==Team.id");
  QCOMPARE(expressionToString(joinList.clauseAt(0).joinConstraintExpression()), expectedExpression);
  QCOMPARE(joinList.clauseAt(1).entityAliasOrName(), QLatin1String("Address"));
  expectedExpression = QLatin1String("Address.personId==Person.id");
  QCOMPARE(expressionToString(joinList.clauseAt(1).joinConstraintExpression()), expectedExpression);

  personStm.clear();
  personStm.joinEntity<TeamPersonRelation>();
  personStm.joinEntity<PersonAddressRelation>();
  joinList = personStm.joinClauseList();
  QCOMPARE(joinList.clauseCount(), 2);
  QCOMPARE(joinList.clauseAt(0).entityAliasOrName(), QLatin1String("Team"));
  expectedExpression = QLatin1String("Person.teamId==Team.id");
  QCOMPARE(expressionToString(joinList.clauseAt(0).joinConstraintExpression()), expectedExpression);
  QCOMPARE(joinList.clauseAt(1).entityAliasOrName(), QLatin1String("Address"));
  expectedExpression = QLatin1String("Address.personId==Person.id");
  QCOMPARE(expressionToString(joinList.clauseAt(1).joinConstraintExpression()), expectedExpression);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SelectStatementTest test;

  return QTest::qExec(&test, argc, argv);
}
