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
#ifndef MDT_SQL_SCHEMA_JOIN_KEY_LIST_H
#define MDT_SQL_SCHEMA_JOIN_KEY_LIST_H

#include "JoinKey.h"
#include <QVector>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief List of JoinKey
   */
  class JoinKeyList
  {
   public:

    /*! \brief Add a key
     */
    void append(const JoinKey & key)
    {
      pvKeyList.append(key);
    }

    /*! \brief Get element at index
     *
     * \pre index must be valid
     */
    const JoinKey & at(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < size());

      return pvKeyList.at(index);
    }

    /*! \brief Get count of elements
     */
    int size() const
    {
      return pvKeyList.size();
    }

    /*! \brief Check if field list is empty
     */
    bool isEmpty() const
    {
      return pvKeyList.empty();
    }

    /*! \brief Clear
     */
    void clear()
    {
      pvKeyList.clear();
    }

   private:

    QVector<JoinKey> pvKeyList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_JOIN_KEY_LIST_H
