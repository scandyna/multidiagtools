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
#ifndef MDT_SQL_SCHEMA_FOREIGN_TABLE_H
#define MDT_SQL_SCHEMA_FOREIGN_TABLE_H

#include "Table.h"
#include <QString>

namespace Mdt{ namespace Sql{ namespace Schema{

  template<typename T>
  class TableTemplate;

  /*! \brief Helper class to explicitly disting foreign table by foreign key declaration
   */
  class ForeignTable
  {
   public:

    /*! \brief Construct with a table name
     *
     * \pre tableName must not be empty
     */
    explicit ForeignTable(const QString & tableName)
     : mTableName(tableName)
    {
      Q_ASSERT(!mTableName.isEmpty());
    }

    /*! \brief Construct on base of a table
     *
     * \pre table must have a non empty table name
     */
    explicit ForeignTable(const Table & table)
     : mTableName(table.tableName())
    {
      Q_ASSERT(!mTableName.isEmpty());
    }

    /*! \brief Construct on base of a table
     *
     * \pre table must have a non empty table name
     */
    template<typename T>
    explicit ForeignTable(const TableTemplate<T> & table)
     : mTableName(table.tableName())
    {
      Q_ASSERT(!mTableName.isEmpty());
    }

    /*! \brief Get table name
     */
    QString tableName() const
    {
      return mTableName;
    }

   private:

    QString mTableName;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // MDT_SQL_SCHEMA_FOREIGN_TABLE_H
