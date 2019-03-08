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
#include "SelectStatement.h"

namespace Mdt{ namespace QueryExpression{

void SelectStatement::setEntityName(const QString& name, const EntityAlias & entityAlias)
{
  Q_ASSERT(!EntityName(name).isNull());

  mEntity.setNameAndAlias(name, entityAlias);
}

// void SelectStatement::setEntityName(const EntityName& name, const QString & alias)
// {
//   Q_ASSERT(!name.isNull());
//   Q_ASSERT(!alias.trimmed().isEmpty());
// 
//   mEntity.setNameAndAlias(name, alias);
// }

void SelectStatement::setEntity(const QueryEntity & entity)
{
  Q_ASSERT(!entity.isNull());

  mEntity = entity;
}

void SelectStatement::selectAllFields()
{
  mFieldList.clear();
  mFieldList.addField(SelectAllField{});
}

void SelectStatement::addSelectAllFields(const QueryEntity & entity)
{
  Q_ASSERT(!entity.isNull());

  mFieldList.addField( SelectAllField(entity) );
}

void SelectStatement::addField(const QString & fieldName, const FieldAlias & fieldAlias)
{
  Q_ASSERT(!fieldName.trimmed().isEmpty());

  mFieldList.addField(fieldName, fieldAlias);
}

// void SelectStatement::addField(const FieldName& fieldName, const QString& fieldAlias)
// {
//   Q_ASSERT(!fieldName.isNull());
//   Q_ASSERT(!fieldAlias.trimmed().isEmpty());
// 
//   mFieldList.addField(fieldName, fieldAlias);
// }

void SelectStatement::addField(const QueryField & field)
{
  mFieldList.addField(field);
}

void SelectStatement::addField(const QueryEntity & entity, const QString & fieldName, const FieldAlias & fieldAlias)
{
  Q_ASSERT(!entity.isNull());
  Q_ASSERT(!fieldName.trimmed().isEmpty());

  mFieldList.addField(entity, fieldName, fieldAlias);
}

void SelectStatement::joinEntity(const QueryEntity & entity, const JoinConstraintExpression& joinConstraintExpression)
{
  Q_ASSERT(!entity.isNull());
  Q_ASSERT(!joinConstraintExpression.isNull());

  mJoinClauseList.addClause(JoinOperator::Join, entity, joinConstraintExpression);
}

void SelectStatement::setFilterExpression(const FilterExpression & filter)
{
  Q_ASSERT(!filter.isNull());

  mFilter = filter;
}

void SelectStatement::clear()
{
  clearAttributesExceptEntity();
  clearEntity();
  mFilter.clear();
  mJoinClauseList.clear();
}

void SelectStatement::clearAttributesExceptEntity()
{
  mFieldList.clear();
}

void SelectStatement::clearEntity()
{
  mEntity.clear();
}

}} // namespace Mdt{ namespace QueryExpression{
