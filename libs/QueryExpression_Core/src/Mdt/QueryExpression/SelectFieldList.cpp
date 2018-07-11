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
#include "SelectFieldList.h"

namespace Mdt{ namespace QueryExpression{

// void SelectFieldList::addField(const SelectField& field)
// {
//   mList.push_back(field);
// }

void SelectFieldList::addField(const SelectAllField& field)
{
  mList.emplace_back(field);
}

void SelectFieldList::addField(const SelectField& field)
{
  mList.push_back(field);
}

void SelectFieldList::addField(const FieldName& fieldName, const QString& fieldAlias)
{
  Q_ASSERT(!fieldName.isNull());

  mList.emplace_back(fieldName, fieldAlias);
}

void SelectFieldList::addField(const SelectEntity& entity, const FieldName& fieldName, const QString& fieldAlias)
{
  Q_ASSERT(!entity.isNull());
  Q_ASSERT(!fieldName.isNull());

  mList.emplace_back(entity, fieldName, fieldAlias);
}

void SelectFieldList::clear()
{
  mList.clear();
}

}} // namespace Mdt{ namespace QueryExpression{
