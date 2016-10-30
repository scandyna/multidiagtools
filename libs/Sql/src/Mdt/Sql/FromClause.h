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
     */
    void setTable(const SelectTable & table);

    /*! \brief Set join clause
     */
//     void setJoinClause(const JoinClause & clause);

    /*! \brief Set raw SQL string
     *
     * For clauses that are not handeld,
     *  a raw FROM clause can be passed.
     *
     * \param sql From clause that come after the FROM keyword
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
    const boost::variant<SelectTable, QString> & clause() const
    {
      return mClause;
    }

   private:

    boost::variant<SelectTable, QString> mClause;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_FROM_CLAUSE_H
