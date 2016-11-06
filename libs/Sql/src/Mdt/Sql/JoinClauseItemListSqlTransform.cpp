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
#include "JoinClauseItemListSqlTransform.h"
#include "JoinClauseItemSqlTransform.h"
#include "JoinClauseItemList.h"
#include <QSqlDatabase>
#include <QStringBuilder>

namespace Mdt{ namespace Sql{

QString JoinClauseItemListSqlTransform::getSql(const JoinClauseItemList & list, const QSqlDatabase & db)
{
  QString sql;
  Q_ASSERT(!list.isEmpty());
  Q_ASSERT(db.isValid());

  sql = JoinClauseItemSqlTransform::getSql(list.at(0), db);
  for(int i = 1; i < list.size(); ++i){
    sql += QStringLiteral("\n") % JoinClauseItemSqlTransform::getSql(list.at(i), db);
  }

  return sql;
}

}} // namespace Mdt{ namespace Sql{
