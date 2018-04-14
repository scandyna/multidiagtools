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
#include "JoinConstraintFieldPairListSqlTransform.h"
#include "JoinConstraintFieldPairList.h"
#include "JoinConstraintFieldPair.h"
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QStringBuilder>
#include <QLatin1String>
#include <QLatin1Char>

namespace Mdt{ namespace Sql{

QString JoinConstraintFieldPairListSqlTransform::getSql(const JoinConstraintFieldPairList & list, const QSqlDatabase & db)
{
  Q_ASSERT(!list.isEmpty());
  Q_ASSERT(db.isValid());

  QString sql;

  sql = getFieldPairSql(list.leftTable(), list.rightTable(), list.at(0), db.driver());
  for(int i = 1; i < list.size(); ++i){
    sql += QLatin1String(" AND ") % getFieldPairSql(list.leftTable(), list.rightTable(), list.at(i), db.driver());
  }

  return sql;
}

QString JoinConstraintFieldPairListSqlTransform::getFieldPairSql(const QString & leftTable, const QString & rightTable, const JoinConstraintFieldPair & pair, const QSqlDriver * const driver)
{
  Q_ASSERT(driver != nullptr);

  return driver->escapeIdentifier(leftTable, QSqlDriver::TableName)
      % QLatin1Char('.')
      % driver->escapeIdentifier(pair.leftField(), QSqlDriver::FieldName)
      % QLatin1Char('=')
      % driver->escapeIdentifier(rightTable, QSqlDriver::TableName)
      % QLatin1Char('.')
      % driver->escapeIdentifier(pair.rightField(), QSqlDriver::FieldName);
}

}} // namespace Mdt{ namespace Sql{
