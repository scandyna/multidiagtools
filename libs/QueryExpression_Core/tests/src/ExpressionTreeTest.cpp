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

//   enum class Operator
//   {
//     BinaryEqual,
//     LogicalOr
//   };

  QString operatorToString(ComparisonOperator op)
  {
    switch(op){
      case ComparisonOperator::Equal:
        return "==";
    }
    return QString();
  }

//   using ExpressionTreeVertexData = boost::variant<Operator, QVariant, SelectFieldVariant>;
// 
//   /*! \internal Graph for ExpressionTree
//    */
//   using ExpressionGraph = boost::adjacency_list<
//     boost::vecS,              // OutEdgeList: vecS (std::vector)
//     boost::vecS,              // VertexList: vecS (std::vector)
//     boost::directedS,         // Directed: directedS (directed graph)
//     ExpressionTreeVertexData, // VertexProperties: we use our custom struct
//     boost::no_property,       // EdgeProperties: we use no property
//     boost::no_property,       // GraphProperties: no_property (None)
//     boost::listS              // EdgeList: listS (std::list)
//   >;
// 
//   /*! \brief
//    */
//   using ExpressionTreeVertex = boost::graph_traits<ExpressionGraph>::vertex_descriptor;

//   /*! \brief 
//    *
//    * The following examples assume using the QueryExpression namespace:
//    * \code
//    * using namespace Mdt::QueryExpression
//    * \endcode
//    */
//   class ExpressionTree
//   {
//    public:
// 
//     ExpressionTreeVertex addNode(const SelectFieldVariant & left, Operator op, const QVariant & right)
//     {
//       const auto lv = addVertex(left);
//       const auto rv = addVertex(right);
//       const auto v = addVertex(op);
// 
//       mRoot = v;
// 
//       addEdge(v, lv);
//       addEdge(v, rv);
//       
//       qDebug() << "Add node Field" << operatorToString(op) << right << ", lv: " << lv << ", rv: " << rv << " , v: " << v << ", root: " << mRoot;
// 
//       return v;
//     }
// 
//     ExpressionTreeVertex addNode(ExpressionTreeVertex leftChild, Operator op, ExpressionTreeVertex rightChild)
//     {
//       const auto v = addVertex(op);
//       
//       mRoot = v;
//       
//       addEdge(v, leftChild);
//       addEdge(v, rightChild);
//       
//       qDebug() << "Add OR node, v: " << v << ", left child: " << leftChild << ", right child: " << rightChild << ", root: " << mRoot;
//       
//       return v;
//     }
// 
//     void clear()
//     {
//       mGraph.clear();
//     }
// 
//     const ExpressionGraph & internalGraph() const
//     {
//       return mGraph;
//     }
// 
//     ExpressionTreeVertex internalRootVertex() const
//     {
//       return mRoot;
//     }
// 
//    private:
// 
//     ExpressionTreeVertex addVertex(Operator op)
//     {
//       return boost::add_vertex(ExpressionTreeVertexData(op) ,mGraph);
//     }
// 
//     ExpressionTreeVertex addVertex(const SelectFieldVariant & field)
//     {
//       return boost::add_vertex(ExpressionTreeVertexData(field), mGraph);
//     }
// 
//     ExpressionTreeVertex addVertex(const QVariant & value)
//     {
//       return boost::add_vertex(ExpressionTreeVertexData(value) ,mGraph);
//     }
// 
//     void addEdge(ExpressionTreeVertex left, ExpressionTreeVertex right)
//     {
//       boost::add_edge(left, right, mGraph);
//     }
// 
//     ExpressionGraph mGraph;
//     ExpressionTreeVertex mRoot;
//   };

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
    boost::apply_visitor(visitor, vertexVariant);
    out << " [label=\"" << v << "\n" << visitor.str << "\"]";
  }

 private:

  const Mdt::QueryExpression::ExpressionTreeGraph & mGraph;
};

struct ExpressionTreeVisitor
{
//   using event_filter = boost::on_discover_vertex;
  using event_filter = boost::on_finish_vertex;

  void operator()(Mdt::QueryExpression::ExpressionTreeVertex v, const Mdt::QueryExpression::ExpressionTreeGraph & g)
  {
    const auto vertexVariant = g[v];
    VertexPropertyWriterVertexVisitor visitor;
    boost::apply_visitor(visitor, vertexVariant);
    qDebug() << QString::fromStdString(visitor.str);
    if(visitor.str == "OR"){
      str += "(";
    }else{
      str += visitor.str;
    }
    qDebug() << "Current str: " << QString::fromStdString(str);
  }

  std::string str;
};

