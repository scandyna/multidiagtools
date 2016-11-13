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

// void SelectStatement::addField(const SelectTable & table, const SelectField & field)
// {
//   mFieldList.append(table.aliasOrTableName(), field);
// }

void SelectStatement::addField(const SelectTable & table, const FieldName & field, const QString & fieldAlias)
{
  Q_ASSERT(!table.isNull());
  Q_ASSERT(!field.isNull());

  mFieldList.addField(table, field, fieldAlias);
}

void SelectStatement::addField(const FieldName & field, const QString & fieldAlias)
{
  Q_ASSERT(!field.isNull());

  mFieldList.addField(field, fieldAlias);
}

void SelectStatement::addAllFields(const SelectTable & table)
{
  Q_ASSERT(!table.isNull());

  mFieldList.addAllFields(table);
}

void SelectStatement::addAllFields()
{
  mFieldList.addAllFields();
}

void SelectStatement::addRawSqlFieldExpression(const QString & sql)
{
  Q_ASSERT(!sql.isEmpty());

  mFieldList.addRawSqlFieldExpression(sql);
}

void SelectStatement::setFromTable(const SelectTable & table)
{
  Q_ASSERT(!table.isNull());

  mFromClause.setTable(table);
}

void SelectStatement::joinTable(const SelectTable & table, const JoinConstraintExpression & expr)
{
  Q_ASSERT(!table.isNull());
  Q_ASSERT(!expr.isNull());
  Q_ASSERT(!mFromClause.isNull());

  mFromClause.joinTableOn(JoinOperator::Join, table, expr);
}

void SelectStatement::joinTable(const SelectTable & table)
{
  Q_ASSERT(!table.isNull());
  // Assertion that table or fromTable must have foreign key is done in JoinClause with a message explaining it
  Q_ASSERT(!mFromClause.isNull());

  mFromClause.joinTableOn(JoinOperator::Join, table);
}

void SelectStatement::joinTable(const SelectTable & table, const SelectTable & constraintOnTable)
{
  Q_ASSERT(!table.isNull());
  Q_ASSERT(!constraintOnTable.isNull());
  // Assertion that table or constraintOnTable must have foreign key is done in JoinClause with a message explaining it
  Q_ASSERT(!mFromClause.isNull());

  mFromClause.joinTableOn(JoinOperator::Join, table, constraintOnTable);
}

void SelectStatement::leftJoinTable(const SelectTable & table, const JoinConstraintExpression & expr)
{
  Q_ASSERT(!table.isNull());
  Q_ASSERT(!expr.isNull());
  Q_ASSERT(!mFromClause.isNull());

  mFromClause.joinTableOn(JoinOperator::LeftJoin, table, expr);
}

void SelectStatement::leftJoinTable(const SelectTable & table)
{
  Q_ASSERT(!table.isNull());
  // Assertion that table or fromTable must have foreign key is done in JoinClause with a message explaining it
  Q_ASSERT(!mFromClause.isNull());

  mFromClause.joinTableOn(JoinOperator::LeftJoin, table);
}

void SelectStatement::leftJoinTable(const SelectTable & table, const SelectTable & constraintOnTable)
{
  Q_ASSERT(!table.isNull());
  Q_ASSERT(!constraintOnTable.isNull());
  // Assertion that table or constraintOnTable must have foreign key is done in JoinClause with a message explaining it
  Q_ASSERT(!mFromClause.isNull());

  mFromClause.joinTableOn(JoinOperator::LeftJoin, table, constraintOnTable);
}

void SelectStatement::setWhereExpression(const WhereExpression & expr)
{
  Q_ASSERT(!expr.isNull());

  mWhereExpression = expr;
}

}} // namespace Mdt{ namespace Sql{
