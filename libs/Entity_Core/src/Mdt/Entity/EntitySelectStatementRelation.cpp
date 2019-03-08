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
#include "EntitySelectStatementRelation.h"
#include "Mdt/QueryExpression/SelectFieldVariant.h"
#include "Mdt/QueryExpression/EntityAndField.h"
#include "Mdt/QueryExpression/FieldName.h"
#include "Mdt/QueryExpression/EntityName.h"
#include "Mdt/QueryExpression/SelectEntity.h"
#include "Mdt/QueryExpression/ComparisonOperator.h"
#include "Mdt/QueryExpression/LogicalOperator.h"
#include "Mdt/QueryExpression/ExpressionTreeGraph.h"

using namespace Mdt::QueryExpression;

namespace Mdt{ namespace Entity{ namespace Impl{

SelectFieldVariant createSelectFieldVariant(const QString & entityName, const QString & fieldName)
{
//   return SelectFieldVariant( EntityAndField( SelectEntity(EntityName(entityName)), FieldName(fieldName) ) );
}

ExpressionTreeVertex addEqualityToTree(ExpressionTree & tree,
                                        const QString & primaryEntityName, const QString & pkFieldName,
                                        const QString & foreignEntityName, const QString & fkFieldName)
{
//   return tree.addNode( createSelectFieldVariant(foreignEntityName, fkFieldName), ComparisonOperator::Equal, createSelectFieldVariant(primaryEntityName, pkFieldName) );
}

ExpressionTree buildJoinConstraintExpressionTree(const QString& primaryEntityName, const QStringList& pk,
                                                 const QString& foreignEntityName, const QStringList& fk)
{
  Q_ASSERT(pk.size() == fk.size());
  Q_ASSERT(!pk.isEmpty());

  ExpressionTree tree;
  ExpressionTreeVertex left, right;

  const int n = pk.size();
  left = addEqualityToTree(tree, primaryEntityName, pk.at(0), foreignEntityName, fk.at(0));
  for(int i = 1; i < n; ++i){
    right = addEqualityToTree(tree, primaryEntityName, pk.at(i), foreignEntityName, fk.at(i));
    left = tree.addNode(left, LogicalOperator::And, right);
  }

  return tree;
}


}}} // namespace Mdt{ namespace Entity{ namespace Impl{
