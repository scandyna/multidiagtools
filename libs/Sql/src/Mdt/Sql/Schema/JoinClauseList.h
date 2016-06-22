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
#ifndef MDT_SQL_SCHEMA_JOIN_CLAUSE_LIST_H
#define MDT_SQL_SCHEMA_JOIN_CLAUSE_LIST_H

#include "JoinClause.h"
#include <QVector>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Hold a list of JoinClause
   */
  class JoinClauseList
  {
   public:

    /*! \brief STL-style const iterator
     */
    typedef QVector<JoinClause>::const_iterator const_iterator;

    /*! \brief Append a join clause
     */
    void append(const JoinClause & join)
    {
      pvJoinClauseList.append(join);
    }

    /*! \brief Get count of elements
     */
    int size() const
    {
      return pvJoinClauseList.size();
    }

    /*! \brief Check if list is empty
     */
    bool isEmpty() const
    {
      return pvJoinClauseList.isEmpty();
    }

    /*! \brief Get join clause at given index
     *
     * \pre index must be valid
     */
    const JoinClause & at(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < size());

      return pvJoinClauseList.at(index);
    }

    /*! \brief Get begin const iterator
     */
    const_iterator begin() const
    {
      return pvJoinClauseList.cbegin();
    }

    /*! \brief Get end const iterator
     */
    const_iterator end() const
    {
      return pvJoinClauseList.cend();
    }

    /*! \brief Clear
     */
    void clear()
    {
      pvJoinClauseList.clear();
    }

   private:

    QVector<JoinClause> pvJoinClauseList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_JOIN_CLAUSE_LIST_H
