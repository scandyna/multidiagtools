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

#include <QDebug>

namespace Mdt{ namespace QueryExpression{

void AbstractExpressionTreeVisitor::preorder(ExpressionTreeVertex v, const ExpressionTreeGraph & g)
{

}

void AbstractExpressionTreeVisitor::inorder(ExpressionTreeVertex v, const ExpressionTreeGraph & g)
{
  qDebug() << "inorder, v: " << v;

  const auto vertexVariant = g[v];
  AbstractExpressionTreeVisitorImpl::CallProcessInorder visitor(*this);
  boost::apply_visitor(visitor, vertexVariant);
}

void AbstractExpressionTreeVisitor::postorder(ExpressionTreeVertex v, const ExpressionTreeGraph & g)
{

}

void AbstractExpressionTreeVisitor::onEdge(ExpressionTreeEdge e, const ExpressionTreeGraph & g)
{
}

void AbstractExpressionTreeVisitor::postEdge(ExpressionTreeEdge e, const ExpressionTreeGraph & g)
{
}

}} // namespace Mdt{ namespace QueryExpression{