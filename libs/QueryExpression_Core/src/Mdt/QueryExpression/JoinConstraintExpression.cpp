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
#include "JoinConstraintExpression.h"
#include "Mdt/QueryExpression/EntityAndField.h"
#include "Mdt/QueryExpression/ComparisonOperator.h"
#include "Mdt/QueryExpression/LogicalOperator.h"
#include "Mdt/QueryExpression/ExpressionTreeGraph.h"

namespace Mdt{ namespace QueryExpression{

void JoinConstraintExpression::clear()
{
  mTree.clear();
}

JoinConstraintExpression JoinConstraintExpression::fromEquality(const QueryEntity & primaryEntity, const QStringList & pk,
                                                                const QueryEntity & foreignEntity, const QStringList & fk)
{
  Q_ASSERT(!primaryEntity.isNull());
  Q_ASSERT(!foreignEntity.isNull());
  Q_ASSERT(!pk.isEmpty());
  Q_ASSERT(pk.size() == fk.size());

  JoinConstraintExpression expression;
  ExpressionTreeVertex leftVertex;
  ExpressionTreeVertex rightVertex;

  /*
   *              (AND)
   *             /     \
   *         (AND)      (==)
   *        /    \      /  \
   *     (==)     (==) (FC) (PC)
   *     /  \     /  \
   *  (FA) (PA) (FB) (PB)
   */
  const int n = pk.size();
  Q_ASSERT(n > 0);
  leftVertex = expression.mTree.addNode( EntityAndField(foreignEntity, fk.at(0)), ComparisonOperator::Equal, EntityAndField(primaryEntity, pk.at(0)) );
  for(int i=1; i < n; ++i){
    rightVertex = expression.mTree.addNode( EntityAndField(foreignEntity, fk.at(i)), ComparisonOperator::Equal, EntityAndField(primaryEntity, pk.at(i)) );
    leftVertex = expression.mTree.addNode(leftVertex, LogicalOperator::And, rightVertex);
  }

  return expression;
}

}} // namespace Mdt{ namespace QueryExpression{
