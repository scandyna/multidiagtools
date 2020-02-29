/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_ENTITY_PRIMARY_KEY_RECORD_FILTER_EXPRESSION_H
#define MDT_ENTITY_PRIMARY_KEY_RECORD_FILTER_EXPRESSION_H

#include "PrimaryKeyRecord.h"
#include "PrimaryKeyRecordField.h"
#include "FieldAt.h"
#include "TypeTraits/IsEntity.h"
#include "TypeTraits/IsEntityDef.h"
#include "Mdt/QueryExpression/SelectField.h"
#include "Mdt/QueryExpression/FilterExpression.h"
#include "Mdt/QueryExpression/ExpressionTree.h"
#include "Mdt/QueryExpression/ComparisonOperator.h"
#include "Mdt/QueryExpression/LogicalOperator.h"
#include "MdtEntity_CoreExport.h"

namespace Mdt{ namespace Entity{

  /*! \brief Get a select field from a primary key record field
   */
  template<typename Entity>
  Mdt::QueryExpression::SelectField selectFieldFromPrimaryKeyRecordField(const PrimaryKeyRecordField & field)
  {
    static_assert( TypeTraits::IsEntity<Entity>::value , "Entity must be a entity type" );

    using Mdt::QueryExpression::SelectField;
    using Mdt::QueryExpression::EntityName;
//     using Mdt::QueryExpression::FieldName;
    using EntityDef = typename Entity::def_type;

//     return SelectField( EntityName(EntityDef::entityName()), FieldName(fieldNameAt(EntityDef{}, field.fieldIndex())) );
  }

  template<typename Entity>
  Mdt::QueryExpression::ExpressionTreeVertex addPrimaryKeyRecordFieldToExpressionTree(Mdt::QueryExpression::ExpressionTree & tree, const PrimaryKeyRecordField & pkField)
  {
    using Mdt::QueryExpression::ComparisonOperator;

    const auto selectField = selectFieldFromPrimaryKeyRecordField<Entity>(pkField);

    return tree.addNode(selectField, ComparisonOperator::Equal, pkField.value() );
  }

  /*! \brief Get a filter expression from \a pkRecord
   *
   * \pre \a pkRecord must not be null
   */
  template<typename Entity>
  Mdt::QueryExpression::FilterExpression filterExpressionFromPrimaryKeyRecord(const PrimaryKeyRecord & pkRecord)
  {
    static_assert( TypeTraits::IsEntity<Entity>::value , "Entity must be a entity type" );

    Q_ASSERT(pkRecord.fieldCount() >= 1);

    using Mdt::QueryExpression::FilterExpression;
    using Mdt::QueryExpression::ExpressionTree;
    using Mdt::QueryExpression::LogicalOperator;

    FilterExpression filter;
    ExpressionTree tree;

    auto leftChild = addPrimaryKeyRecordFieldToExpressionTree<Entity>(tree, pkRecord.fieldAt(0));
    for(int i = 1; i < pkRecord.fieldCount(); ++i){
      auto rightChild = addPrimaryKeyRecordFieldToExpressionTree<Entity>(tree, pkRecord.fieldAt(i));
      leftChild = tree.addNode(leftChild, LogicalOperator::And, rightChild);
    }
    filter.setFilterExpressionTree(tree);

    return filter;
  }

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_PRIMARY_KEY_RECORD_FILTER_EXPRESSION_H
