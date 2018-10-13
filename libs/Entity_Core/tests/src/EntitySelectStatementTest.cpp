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
#include "Mdt/Entity/Relation.h"
#include "Mdt/QueryExpression/SelectField.h"
#include "Mdt/QueryExpression/AbstractExpressionTreeVisitor.h"
#include "Mdt/QueryExpression/ExpressionTree.h"
#include "Mdt/QueryExpression/LogicalOperator.h"
#include "Mdt/QueryExpression/TravserseTreeGraph.h"
#include "Mdt/QueryExpression/EntityAndField.h"
#include <boost/variant.hpp>

using namespace Mdt::Entity;
using Mdt::QueryExpression::JoinOperator;
using Mdt::QueryExpression::JoinConstraintExpression;
using Mdt::QueryExpression::ExpressionTree;
using Mdt::QueryExpression::EntityAndField;

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

struct TwoFieldPkDataStruct
{
  int pk1;
  QString pk2;
};

MDT_ENTITY_DEF(
  (TwoFieldPkDataStruct),
  TwoFieldPk,
  (pk1, FieldFlag::IsPrimaryKey),
  (pk2, FieldFlag::IsPrimaryKey)
)

struct TwoFieldFkDataStruct
{
  int fk1;
  QString fk2;
};

MDT_ENTITY_DEF(
  (TwoFieldFkDataStruct),
  TwoFieldFk,
  (fk1),
  (fk2)
)

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
 * Helpers to transform a expression tree to a string
 */

class ExpressionTreeToStringVisitor : public Mdt::QueryExpression::AbstractExpressionTreeVisitor
{
 public:

  void processPreorder(Mdt::QueryExpression::LogicalOperator) override
  {
    mExpressionString += "(";
  }

  void processPostorder(Mdt::QueryExpression::LogicalOperator) override
  {
    mExpressionString += ")";
  }

  void processInorder(Mdt::QueryExpression::ComparisonOperator op) override
  {
    mExpressionString += comparisonOperatorToString(op);
  }

  void processInorder(Mdt::QueryExpression::LogicalOperator op) override
  {
    mExpressionString += ")" + logicalOperatorToString(op) + "(";
  }

  void processInorder(const Mdt::QueryExpression::EntityAndField & field) override
  {
    mExpressionString += field.entityAliasOrName() + "." + field.fieldAliasOrName();
  }

  void processInorder(const QVariant & value) override
  {
    mExpressionString += value.toString();
  }

  void clear()
  {
    mExpressionString.clear();
  }

  QString toString() const
  {
    return mExpressionString;
  }

 private:

  static QString comparisonOperatorToString(Mdt::QueryExpression::ComparisonOperator op)
  {
    using Mdt::QueryExpression::ComparisonOperator;
    switch(op){
      case ComparisonOperator::Equal:
        return "==";
      case ComparisonOperator::Like:
        return " Like ";
      case ComparisonOperator::NotEqual:
        return "!=";
      case ComparisonOperator::Less:
        return "<";
      case ComparisonOperator::LessEqual:
        return "<=";
      case ComparisonOperator::Greater:
        return ">";
      case ComparisonOperator::GreaterEqual:
        return ">=";
    }
  }

  static QString logicalOperatorToString(Mdt::QueryExpression::LogicalOperator op)
  {
    using Mdt::QueryExpression::LogicalOperator;

    switch(op){
      case LogicalOperator::And:
        return "&&";
      case LogicalOperator::Or:
        return "||";
    }
    return QString();
  }

  QString mExpressionString;
};

QString expressionTreeToString(const Mdt::QueryExpression::ExpressionTree & expresionTree)
{
  Q_ASSERT(!expresionTree.isNull());

  ExpressionTreeToStringVisitor visitor;
  Mdt::QueryExpression::traverseExpressionTree(expresionTree, visitor);

  return visitor.toString();
}

/*
 * Compile time tests
 */

void EntitySelectStatementTest::selectOppositeTypeTest()
{
  using Address = Impl::SelectOppositeType<PersonEntity, PersonEntity, AddressEntity>::type;
  static_assert( std::is_same<Address, AddressEntity>::value, "" );
  static_assert(!std::is_same<Address, PersonEntity>::value, "" );

  using Person = Impl::SelectOppositeType<AddressEntity, PersonEntity, AddressEntity>::type;
  static_assert( std::is_same<Person, PersonEntity>::value, "" );
  static_assert(!std::is_same<Person, AddressEntity>::value, "" );
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

void EntitySelectStatementTest::fieldIndexTest()
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
  QCOMPARE(stm.fieldIndex( stm.def().firstName() ), 0);
  QCOMPARE(stm.fieldIndex( stm.def().remarks() ), 1);
  QCOMPARE(stm.fieldIndex( address, address.def().street() ), 2);
  QCOMPARE(stm.fieldIndex( addressStreet ), 2);
}

