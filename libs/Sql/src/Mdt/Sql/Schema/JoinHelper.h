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
#include "JoinOperator.h"
#include <QString>

namespace Mdt{ namespace Sql{ namespace Schema{

  class Table;
  class ForeignKey;
  class View;
  class ViewTable;

  template<typename T>
  class TableTemplate;

  /*! \brief Helper class to build JoinClause from a existing schema
   */
  class JoinHelper
  {
   public:


    /*! \brief Get a join clause from foreign key
     */
//     static JoinClause joinClauseFromForeignKey(const ForeignKey & fk, MainTableRule rule,
//                                                const QString & mainTableAlias = QString(), const QString & tableToJoinAlias = QString());

    /*! \brief Get a join clause from foreign key
     *
     * 
     * \pre fk must have its parent table and child table defined
     * \pre fk must have at least 1 pair of key fields
     * \sa joinClauseFromTables(const Table &, const ViewTable &, const Table &, const ViewTable &, JoinOperator::Operator) .
     */
//     static JoinClause joinClauseFromForeignKey(const ForeignKey & fk, MainTableRule rule,
//                                                const ViewTable & mainViewTable, const ViewTable & viewTableToJoin,
//                                                JoinOperator::Operator joinOperator = JoinOperator::Join);

    /*! \brief Get a join clause from tables
     *
     * Will first check if tableToJoin has a foreign key that refers to mainTable.
     *  If not found, it will be check if mainTable has a foreign key that refers to tableToJoin.
     *  If a foreign key was found, a join is built based on it and returned.
     */
//     static JoinClause joinClauseFromTables(const Table & mainTable, const Table & tableToJoin,
//                                            const QString & mainTableAlias = QString(), const QString & tableToJoinAlias = QString());

    /*! \brief Get a join clause from tables
     *
     * Will first check if tableToJoin has a foreign key that refers to mainTable.
     *  If not found, it will be check if mainTable has a foreign key that refers to tableToJoin.
     *  If a foreign key was found, a join is built based on it and returned.
     *
     * \pre mainViewTable's table name must be the same as mainTable's table name
     * \pre viewTableToJoin's table name must be the same as tableToJoin's table name
     * \pre a foreign key must exist between mainTable and tableToJoin
     * \pre the foreign key must have its parent table and child table defined
     * \pre the foreign key must have at least 1 pair of key fields
     * \post If all preconditions are satisfied,
     *        the returned join has its main table set with requested mainTable,
     *        its table to join will be requested tableToJoin.
     */
    static JoinClause joinClauseFromTables(const Table & mainTable, const ViewTable & mainViewTable,
                                           const Table & tableToJoin, const ViewTable & viewTableToJoin,
                                           JoinOperator::Operator joinOperator = JoinOperator::Join);

    /*! \brief Get a join clause from tables
     *
     * \sa joinClauseFromTables(const Table &, const ViewTable &, const Table &, const ViewTable &, JoinOperator::Operator) .
     */
    template<typename T, typename U>
    static JoinClause joinClauseFromTables(const TableTemplate<T> & mainTable, ViewTable & mainViewTable,
                                           const TableTemplate<U> & tableToJoin, const ViewTable & viewTableToJoin,
                                           JoinOperator::Operator joinOperator = JoinOperator::Join)
    {
      return joinClauseFromTables(mainTable.toTable(), mainViewTable, tableToJoin.toTable(), viewTableToJoin, joinOperator);
    }

    /*! \brief Get a join clause from tables
     *
     * This overload takes view's table as main table to build the join.
     *
     * \pre mainTable and view's table must refer to the same table.
     * \sa joinClauseFromTables(const Table &, const ViewTable &, const Table &, const ViewTable &, JoinOperator::Operator) .
     */
    static JoinClause joinClauseFromTables(const Table & mainTable, const View & view,
                                           const Table & tableToJoin, const ViewTable & viewTableToJoin,
                                           JoinOperator::Operator joinOperator = JoinOperator::Join);

    /*! \brief Get a join clause from tables
     */
    template<typename T, typename U>
    static JoinClause joinClauseFromTables(const TableTemplate<T> & mainTable, const View & view,
                                           const TableTemplate<U> & tableToJoin, const ViewTable & viewTableToJoin,
                                           JoinOperator::Operator joinOperator = JoinOperator::Join)
    {
      return joinClauseFromTables(mainTable.toTable(), view, tableToJoin.toTable(), viewTableToJoin, joinOperator);
    }

    /*! \brief Get a join clause from tables
     *
     * \sa joinClauseFromTables(const Table &, const Table &, const QString &, const QString &)
     */
//     template<typename T, typename U>
//     static JoinClause joinClauseFromTables(const TableTemplate<T> & mainTable, const TableTemplate<U> & tableToJoin,
//                                            const QString & mainTableAlias = QString(), const QString & tableToJoinAlias = QString())
//     {
//       return joinClauseFromTables(mainTable.toTable(), tableToJoin.toTable(), mainTableAlias, tableToJoinAlias);
//     }


   private:

    /*! \brief Rule to define which table will be the main in the join
     */
    enum MainTableRule
    {
      ParentIsMain, /*!< Foreign key's parent table becomes the main table in join */
      ChildIsMain   /*!< Foreign key's child table becomes the main table in join */
    };

    /*! \brief Fill join clause
     */
    static void fillJoinClause(JoinClause & join, const ForeignKey & fk, MainTableRule rule,
                               const ViewTable & mainViewTable, const ViewTable & viewTableToJoin,
                               JoinOperator::Operator joinOperator);

    /*! \brief Add key to join clause
     */
    static void addKeysToJoinClause(JoinClause & join, const ForeignKey & fk, MainTableRule rule);
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_JOIN_HELPER_H
