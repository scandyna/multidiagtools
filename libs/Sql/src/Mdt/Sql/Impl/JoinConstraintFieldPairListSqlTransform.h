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
#ifndef MDT_SQL_IMPL_JOIN_CONSTRAINT_FIELD_PAIR_LIST_SQL_TRANSFORM_H
#define MDT_SQL_IMPL_JOIN_CONSTRAINT_FIELD_PAIR_LIST_SQL_TRANSFORM_H

#include <QString>

class QSqlDatabase;
class QSqlDriver;

namespace Mdt{ namespace Sql{ namespace Impl{

  class JoinConstraintFieldPairList;
  class JoinConstraintFieldPair;

  /*! \brief Transform a list of field pairs to SQL
   */
  class JoinConstraintFieldPairListSqlTransform
  {
   public:

    /*! \brief Get SQL string of list
     *
     * \pre list must not be empty
     * \pre db must be valid (have a driver loaded)
     */
    static QString getSql(const JoinConstraintFieldPairList & list, const QSqlDatabase & db);

   private:

    static QString getFieldPairSql(const QString & leftTable, const QString & rightTable, const JoinConstraintFieldPair & pair, const QSqlDriver * const driver);
  };

}}} // namespace Mdt{ namespace Sql{ namespace Impl{

#endif // #ifndef MDT_SQL_IMPL_JOIN_CONSTRAINT_FIELD_PAIR_LIST_SQL_TRANSFORM_H
