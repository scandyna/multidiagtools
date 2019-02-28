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
#include "ExpressionTreeTest.h"
#include "Mdt/QueryExpression/SelectField.h"
#include "Mdt/QueryExpression/ExpressionTree.h"
#include "Mdt/QueryExpression/AbstractExpressionTreeVisitor.h"
#include "Mdt/QueryExpression/TravserseTreeGraph.h"
#include "Mdt/QueryExpression/FilterExpression.h"
#include "Mdt/QueryExpression/JoinConstraintExpression.h"
#include <QMetaType>

using namespace Mdt::QueryExpression;

/*
 * Expression tree visitors for tests
 */

class AbstractExpressionToStringVisitor : public Mdt::QueryExpression::AbstractExpressionTreeVisitor
{
 protected:

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

  static QString logicalOperatorToString(LogicalOperator op)
  {
    switch(op){
      case LogicalOperator::And:
        return "&&";
      case LogicalOperator::Or:
        return "||";
    }
    return QString();
  }

  static bool isTypeString(const QVariant & v)
  {
    const auto type = static_cast<QMetaType::Type>(v.type());
    return type == QMetaType::QString;
  }

};

class ExpressionToPrefixStringVisitor : public AbstractExpressionToStringVisitor
{
 public:

  void processPreorder(Mdt::QueryExpression::ComparisonOperator op) override
  {
    mExpressionString += comparisonOperatorToString(op) + " ";
  }

  void processPreorder(Mdt::QueryExpression::LogicalOperator op) override
  {
    mExpressionString += logicalOperatorToString(op) + " ";
  }

  void processPreorder(const Mdt::QueryExpression::EntityAndField & field) override
  {
    mExpressionString += field.fieldAliasOrName() + " ";
  }

  void processPreorder(const QVariant& value) override
  {
    mExpressionString += value.toString() + " ";
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

  QString mExpressionString;
};

class ExpressionToInfixStringVisitor : public AbstractExpressionToStringVisitor
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
    mExpressionString += field.fieldAliasOrName();
  }

  void processInorder(const QVariant & value) override
  {
    if(isTypeString(value)){
      mExpressionString += "\"" + value.toString() + "\"";
    }else{
      mExpressionString += value.toString();
    }
  }

  void processInorder(const LikeExpressionData& data) override
  {
    mExpressionString += "'" + data.toString() + "'";
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

  QString mExpressionString;
};

class ExpressionToPostfixStringVisitor : public AbstractExpressionToStringVisitor
{
 public:

  void processPostorder(Mdt::QueryExpression::ComparisonOperator op) override
  {
    mExpressionString += comparisonOperatorToString(op) + " ";
  }

  void processPostorder(Mdt::QueryExpression::LogicalOperator op) override
  {
    mExpressionString += logicalOperatorToString(op) + " ";
  }

  void processPostorder(const Mdt::QueryExpression::EntityAndField & field) override
  {
    mExpressionString += field.fieldAliasOrName() + " ";
  }

