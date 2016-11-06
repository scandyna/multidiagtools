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
#ifndef MDT_SQL_FROM_CLAUSE_H
#define MDT_SQL_FROM_CLAUSE_H

#include "SelectTable.h"
#include "JoinClause.h"
#include <QString>
#include <boost/variant.hpp>

namespace Mdt{ namespace Sql{

  /*! \brief From clause part of a SelectStatement
   *
   * FromClause is a union that can store
   *  a single SelectTable ,
   *  a JoinClause or
   *  a raw SQL string.
   */
  class FromClause
  {
   public:

    /*! \brief Set table
     *
     * If a raw SQL string was set,
     *  it will be cleared.
     *
     * \pre No joined table must allready been set.
     * \pre table must not be null.
     */
    void setTable(const SelectTable & table);

    /*! \brief Join a table
     *
     * Join table with expr as constraint
     *
     * \pre table must not be null
     * \pre expr must not be null
     * \pre From table must allready been set (see setTable()).
     */
    void joinTableOn(JoinOperator op, const SelectTable & table, const JoinConstraintExpression & expr);

    /*! \brief Join a table automatically
     *
     * Will fetch table's foreign key list
     *  and fromTable's foreign key list about relation
     *  and also generate the apropriate join constraint.
     *
     * \pre table must not be null
     * \pre table or fromTable must have a relation defined by their foreign key
     * \pre From table must allready been set (see setTable()).
     */
    void joinTableOn(JoinOperator op, const SelectTable & table);

    /*! \brief Join a table automatically
     *
     * Will fetch table's foreign key list
     *  and constraintOnTable's foreign key list about relation
     *  and also generate the apropriate join constraint.
     *
     * \pre table must not be null
     * \pre constraintOnTable must not be null
     * \pre table or constraintOnTable must have a relation defined by their foreign key
     * \pre From table must allready been set (see setTable()).
     */
    void joinTableOn(JoinOperator op, const SelectTable & table, const SelectTable & constraintOnTable);

    /*! \brief Set raw SQL string
     *
     * For clauses that are not handeld,
     *  a raw FROM clause can be passed.
     *
     * Setting a raw SQL string will clear table
     *  and joined tables allready set.
     *
     * \param sql From clause that comes after the FROM keyword
     *             (FROM keyword must not be part of sql string)
     */
    void setSqlString(const QString & sql);

    /*! \brief Check if this join clause is null
     */
    bool isNull() const;

    /*! \internal Access internal clause
     *
     * This function is used by transforms and unit tests
     */
    const auto & clause() const
    {
      return mClause;
    }

   private:

    boost::variant<boost::blank, JoinClause, QString> mClause;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_FROM_CLAUSE_H
