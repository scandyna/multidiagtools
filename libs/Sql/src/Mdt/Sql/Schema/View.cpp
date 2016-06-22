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
#include "View.h"
#include "SelectField.h"

namespace Mdt{ namespace Sql{ namespace Schema{

View::View()
 : pvSelectOperator(Select)
{
}

void View::setName(const QString & name)
{
  pvName = name;
}

void View::setSelectOperator(View::SelectOperator s)
{
  pvSelectOperator = s;
}

void View::setTable(const ViewTable& table)
{
  pvTable = table;
}

void View::addSelectField(const ViewTable& table, const Field& field, const QString& fieldAlias)
{
  pvSelectFieldList.append( table.aliasOrTableName(), SelectField(field.name(), fieldAlias) );
}

void View::addSelectField(const ViewTable& table, const FieldName& fieldName, const QString& fieldAlias)
{
  pvSelectFieldList.append( table.aliasOrTableName(), SelectField(fieldName.toString(), fieldAlias) );
}

void View::addSelectField(const ViewTable& table, const AutoIncrementPrimaryKey& pk, const QString& fieldAlias)
{
  pvSelectFieldList.append( table.aliasOrTableName(), SelectField(pk.fieldName(), fieldAlias) );
}

void View::addSelectField(const ViewTable& table, const SingleFieldPrimaryKey& pk, const QString& fieldAlias)
{
  pvSelectFieldList.append( table.aliasOrTableName(), SelectField(pk.fieldName(), fieldAlias) );
}

void View::addSelectAllFields(const ViewTable& table)
{
  pvSelectFieldList.append(table.aliasOrTableName(), SelectField("*") );
}

void View::addJoinClause(const JoinClause & join)
{
  pvJoinClauseList.append(join);
}

void View::clear()
{
  pvSelectOperator = Select;
  pvName.clear();
  pvTable.clear();
  pvSelectFieldList.clear();
  pvJoinClauseList.clear();
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{