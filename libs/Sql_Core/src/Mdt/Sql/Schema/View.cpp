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
#include "Mdt/Sql/SelectField.h"

namespace Mdt{ namespace Sql{ namespace Schema{

void View::setName(const QString & name)
{
  mName = name;
}

void View::setSelectOperator(SelectOperator op)
{
  mSelectStatement.setSelectOperator(op);
}

void View::addField(const SelectTable & table, const FieldName & field, const QString & fieldAlias)
{
  Q_ASSERT(!table.isNull());
  Q_ASSERT(!field.isNull());

  mSelectStatement.addField(table, field, fieldAlias);
}

void View::addField(const FieldName & field, const QString & fieldAlias)
{
  Q_ASSERT(!field.isNull());

  mSelectStatement.addField(field, fieldAlias);
}

void View::addAllFields(const SelectTable & table)
{
  Q_ASSERT(!table.isNull());

  mSelectStatement.addAllFields(table);
}

void View::addAllFields()
{
  mSelectStatement.addAllFields();
}

void View::addRawSqlFieldExpression(const QString & sql)
{
  Q_ASSERT(!sql.isEmpty());

  mSelectStatement.addRawSqlFieldExpression(sql);
}

void View::setTable(const SelectTable & table)
{
  Q_ASSERT(!table.isNull());

  mSelectStatement.setTable(table);
}

void View::setFromTable(const SelectTable & table)
{
  Q_ASSERT(!table.isNull());

  mSelectStatement.setFromTable(table);
}

void View::joinTable(const SelectTable & table, const JoinConstraintExpression & expr)
{
  Q_ASSERT(!table.isNull());
  Q_ASSERT(!expr.isNull());
  Q_ASSERT(!mSelectStatement.fromClause().isNull());

  mSelectStatement.joinTable(table, expr);
}

void View::joinTable(const SelectTable & table)
{
  Q_ASSERT(!table.isNull());
  // Assertion that table or fromTable must have foreign key is done in JoinClause with a message explaining it
  Q_ASSERT(!mSelectStatement.fromClause().isNull());

  mSelectStatement.joinTable(table);
}

void View::joinTable(const SelectTable & table, const SelectTable & constraintOnTable)
{
  Q_ASSERT(!table.isNull());
  Q_ASSERT(!constraintOnTable.isNull());
  // Assertion that table or constraintOnTable must have foreign key is done in JoinClause with a message explaining it
  Q_ASSERT(!mSelectStatement.fromClause().isNull());

  mSelectStatement.joinTable(table, constraintOnTable);
}

void View::leftJoinTable(const SelectTable & table, const JoinConstraintExpression & expr)
{
  Q_ASSERT(!table.isNull());
  Q_ASSERT(!expr.isNull());
  Q_ASSERT(!mSelectStatement.fromClause().isNull());

  mSelectStatement.leftJoinTable(table, expr);
}

void View::leftJoinTable(const SelectTable & table)
{
  Q_ASSERT(!table.isNull());
  // Assertion that table or fromTable must have foreign key is done in JoinClause with a message explaining it
  Q_ASSERT(!mSelectStatement.fromClause().isNull());

  mSelectStatement.leftJoinTable(table);
}

void View::leftJoinTable(const SelectTable & table, const SelectTable & constraintOnTable)
{
  Q_ASSERT(!table.isNull());
  Q_ASSERT(!constraintOnTable.isNull());
  // Assertion that table or constraintOnTable must have foreign key is done in JoinClause with a message explaining it
  Q_ASSERT(!mSelectStatement.fromClause().isNull());

  mSelectStatement.leftJoinTable(table, constraintOnTable);
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{
