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
#include "SelectStatement.h"
#include "Schema/Field.h"
#include "Schema/AutoIncrementPrimaryKey.h"
#include "Schema/SingleFieldPrimaryKey.h"

namespace Mdt{ namespace Sql{

void SelectStatement::addField(const SelectTable & table, const SelectField & field)
{
  mFieldList.append(table.aliasOrTableName(), field);
}

void SelectStatement::addField(const SelectTable & table, const FieldName & field, const QString & fieldAlias)
{
  addField( table, SelectField(field.toString(), fieldAlias) );
}

void SelectStatement::addField(const SelectTable & table, const Schema::Field & field, const QString & fieldAlias)
{
  addField( table, SelectField(field.name(), fieldAlias) );
}

void SelectStatement::addField(const SelectTable & table, const Schema::AutoIncrementPrimaryKey & field, const QString & fieldAlias)
{
  addField( table, SelectField(field.fieldName(), fieldAlias) );
}

void SelectStatement::addField(const SelectTable & table, const Schema::SingleFieldPrimaryKey & field, const QString & fieldAlias)
{
  addField( table, SelectField(field.fieldName(), fieldAlias) );
}

}} // namespace Mdt{ namespace Sql{
