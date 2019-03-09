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
#include "AbstractExpressionTreeVisitor.h"
#include "EntityAndField.h"
#include <boost/variant.hpp>

namespace Mdt{ namespace QueryExpression{

namespace AbstractExpressionTreeVisitorImpl{


  /*! \internal
   */
  class CallProcessPreorder : public boost::static_visitor<>
  {
    public:

    CallProcessPreorder(AbstractExpressionTreeVisitor & treeVisitor)
    : mTreeVisitor(treeVisitor)
    {
    }

    void operator()(ComparisonOperator op)
    {
      mTreeVisitor.processPreorder(op);
    }

    void operator()(LogicalOperator op)
    {
      mTreeVisitor.processPreorder(op);
    }

    [[deprecated]]
    void operator()(const SelectFieldVariant &)
    {
    }

//     [[deprecated]]
//     void operator()(const QueryFieldVariant & field)
//     {
// //       CallProcessPreorderForQueryFieldVariant visitor(mTreeVisitor);
// //       boost::apply_visitor(visitor, field.internalVariant());
//     }

    void operator()(const EntityAndField & field)
    {
      mTreeVisitor.processPreorder(field);
    }

    void operator()(const QVariant & value)
    {
      mTreeVisitor.processPreorder(value);
    }

    void operator()(const LikeExpressionData & data)
    {
      mTreeVisitor.processPreorder(data);
    }

    private:

    AbstractExpressionTreeVisitor & mTreeVisitor;
  };

  /*! \internal
   */
  class CallProcessInorder : public boost::static_visitor<>
  {
    public:

    CallProcessInorder(AbstractExpressionTreeVisitor & treeVisitor)
    : mTreeVisitor(treeVisitor)
    {
    }

    void operator()(ComparisonOperator op)
    {
      mTreeVisitor.processInorder(op);
    }

    void operator()(LogicalOperator op)
    {
      mTreeVisitor.processInorder(op);
    }

    [[deprecated]]
    void operator()(const SelectFieldVariant &)
    {
    }
    
//     [[deprecated]]
//     void operator()(const QueryFieldVariant & field)
//     {
// //       CallProcessInorderForQueryFieldVariant visitor(mTreeVisitor);
// //       boost::apply_visitor(visitor, field.internalVariant());
//     }

    void operator()(const EntityAndField & field)
    {
      mTreeVisitor.processInorder(field);
    }

    void operator()(const QVariant & value)
    {
      mTreeVisitor.processInorder(value);
    }

    void operator()(const LikeExpressionData & data)
    {
      mTreeVisitor.processInorder(data);
    }

    private:

    AbstractExpressionTreeVisitor & mTreeVisitor;
  };

  /*! \internal
   */
  class CallProcessPostorder : public boost::static_visitor<>
  {
    public:

    CallProcessPostorder(AbstractExpressionTreeVisitor & treeVisitor)
    : mTreeVisitor(treeVisitor)
    {
    }

    void operator()(ComparisonOperator op)
    {
      mTreeVisitor.processPostorder(op);
    }

    void operator()(LogicalOperator op)
    {
      mTreeVisitor.processPostorder(op);
    }

    [[deprecated]]
    void operator()(const SelectFieldVariant &)
    {
    }
    
//     [[deprecated]]
//     void operator()(const QueryFieldVariant & field)
//     {
//       CallProcessPostorderForQueryFieldVariant visitor(mTreeVisitor);
//       boost::apply_visitor(visitor, field.internalVariant());
//     }

    void operator()(const EntityAndField & field)
    {
      mTreeVisitor.processPostorder(field);
    }

    void operator()(const QVariant & value)
    {
      mTreeVisitor.processPostorder(value);
    }

    void operator()(const LikeExpressionData & data)
    {
      mTreeVisitor.processPostorder(data);
    }

    private:

    AbstractExpressionTreeVisitor & mTreeVisitor;
  };

} // namespace AbstractExpressionTreeVisitorImpl{

void AbstractExpressionTreeVisitor::preorder(ExpressionTreeVertex v, const ExpressionTreeGraph & g)
{
  const auto vertexVariant = g[v];
  AbstractExpressionTreeVisitorImpl::CallProcessPreorder visitor(*this);
  boost::apply_visitor(visitor, vertexVariant);
}

void AbstractExpressionTreeVisitor::inorder(ExpressionTreeVertex v, const ExpressionTreeGraph & g)
{
  const auto vertexVariant = g[v];
  AbstractExpressionTreeVisitorImpl::CallProcessInorder visitor(*this);
  boost::apply_visitor(visitor, vertexVariant);
}

void AbstractExpressionTreeVisitor::postorder(ExpressionTreeVertex v, const ExpressionTreeGraph & g)
{
  const auto vertexVariant = g[v];
  AbstractExpressionTreeVisitorImpl::CallProcessPostorder visitor(*this);
  boost::apply_visitor(visitor, vertexVariant);
}

void AbstractExpressionTreeVisitor::onEdge(ExpressionTreeEdge, const ExpressionTreeGraph &)
{
}

void AbstractExpressionTreeVisitor::postEdge(ExpressionTreeEdge, const ExpressionTreeGraph &)
{
}

}} // namespace Mdt{ namespace QueryExpression{
