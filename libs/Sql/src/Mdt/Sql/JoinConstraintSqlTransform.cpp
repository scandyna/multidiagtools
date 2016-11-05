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
#include "JoinConstraintSqlTransform.h"
#include "JoinConstraint.h"
#include "JoinConstraintExpression.h"
#include "JoinConstraintFieldPairList.h"
#include "JoinConstraintFieldPairListSqlTransform.h"
#include <QSqlDatabase>
#include <QStringBuilder>
#include <boost/variant.hpp>

namespace Mdt{ namespace Sql{

/*
 * JoinConstraintSqlTransformVisitor
 */
class JoinConstraintSqlTransformVisitor : boost::static_visitor<QString>
{
 public:

  JoinConstraintSqlTransformVisitor(const QSqlDatabase & db)
   : mDatabase(db)
  {
  }

  QString operator()(const JoinConstraintExpression & expr) const
  {
    return QStringLiteral("ON ") % expr.toSql(mDatabase);
  }

  QString operator()(const JoinConstraintFieldPairList & fpl) const
  {
    return QStringLiteral("ON ") % JoinConstraintFieldPairListSqlTransform::getSql(fpl, mDatabase);
  }

 private:

  const QSqlDatabase & mDatabase;
};

/*
 * JoinConstraintSqlTransform implementation
 */

QString JoinConstraintSqlTransform::getSql(const JoinConstraint & jc, const QSqlDatabase & db)
{
  Q_ASSERT(!jc.isNull());
  Q_ASSERT(db.isValid());

  return boost::apply_visitor( JoinConstraintSqlTransformVisitor(db) , jc.constraint() );
}

}} // namespace Mdt{ namespace Sql{
