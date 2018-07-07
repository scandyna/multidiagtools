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
#include "ExpressionTreeTest.h"
#include "Mdt/QueryExpression/SelectField.h"
#include "Mdt/QueryExpression/ExpressionTree.h"
#include "Mdt/QueryExpression/AbstractExpressionTreeVisitor.h"
#include "Mdt/QueryExpression/TravserseTreeGraph.h"
#include "Mdt/QueryExpression/FilterExpression.h"
#include <QMetaType>

#include "Mdt/QueryExpression/Terminal.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/proto/proto_fwd.hpp>
#include <boost/proto/traits.hpp>
#include <boost/proto/matches.hpp>
#include <boost/proto/transform/when.hpp>
#include <boost/proto/transform.hpp>

#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/depth_first_search.hpp>
// #include <boost/graph/tree_traits.hpp>
// #include <boost/graph/graph_as_tree.hpp>
#include <boost/graph/graph_traits.hpp>
#include <tuple>

#include <type_traits>

#include <boost/variant.hpp>

#include <boost/graph/graphviz.hpp>
#include <QDebug>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace Mdt{ namespace QueryExpression{

  struct AddEqualExpressionToTree : boost::proto::callable
  {
    using result_type = ExpressionTreeVertex;

    ExpressionTreeVertex operator()(const SelectFieldVariant & left, const QVariant & right, ExpressionTree & tree) const
    {
      return tree.addNode(left, ComparisonOperator::Equal, right);
    }
  };

  struct AddLogicalOrExpressionToTree : boost::proto::callable
  {
    using result_type = ExpressionTreeVertex;

    ExpressionTreeVertex operator()(ExpressionTreeVertex leftChild, ExpressionTreeVertex rightChild, ExpressionTree & tree) const
    {
      return tree.addNode(leftChild, LogicalOperator::Or, rightChild);
    }
  };

  struct ComparisonTransform : boost::proto::or_<
    boost::proto::when<
      boost::proto::equal_to<LeftTerminal, RightTerminal>,
      boost::proto::call<AddEqualExpressionToTree( boost::proto::_value(boost::proto::_left), boost::proto::_value(boost::proto::_right), boost::proto::_data )>
    >
  >
  {
  };

  struct LogicalTransform : boost::proto::or_<
    boost::proto::when<
      boost::proto::logical_or<LogicalTransform, LogicalTransform>,
      boost::proto::call<AddLogicalOrExpressionToTree( LogicalTransform(boost::proto::_left), LogicalTransform(boost::proto::_right), boost::proto::_data )>
    >,
    ComparisonTransform
  >
  {
  };

}} // namespace Mdt{ namespace QueryExpression{

using namespace Mdt::QueryExpression;

class VertexPropertyWriterVertexVisitor : public boost::static_visitor<>
{
 public:

  void operator()(ComparisonOperator op)
  {
    switch(op){
      case ComparisonOperator::Equal:
        str = "==";
        break;
    }
  }

  void operator()(LogicalOperator op)
  {
    switch(op){
      case LogicalOperator::Or:
        str = "OR";
        break;
    }
  }

  void operator()(const Mdt::QueryExpression::SelectFieldVariant & field)
  {
    str = "Field";
  }

  void operator()(const QVariant & value)
  {
    str = value.toString().toStdString();
  }

  std::string str;
};

class VertexPropertyWriter
{
 public:

  VertexPropertyWriter(const Mdt::QueryExpression::ExpressionTreeGraph & graph)
   : mGraph(graph)
  {
  }

  void operator()(std::ostream & out, ExpressionTreeVertex v) const
  {
    qDebug() << "Prop of v: " << v;
    const auto vertexVariant = mGraph[v];
    VertexPropertyWriterVertexVisitor visitor;
//     boost::apply_visitor(visitor, vertexVariant);
    out << " [label=\"" << v << "\n" << visitor.str << "\"]";
  }

 private:

  const Mdt::QueryExpression::ExpressionTreeGraph & mGraph;
};

std::string vertexVariantToString(Mdt::QueryExpression::ExpressionTreeVertex v, const Mdt::QueryExpression::ExpressionTreeGraph & g)
{
  const auto vertexVariant = g[v];
  VertexPropertyWriterVertexVisitor visitor;
//   boost::apply_visitor(visitor, vertexVariant);
  return visitor.str;
}

void ExpressionTreeTest::sandbox()
{
  SelectEntity person(EntityName("Person"), "P");
  SelectField clientId(person, FieldName("id"), "ID");
  ExpressionTree tree;
  //Vertex root;

  LogicalTransform transform;
  qDebug() << "***  clientId == 25  ***";
  transform( clientId == 25, 0, tree);
  
  tree.clear();
  qDebug() << "***  (clientId == 25) || (clientId == 44)  ***";
  transform( (clientId == 25) || (clientId == 44), 0, tree);
  
  tree.clear();
  qDebug() << "***  (clientId == 25) || (clientId == 44) || (clientId == 256)  ***";
  transform( (clientId == 25) || (clientId == 44) || (clientId == 256), 0, tree);
  
//   std::ofstream graphVizFile("graphvizOut.dot");
//   QVERIFY(graphVizFile.good());
//   VertexPropertyWriter vp( tree.internalGraph() );
//   boost::write_graphviz(graphVizFile, tree.internalGraph(), vp);
}

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
    using Mdt::QueryExpression::ComparisonOperator;
    switch(op){
      case LogicalOperator::And:
        return "&&";
      case LogicalOperator::Or:
        return "||";
    }
  }

  static bool isTypeString(const QVariant & v)
  {
    const auto type = static_cast<QMetaType::Type>(v.type());
    return type == QMetaType::QString;
  }

};

/*
 * Test visitors
 */

class ExpressionToPrefixStringVisitor : public AbstractExpressionToStringVisitor
{
 public:

  void processPreorder(Mdt::QueryExpression::ComparisonOperator op) override
  {
    qDebug() << "processPreorder: " << comparisonOperatorToString(op);
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

  void processPreorder(Mdt::QueryExpression::LogicalOperator op) override
  {
    mExpressionString += "(";
  }

  void processPostorder(Mdt::QueryExpression::LogicalOperator op) override
  {
    mExpressionString += ")";
  }

  void processInorder(Mdt::QueryExpression::ComparisonOperator op) override
  {
    qDebug() << "processInorder: " << comparisonOperatorToString(op);
    mExpressionString += comparisonOperatorToString(op);
  }

  void processInorder(Mdt::QueryExpression::LogicalOperator op) override
  {
    qDebug() << "processInorder: " << logicalOperatorToString(op);
    qDebug() << "-> cur exp: " << mExpressionString;
    mExpressionString += ")" + logicalOperatorToString(op) + "(";
  }

  void processInorder(const Mdt::QueryExpression::EntityAndField & field) override
  {
    qDebug() << "processInorder: " << field.fieldAliasOrName();
    mExpressionString += field.fieldAliasOrName();
  }

  void processInorder(const QVariant & value) override
  {
    qDebug() << "processInorder: " << value;
    if(isTypeString(value)){
      mExpressionString += "\"" + value.toString() + "\"";
    }else{
      mExpressionString += value.toString();
    }
  }

  void processInorder(const LikeExpressionData& data) override
  {
    mExpressionString += data.toString();
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
    qDebug() << "processPostorder: " << comparisonOperatorToString(op);
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
  qDebug() << "Next...........";
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

  SelectEntity person(EntityName("Person"), "P");
  SelectField clientId(person, FieldName("id"));
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

  filter.setFilter(clientId == Like("?B*"));
  expectedString = "id==?B*";
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