std::string vertexVariantToString(Mdt::QueryExpression::ExpressionTreeVertex v, const Mdt::QueryExpression::ExpressionTreeGraph & g)
{
  const auto vertexVariant = g[v];
  VertexPropertyWriterVertexVisitor visitor;
  boost::apply_visitor(visitor, vertexVariant);
  return visitor.str;
}

/// \todo Check boost visitor for order (g, v) or (v, g) ? -> it seems to be (v, g) and (e, g)
template<typename TreeGraph>
class TreeGraphVisitor
{
 public:

  using Vertex = typename boost::graph_traits<TreeGraph>::vertex_descriptor;
  using Edge = typename boost::graph_traits<TreeGraph>::edge_descriptor;

  void preorder(Vertex v, const TreeGraph & g)
  {
//     qDebug() << "Pre, v: " << v << QString::fromStdString(vertexVariantToString(v, g));
  }

  void inorder(Vertex v, const TreeGraph & g)
  {
    qDebug() << "In, v: " << v << QString::fromStdString(vertexVariantToString(v, g));
    
  }

  void postorder(Vertex v, const TreeGraph & g)
  {
//     qDebug() << "Post, v: " << v << QString::fromStdString(vertexVariantToString(v, g));
  }

  void onEdge(Edge e, const TreeGraph & g)
  {
  }

  void postEdge(Edge e, const TreeGraph & g)
  {
  }

  std::string str;
};

// template<typename Graph>
// constexpr bool isDirectedGraph()
// {
//   return std::is_same< typename boost::graph_traits<Graph>::directed_category, boost::directed_tag >::value;
// }

// /*! \brief
//  *
//  * Source: http://boost-users.boost.narkive.com/sN4cBmiE/bgl-traversal-algorithm-for-tree
//  *
//  * \todo order of arguments should match DFS, BFS provided by boost::graph
//  */
// template<typename TreeGraph, typename TreeGraphVisitor>
// void traverseTreeGraph(typename boost::graph_traits<TreeGraph>::vertex_descriptor v, TreeGraph & g, TreeGraphVisitor & visitor)
// {
//   static_assert( isDirectedGraph<TreeGraph>()  , "traverseTreeGraph() requires a directed graph (else it will run to infinite loop)" );
// 
//   visitor.preorder(v, g);
// 
//   typename boost::graph_traits<TreeGraph>::adjacency_iterator it, end;
//   std::tie(it, end) = boost::adjacent_vertices(v, g);
//   if(it != end){
//     // Current node is not a leaf
//     visitor.onEdge( boost::edge(v, *it, g).first, g );
//     traverseTreeGraph(*it, g, visitor);
//     visitor.postEdge( boost::edge(v, *it++, g).first, g );
//     while(it != end){
//       visitor.inorder(v, g);
//       visitor.onEdge( boost::edge(v, *it, g).first, g );
//       traverseTreeGraph(*it, g, visitor);
//       visitor.postEdge( boost::edge(v, *it++, g).first, g );
//     }
//   }else{
//     visitor.inorder(v, g);
//     visitor.postorder(v, g);
//   }
// }

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
  
  std::ofstream graphVizFile("graphvizOut.dot");
  QVERIFY(graphVizFile.good());
  VertexPropertyWriter vp( tree.internalGraph() );
  boost::write_graphviz(graphVizFile, tree.internalGraph(), vp);

  qDebug() << "BFS ...";
  boost::bfs_visitor<ExpressionTreeVisitor> bfsVis;
  boost::breadth_first_search( tree.internalGraph(), tree.internalRootVertex(), boost::visitor(bfsVis) );
  
  qDebug() << "DFS ...";
  boost::dfs_visitor<ExpressionTreeVisitor> dfsVis;
  boost::depth_first_search( tree.internalGraph(), boost::visitor(dfsVis) );

  qDebug() << "Traverse tree ...";
  TreeGraphVisitor<Mdt::QueryExpression::ExpressionTreeGraph> tvis;
  traverseTreeGraph( tree.internalGraph(), tree.internalRootVertex(), tvis );
  qDebug() << QString::fromStdString(tvis.str);
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
  leftVertex = tree.addNode(clientId, ComparisonOperator::Equal, 44);
  QCOMPARE(tree.internalRootVertex(), leftVertex);
  rightVertex = tree.addNode(clientId, ComparisonOperator::Less, 25); // Root vertex is ambigous for now
  rootVertex = tree.addNode(leftVertex, LogicalOperator::Or, rightVertex);
  QCOMPARE(tree.internalRootVertex(), rootVertex);
  
  qDebug() << "L: " << leftVertex << ", R: " << rightVertex << ", root: " << rootVertex << ", Groot: " << tree.internalRootVertex();
  

  QFAIL("Not complete");
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

  QFAIL("Not complete");
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
