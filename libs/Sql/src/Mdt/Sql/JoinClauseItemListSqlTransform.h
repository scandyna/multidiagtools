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
#ifndef MDT_SQL_JOIN_CLAUSE_ITEM_LIST_SQL_TRANSFORM_H
#define MDT_SQL_JOIN_CLAUSE_ITEM_LIST_SQL_TRANSFORM_H

#include <QString>

class QSqlDatabase;

namespace Mdt{ namespace Sql{

  class JoinClauseItemList;

  /*! \brief Transform a JoinClauseItemList to its SQL representation
   */
  class JoinClauseItemListSqlTransform
  {
   public:

    /*! \brief Get SQL string of list
     *
     * \pre list must not be empty
     * \pre db must be valid (a driver must be loaded)
     */
    static QString getSql(const JoinClauseItemList & list, const QSqlDatabase & db);
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_JOIN_CLAUSE_ITEM_LIST_SQL_TRANSFORM_H
