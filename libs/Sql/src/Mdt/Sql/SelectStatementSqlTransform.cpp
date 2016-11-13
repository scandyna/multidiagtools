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
#include "SelectStatementSqlTransform.h"
#include "SelectStatement.h"
#include "SelectFieldListSqlTransform.h"
#include "FromClauseSqlTransform.h"
#include "WhereExpression.h"
#include <QSqlDatabase>
#include <QLatin1Char>
#include <QLatin1String>
#include <QStringBuilder>

namespace Mdt{ namespace Sql{

QString SelectStatementSqlTransform::getSql(const SelectStatement & stm, const QSqlDatabase & db)
{
  Q_ASSERT(!stm.fieldList().isEmpty());
  Q_ASSERT(db.isValid());

  QString sql;

  sql = getSelectOperatorString(stm.selectOperator()) \
      % QLatin1Char('\n') \
      % SelectFieldListSqlTransform::getSql(stm.fieldList(), db) \
      % QLatin1Char('\n') \
      % FromClauseSqlTransform::getSql(stm.fromClause(), db);
  if(!stm.whereExpression().isNull()){
    sql += QLatin1String("\nWHERE ") % stm.whereExpression().toSql(db);
  }

  return sql;
}

QString SelectStatementSqlTransform::getSelectOperatorString(SelectOperator op)
{
  switch(op){
    case SelectOperator::Select:
      return QLatin1String("SELECT");
    case SelectOperator::SelectDistinct:
      return QLatin1String("SELECT DISTINCT");
  }
  return QString();
}

}} // namespace Mdt{ namespace Sql{