void EntitySelectStatementTest::buildJoinConstraintExpressionTreeTest()
{
  ExpressionTree tree;

  /*
   *                  (==)
   *                 /    \
   * (Address.personId)  (Person.id)
   */
  tree = Impl::buildJoinConstraintExpressionTree("Person", {"id"}, "Address", {"personId"});
  QCOMPARE(expressionTreeToString(tree), QString("Address.personId==Person.id"));

  /*
   *       (==)
   *      /    \
   * (F.fk1)  (P.pk1)
   */
  tree = Impl::buildJoinConstraintExpressionTree("P", {"pk1"}, "F", {"fk1"});
  QCOMPARE(expressionTreeToString(tree), QString("F.fk1==P.pk1"));

  /*
   *            ____(&&)____
   *           /            \
   *       (==)              (==)
   *      /    \            /    \
   * (F.fk1)  (P.pk1)  (F.fk2)  (P.pk2)
   */
  tree = Impl::buildJoinConstraintExpressionTree("P", {"pk1","pk2"}, "F", {"fk1","fk2"});
  QCOMPARE(expressionTreeToString(tree), QString("(F.fk1==P.pk1)&&(F.fk2==P.pk2)"));

  /*
   *                     _______(&&)_______
   *                    /                  \
   *            ____(&&)____               (==)
   *           /            \             /    \
   *       (==)              (==)    (F.fk3)  (P.pk3)
   *      /    \            /    \
   * (F.fk1)  (P.pk1)  (F.fk2)  (P.pk2)
   */
  tree = Impl::buildJoinConstraintExpressionTree("P", {"pk1","pk2","pk3"}, "F", {"fk1","fk2","fk3"});
  QCOMPARE(expressionTreeToString(tree), QString("((F.fk1==P.pk1)&&(F.fk2==P.pk2))&&(F.fk3==P.pk3)"));

  /*
   *                                __________(&&)________
   *                               /                      \
   *                     _______(&&)_______               (==)
   *                    /                  \             /    \
   *            ____(&&)____               (==)     (F.fk4)  (P.pk4)
   *           /            \             /    \
   *       (==)              (==)    (F.fk3)  (P.pk3)
   *      /    \            /    \
   * (F.fk1)  (P.pk1)  (F.fk2)  (P.pk2)
   */
  tree = Impl::buildJoinConstraintExpressionTree("P", {"pk1","pk2","pk3","pk4"}, "F", {"fk1","fk2","fk3","fk4"});
  QCOMPARE(expressionTreeToString(tree), QString("(((F.fk1==P.pk1)&&(F.fk2==P.pk2))&&(F.fk3==P.pk3))&&(F.fk4==P.pk4)"));
}

void EntitySelectStatementTest::buildJoinConstraintExpressionTest()
{
  using PersonAddressRelation = Relation<PersonEntity, AddressEntity, AddressDef::personIdField>;
  using TwoFieldRelation = Relation<TwoFieldPkEntity, TwoFieldFkEntity, TwoFieldFkDef::fk1Field, TwoFieldFkDef::fk2Field>;

  JoinConstraintExpression join;

  join = Impl::buildJoinConstraintExpression<PersonAddressRelation>();
  QCOMPARE(expressionTreeToString(join.internalTree()), QString("Address.personId==Person.id"));

  join = Impl::buildJoinConstraintExpression<TwoFieldRelation>();
  QCOMPARE(expressionTreeToString(join.internalTree()), QString("(TwoFieldFk.fk1==TwoFieldPk.pk1)&&(TwoFieldFk.fk2==TwoFieldPk.pk2)"));
}

void EntitySelectStatementTest::joinEntityByRelationTest()
{
  using PersonAddressRelation = Mdt::Entity::Relation<PersonEntity, AddressEntity, AddressDef::personIdField>;

  QString expectedJoinConstraint;

  EntitySelectStatement<PersonEntity> stm;
  stm.joinEntity<PersonAddressRelation>();
  QCOMPARE(stm.joinClauseList().clauseCount(), 1);
  QCOMPARE(stm.joinClauseList().clauseAt(0).entity().name(), QString("Address"));
  QCOMPARE(stm.joinClauseList().clauseAt(0).joinOperator(), JoinOperator::Join);
  expectedJoinConstraint = "Address.personId==Person.id";
  QCOMPARE(expressionTreeToString(stm.joinClauseList().clauseAt(0).joinConstraintExpression().internalTree()), expectedJoinConstraint);

  EntitySelectStatement<AddressEntity> rstm;
  rstm.joinEntity<PersonAddressRelation>();
  QCOMPARE(rstm.joinClauseList().clauseCount(), 1);
  QCOMPARE(rstm.joinClauseList().clauseAt(0).entity().name(), QString("Person"));
  expectedJoinConstraint = "Address.personId==Person.id";
  QCOMPARE(expressionTreeToString(rstm.joinClauseList().clauseAt(0).joinConstraintExpression().internalTree()), expectedJoinConstraint);
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
