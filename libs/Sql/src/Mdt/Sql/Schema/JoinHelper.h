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
#ifndef MDT_SQL_SCHEMA_JOIN_HELPER_H
#define MDT_SQL_SCHEMA_JOIN_HELPER_H

#include "JoinClause.h"
#include <QString>

namespace Mdt{ namespace Sql{ namespace Schema{

  class Table;
  class ForeignKey;

  template<typename T>
  class TableTemplate;

  /*! \brief Helper class to build JoinClause from a existing schema
   */
  class JoinHelper
  {
   public:

    /*! \brief Rule to define which table will be the main in the join
     */
    enum MainTableRule
    {
      ParentIsMain, /*!< Foreign key's parent table becomes the main table in join */
      ChildIsMain   /*!< Foreign key's child table becomes the main table in join */
    };

    /*! \brief Get a join clause from foreign key
     */
    static JoinClause joinClauseFromForeignKey(const ForeignKey & fk, MainTableRule rule,
                                               const QString & mainTableAlias = QString(), const QString & tableToJoinAlias = QString());

    /*! \brief Get a join clause from tables
     *
     * Will first check if tableToJoin has a foreign key that refers to mainTable.
     *  If not found, it will be check if mainTable has a foreign key that refers to tableToJoin.
     *  If a foreign key was found, a join is built based on it and returned.
     */
    static JoinClause joinClauseFromTables(const Table & mainTable, const Table & tableToJoin,
                                           const QString & mainTableAlias = QString(), const QString & tableToJoinAlias = QString());

    /*! \brief Get a join clause from tables
     *
     * \sa joinClauseFromTables(const Table &, const Table &, const QString &, const QString &)
     */
    template<typename T, typename U>
    static JoinClause joinClauseFromTables(const TableTemplate<T> & mainTable, const TableTemplate<U> & tableToJoin,
                                           const QString & mainTableAlias = QString(), const QString & tableToJoinAlias = QString())
    {
      return joinClauseFromTables(mainTable.toTable(), tableToJoin.toTable(), mainTableAlias, tableToJoinAlias);
    }


   private:

    /*! \brief Add key to join clause
     */
    static void addKeysToJoinClause(JoinClause & join, const ForeignKey & fk, MainTableRule rule);
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_JOIN_HELPER_H
