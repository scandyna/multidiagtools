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
#include "FromClauseSqlTransform.h"
#include "FromClause.h"
#include "JoinClauseSqlTransform.h"
#include <QSqlDatabase>
#include <QStringBuilder>
#include <boost/variant.hpp>

namespace Mdt{ namespace Sql{

/*
 * FromClauseSqlTransformVisitor
 */
class FromClauseSqlTransformVisitor : boost::static_visitor<QString>
{
 public:

  FromClauseSqlTransformVisitor(const QSqlDatabase & db)
   : mDatabase(db)
  {
  }

  QString operator()(const boost::blank &) const
  {
    return QString();
  }

  QString operator()(const JoinClause & joinClause) const
  {
    QString sql;

    sql = QStringLiteral("FROM ") % JoinClauseSqlTransform::getSql(joinClause, mDatabase);

    return sql;
  }

  QString operator()(const QString & sql) const
  {
    return QStringLiteral("FROM ") % sql;
  }

 private:

  const QSqlDatabase & mDatabase;
};

/*
 * FromClauseSqlTransform implementation
 */

QString FromClauseSqlTransform::getSql(const FromClause & clause, const QSqlDatabase & db)
{
  Q_ASSERT(!clause.isNull());
  Q_ASSERT(db.isValid());

  return boost::apply_visitor( FromClauseSqlTransformVisitor(db) , clause.clause() );
}

}} // namespace Mdt{ namespace Sql{