  void processPostorder(const QVariant& value) override
  {
    mExpressionString += value.toString() + " ";
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

  QString mExpressionString;
};

/*
 * Tests
 */

void ExpressionTreeTest::simpleBuildTreeTest()
{
  SelectEntity person(EntityName("Person"), "P");
  SelectField clientId(person, FieldName("id"), "ID");
  ExpressionTreeVertex leftVertex, rightVertex, rootVertex;

  ExpressionTree tree;
  QVERIFY(tree.isNull());

  leftVertex = tree.addNode(clientId, ComparisonOperator::Equal, 44);
  QVERIFY(!tree.isNull());
  QCOMPARE(tree.internalRootVertex(), leftVertex);
  rightVertex = tree.addNode(clientId, ComparisonOperator::Less, 25); // Root vertex is ambigous for now
  rootVertex = tree.addNode(leftVertex, LogicalOperator::Or, rightVertex);
  QCOMPARE(tree.internalRootVertex(), rootVertex);

  tree.clear();
  QVERIFY(tree.isNull());
  QCOMPARE((int)tree.internalRootVertex(), 0);
  leftVertex = tree.addNode(clientId, ComparisonOperator::Equal, 55);
  QCOMPARE(tree.internalRootVertex(), leftVertex);
  rightVertex = tree.addNode(clientId, ComparisonOperator::Less, 46); // Root vertex is ambigous for now
  rootVertex = tree.addNode(leftVertex, LogicalOperator::Or, rightVertex);
  QCOMPARE(tree.internalRootVertex(), rootVertex);
}

void ExpressionTreeTest::buildAndVisitTreeTest()
{
  SelectEntity person(EntityName("Person"), "P");
  SelectField clientId(person, FieldName("id"));
  ExpressionTree tree;
  ExpressionTreeVertex leftVertex, rightVertex;
  ExpressionToPrefixStringVisitor prefixVisitor;
  ExpressionToInfixStringVisitor infixVisitor;
  ExpressionToPostfixStringVisitor postFixVisitor;
  QString expectedString;

  /*
   *    (==)
   *    /  \
   *  (id) (25)
   */
  tree.addNode(clientId, ComparisonOperator::Equal, 25);
  // Prefix expression
  expectedString = "== id 25 ";
  traverseExpressionTree(tree, prefixVisitor);
  QCOMPARE(prefixVisitor.toString(), expectedString);
  // Infix expression
  expectedString = "id==25";
  traverseExpressionTree(tree, infixVisitor);
  QCOMPARE(infixVisitor.toString(), expectedString);
  // Postfix expression
  expectedString = "id 25 == ";
  traverseExpressionTree(tree, postFixVisitor);
  QCOMPARE(postFixVisitor.toString(), expectedString);

  /*
   *         (OR)
   *        /    \
   *     (==)     (<)
   *    /   \     /  \
   *  (id) (25) (id) (150)
   */
  tree.clear();
  leftVertex = tree.addNode(clientId, ComparisonOperator::Equal, 25);
  rightVertex = tree.addNode(clientId, ComparisonOperator::Less, 150);
  tree.addNode(leftVertex, LogicalOperator::Or, rightVertex);
  // Prefix expression
  prefixVisitor.clear();
  expectedString = "|| == id 25 < id 150 ";
  traverseExpressionTree(tree, prefixVisitor);
  QCOMPARE(prefixVisitor.toString(), expectedString);
  // Infix expression
  infixVisitor.clear();
  expectedString = "(id==25)||(id<150)";
  traverseExpressionTree(tree, infixVisitor);
  QCOMPARE(infixVisitor.toString(), expectedString);
  // Postfix expression
  postFixVisitor.clear();
  expectedString = "id 25 == id 150 < || ";
  traverseExpressionTree(tree, postFixVisitor);
  QCOMPARE(postFixVisitor.toString(), expectedString);

  /*
   *              (AND)
   *             /     \
   *         (OR)       (>=)
   *        /    \      /  \
   *     (==)     (<) (id) (200)
   *    /   \     /  \
   *  (id) (25) (id) (150)
   */
  tree.clear();
  leftVertex = tree.addNode(clientId, ComparisonOperator::Equal, 25);
  rightVertex = tree.addNode(clientId, ComparisonOperator::Less, 150);
  leftVertex = tree.addNode(leftVertex, LogicalOperator::Or, rightVertex);
  rightVertex = tree.addNode(clientId, ComparisonOperator::GreaterEqual, 200);
  tree.addNode(leftVertex, LogicalOperator::And, rightVertex);
  // Prefix expression
  prefixVisitor.clear();
  expectedString = "&& || == id 25 < id 150 >= id 200 ";
  traverseExpressionTree(tree, prefixVisitor);
  QCOMPARE(prefixVisitor.toString(), expectedString);
  // Infix expression
  infixVisitor.clear();
  expectedString = "((id==25)||(id<150))&&(id>=200)";
  traverseExpressionTree(tree, infixVisitor);
  QCOMPARE(infixVisitor.toString(), expectedString);
  // Postfix expression
  postFixVisitor.clear();
  expectedString = "id 25 == id 150 < || id 200 >= && ";
  traverseExpressionTree(tree, postFixVisitor);
  QCOMPARE(postFixVisitor.toString(), expectedString);
}

void ExpressionTreeTest::filterExpressionTest()
{
  using Like = LikeExpression;

//   SelectEntity person(EntityName("Person"), "P");
//   SelectField clientId(person, FieldName("id"));
  
  QueryEntity person("Person", EntityAlias("P"));
  QueryField clientId(person, "id");
  ExpressionToInfixStringVisitor infixVisitor;
  QString expectedString;
  FilterExpression filter;

  filter.setFilter(clientId == 15);
  expectedString = "id==15";
  traverseExpressionTree(filter.internalTree(), infixVisitor);
  QCOMPARE(infixVisitor.toString(), expectedString);
  infixVisitor.clear();

  filter.setFilter(clientId == "A");
  expectedString = "id==\"A\"";
  traverseExpressionTree(filter.internalTree(), infixVisitor);
  QCOMPARE(infixVisitor.toString(), expectedString);
  infixVisitor.clear();

  QString matchinString = "M";
  filter.setFilter(clientId == matchinString);
  expectedString = "id==\"M\"";
  traverseExpressionTree(filter.internalTree(), infixVisitor);
  QCOMPARE(infixVisitor.toString(), expectedString);
  infixVisitor.clear();

  filter.setFilter(clientId == Like("?B*"));
  expectedString = "id Like '?B*'";
  traverseExpressionTree(filter.internalTree(), infixVisitor);
  QCOMPARE(infixVisitor.toString(), expectedString);
  infixVisitor.clear();

  filter.setFilter(clientId != 15);
  expectedString = "id!=15";
  traverseExpressionTree(filter.internalTree(), infixVisitor);
  QCOMPARE(infixVisitor.toString(), expectedString);
  infixVisitor.clear();

  filter.setFilter(clientId < 20);
  expectedString = "id<20";
  traverseExpressionTree(filter.internalTree(), infixVisitor);
  QCOMPARE(infixVisitor.toString(), expectedString);
  infixVisitor.clear();

  filter.setFilter(clientId <= 20);
  expectedString = "id<=20";
  traverseExpressionTree(filter.internalTree(), infixVisitor);
  QCOMPARE(infixVisitor.toString(), expectedString);
  infixVisitor.clear();

  filter.setFilter(clientId > 20);
  expectedString = "id>20";
  traverseExpressionTree(filter.internalTree(), infixVisitor);
  QCOMPARE(infixVisitor.toString(), expectedString);
  infixVisitor.clear();

  filter.setFilter(clientId >= 20);
  expectedString = "id>=20";
  traverseExpressionTree(filter.internalTree(), infixVisitor);
  QCOMPARE(infixVisitor.toString(), expectedString);
  infixVisitor.clear();

  filter.setFilter(clientId == 2 || clientId < 12);
  expectedString = "(id==2)||(id<12)";
  traverseExpressionTree(filter.internalTree(), infixVisitor);
  QCOMPARE(infixVisitor.toString(), expectedString);
  infixVisitor.clear();

  filter.setFilter(clientId > 0 && clientId < 100);
  expectedString = "(id>0)&&(id<100)";
  traverseExpressionTree(filter.internalTree(), infixVisitor);
  QCOMPARE(infixVisitor.toString(), expectedString);
  infixVisitor.clear();

  filter.setFilter(clientId == 2 || clientId < 12 || clientId == 22);
  expectedString = "((id==2)||(id<12))||(id==22)";
  traverseExpressionTree(filter.internalTree(), infixVisitor);
  QCOMPARE(infixVisitor.toString(), expectedString);
  infixVisitor.clear();

  filter.setFilter(clientId == 1 || ((clientId > 2) && (clientId < 22)));
  expectedString = "(id==1)||((id>2)&&(id<22))";
  traverseExpressionTree(filter.internalTree(), infixVisitor);
  QCOMPARE(infixVisitor.toString(), expectedString);
  infixVisitor.clear();
}

void ExpressionTreeTest::joinConstraintExpressionTest()
{
//   SelectEntity person(EntityName("Person"), "P");
//   SelectEntity address(EntityName("Address"), "ADR");
// 
//   SelectField personId(person, FieldName("id"));
//   SelectField addressPersonId(address, FieldName("personId"));

  QueryEntity person("Person", EntityAlias("P"));
  QueryEntity address("Address", EntityAlias("ADR"));

  QueryField personId(person, "id");
  QueryField addressPersonId(address, "personId");
  
  ExpressionToInfixStringVisitor infixVisitor;
  QString expectedString;

  JoinConstraintExpression join;
  QVERIFY(join.isNull());

  join.setJoin(addressPersonId == personId);
  QVERIFY(!join.isNull());
  expectedString = "personId==id";
  traverseExpressionTree(join.internalTree(), infixVisitor);
  QCOMPARE(infixVisitor.toString(), expectedString);
  infixVisitor.clear();
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  ExpressionTreeTest test;

  return QTest::qExec(&test, argc, argv);
}
