/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "SelectFieldItem.h"

namespace Mdt{ namespace Sql{


SelectFieldItem::SelectFieldItem(const SelectTable & table, const FieldName & field, const QString & fieldAlias)
 : mTableName(table.aliasOrTableName()) ,
   mItem( SelectField(field.toString(), fieldAlias) )
{
  Q_ASSERT(!mTableName.isEmpty());
  Q_ASSERT(!field.isNull());
}

SelectFieldItem::SelectFieldItem(const FieldName & field, const QString & fieldAlias)
 : mItem( SelectField(field.toString(), fieldAlias) )
{
  Q_ASSERT(!field.isNull());
}

SelectFieldItem::SelectFieldItem(const SelectTable & table, const SelectAllField & allField)
 : mTableName(table.aliasOrTableName()),
   mItem( allField )
{
  Q_ASSERT(!mTableName.isEmpty());
}

SelectFieldItem::SelectFieldItem(const SelectAllField & allField)
 : mItem( allField )
{
}

SelectFieldItem::SelectFieldItem(const SelectFieldRawSql & sql)
 : mItem( sql )
{
  Q_ASSERT(!sql.sql.isEmpty());
}

}} // namespace Mdt{ namespace Sql{
