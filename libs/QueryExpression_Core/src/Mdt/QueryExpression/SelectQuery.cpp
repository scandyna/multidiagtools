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
#include "SelectQuery.h"

namespace Mdt{ namespace QueryExpression{

void SelectQuery::setEntityName(const QString& name)
{
  Q_ASSERT(!EntityName(name).isNull());

  mEntity.setNameAndAlias(EntityName(name));
}

void SelectQuery::setEntityName(const EntityName& name, const QString & alias)
{
  Q_ASSERT(!name.isNull());
  Q_ASSERT(!alias.trimmed().isEmpty());

  mEntity.setNameAndAlias(name, alias);
}

void SelectQuery::setEntity(const SelectEntity & entity)
{
  Q_ASSERT(!entity.isNull());

  mEntity = entity;
}

void SelectQuery::addField(const QString & fieldName)
{
  Q_ASSERT(!FieldName(fieldName).isNull());

}

void SelectQuery::addField(const FieldName& fieldName, const QString& fieldAlias)
{
  Q_ASSERT(!fieldName.isNull());
  Q_ASSERT(!fieldAlias.trimmed().isEmpty());

}

void SelectQuery::addField(const SelectField & field)
{

}

void SelectQuery::addField(const SelectEntity& entity, const FieldName& fieldName, const QString& fieldAlias)
{
  Q_ASSERT(!entity.isNull());
  Q_ASSERT(!fieldName.isNull());

}

}} // namespace Mdt{ namespace QueryExpression{
