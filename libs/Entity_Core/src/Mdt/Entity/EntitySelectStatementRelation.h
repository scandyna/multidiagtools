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
#ifndef MDT_ENTITY_ENTITY_SELECT_STATEMENT_RELATION_H
#define MDT_ENTITY_ENTITY_SELECT_STATEMENT_RELATION_H

#include "Relation.h"
#include "RelationAlgorithm.h"
#include "Mdt/QueryExpression/JoinConstraintExpression.h"
#include "Mdt/QueryExpression/ExpressionTree.h"
#include "Mdt/QueryExpression/EntityAndField.h"
#include "MdtEntity_CoreExport.h"
#include <QString>
#include <QStringList>
#include <type_traits>
#include <vector>

namespace Mdt{ namespace Entity{ namespace Impl{

  /*! \brief Select the type in \a A and \a B that is not \a T
   */
  template<typename T, typename A, typename B>
  struct SelectOppositeType
  {
    using type = typename std::conditional< !std::is_same<T, A>::value, A, B >::type;
  };

  /*! \internal Build a join constraint expression tree
   *
   * \pre Count of fields in \a pk and \a fk must be the same
   * \pre \a pk and \a fk must not be empty
   */
  Mdt::QueryExpression::ExpressionTree MDT_ENTITY_CORE_EXPORT buildJoinConstraintExpressionTree(
                                                                const QString & primaryEntityName, const QStringList & pk,
                                                                const QString & foreignEntityName, const QStringList & fk);

  /*! \brief Create a join constraint expression from a relation
   */
  template<typename EntityRelation>
  Mdt::QueryExpression::JoinConstraintExpression buildJoinConstraintExpression()
  {
    using PrimaryEntity = typename EntityRelation::primary_entity;
    using ForeignEntity = typename EntityRelation::foreign_entity;

    Mdt::QueryExpression::JoinConstraintExpression join;

    const auto pkFieldList = getPrimaryKeyFieldNameList<EntityRelation>();
    const auto fkFieldList = getForeignKeyFieldNameList<EntityRelation>();
    Q_ASSERT(pkFieldList.size() == fkFieldList.size());

    const auto expressionTree = buildJoinConstraintExpressionTree(PrimaryEntity::def().entityName(), pkFieldList, ForeignEntity::def().entityName(), fkFieldList);
    Q_ASSERT(!expressionTree.isNull());
    join.setJoinConstraintExpressionTree(expressionTree);

    return join;
  }

}}} // namespace Mdt{ namespace Entity namespace Impl{

#endif // #ifndef MDT_ENTITY_ENTITY_SELECT_STATEMENT_RELATION_H
