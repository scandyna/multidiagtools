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
#include "ViewSqlTransform.h"
#include "View.h"
#include "../SelectStatementSqlTransform.h"
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QLatin1String>
#include <QLatin1Char>
#include <QStringBuilder>

namespace Mdt{ namespace Sql{ namespace Schema{

QString ViewSqlTransform::getSqlToCreateView(const View & view, const QSqlDatabase & db)
{
  Q_ASSERT(!view.isNull());
  Q_ASSERT(db.isValid());
  const auto *driver = db.driver();
  Q_ASSERT(driver != nullptr);

  return QLatin1String("CREATE VIEW IF NOT EXISTS ") \
         % driver->escapeIdentifier(view.name(), QSqlDriver::TableName) \
         % QLatin1String(" AS\n") \
         % SelectStatementSqlTransform::getSql(view.selectStatement(), db) \
         % QLatin1Char(';');
}

QString ViewSqlTransform::getSqlToDropView(const View & view, const QSqlDatabase & db)
{
  Q_ASSERT(!view.name().isEmpty());
  Q_ASSERT(db.isValid());
  const auto *driver = db.driver();
  Q_ASSERT(driver != nullptr);

  return QLatin1String("DROP VIEW IF EXISTS ") \
         % driver->escapeIdentifier(view.name(), QSqlDriver::TableName) \
         % QLatin1Char(';');
}


}}} // namespace Mdt{ namespace Sql{ namespace Schema{
