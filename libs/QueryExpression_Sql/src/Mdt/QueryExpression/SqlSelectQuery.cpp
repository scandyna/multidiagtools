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
#include "SqlSelectQuery.h"

namespace Mdt{ namespace QueryExpression{

SqlSelectQuery::SqlSelectQuery(const QSqlDatabase& db)
 : AbstractSelectQuery(db)
{
  Q_ASSERT(db.isValid());
}

SqlSelectQuery::SqlSelectQuery(QObject* parent, const QSqlDatabase& db)
 : AbstractSelectQuery(parent, db)
{
  Q_ASSERT(db.isValid());
}

bool SqlSelectQuery::exec(const SelectQuery& query)
{

  return false;
}

}} // namespace Mdt{ namespace QueryExpression{
