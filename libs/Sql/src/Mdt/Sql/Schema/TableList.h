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
#ifndef MDT_SQL_SCHEMA_TABLE_LIST_H
#define MDT_SQL_SCHEMA_TABLE_LIST_H

#include "Table.h"
#include <QVector>

namespace Mdt{ namespace Sql{ namespace Schema{

  template<typename T>
  class TableTemplate;

  /*! \brief Holds a list of Table
   */
  class TableList
  {
   public:

    /*! \brief Append a table
     */
    void append(const Table & t)
    {
      pvTableList.append(t);
    }

    /*! \brief Append a table
     */
    template<typename T>
    void append(const TableTemplate<T> & t)
    {
      pvTableList.append(t.toTable());
    }

    /*! \brief Get count of elements
     */
    int size() const
    {
      return pvTableList.size();
    }

    /*! \brief Check if table list is empty
     */
    bool isEmpty() const
    {
      return pvTableList.isEmpty();
    }

    /*! \brief Get table at given index
     *
     * \pre index must be valid
     */
    const Table & at(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < size());

      return pvTableList.at(index);
    }

    /*! \brief Clear
     */
    void clear()
    {
      pvTableList.clear();
    }

   private:

    QVector<Table> pvTableList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // ifndef MDT_SQL_SCHEMA_TABLE_LIST_H
