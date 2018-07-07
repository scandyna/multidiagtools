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
#include "AbstractExpressionTreeVisitor.h"
#include <boost/variant.hpp>

namespace Mdt{ namespace QueryExpression{

namespace AbstractExpressionTreeVisitorImpl{

  /*! \internal
   */
  class CallForSelectFieldVariantBase : public boost::static_visitor<>
  {
    public:

    CallForSelectFieldVariantBase(AbstractExpressionTreeVisitor & treeVisitor)
    : mTreeVisitor(treeVisitor)
    {
    }

    void operator()(const SelectAllField)
    {
      Q_ASSERT_X(false, "processInorder", "SelectAllField is not allowed in a condition expression");
    }

    protected:

    AbstractExpressionTreeVisitor & treeVisitor()
    {
      return mTreeVisitor;
    }

    private:

    AbstractExpressionTreeVisitor & mTreeVisitor;
  };

  /*! \internal
   */
  class CallProcessPreorderForSelectFieldVariant : public CallForSelectFieldVariantBase
  {
    public:

    using ParentClass = CallForSelectFieldVariantBase;
    using ParentClass::ParentClass;
    using ParentClass::operator();

    void operator()(const EntityAndField & field)
    {
      treeVisitor().processPreorder(field);
    }
  };

  /*! \internal
   */
  class CallProcessInorderForSelectFieldVariant : public CallForSelectFieldVariantBase
  {
    public:

    using ParentClass = CallForSelectFieldVariantBase;
    using ParentClass::ParentClass;
    using ParentClass::operator();

    void operator()(const EntityAndField & field)
    {
      treeVisitor().processInorder(field);
    }
  };

  /*! \internal
   */
  class CallProcessPostorderForSelectFieldVariant : public CallForSelectFieldVariantBase
  {
    public:

    using ParentClass = CallForSelectFieldVariantBase;
    using ParentClass::ParentClass;
    using ParentClass::operator();

    void operator()(const EntityAndField & field)
    {
      treeVisitor().processPostorder(field);
    }
  };

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

    void operator()(const SelectFieldVariant & field)
    {
      CallProcessPreorderForSelectFieldVariant visitor(mTreeVisitor);
      boost::apply_visitor(visitor, field);
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

    void operator()(const SelectFieldVariant & field)
    {
      CallProcessInorderForSelectFieldVariant visitor(mTreeVisitor);
      boost::apply_visitor(visitor, field);
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

    void operator()(const SelectFieldVariant & field)
    {
      CallProcessPostorderForSelectFieldVariant visitor(mTreeVisitor);
      boost::apply_visitor(visitor, field);
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