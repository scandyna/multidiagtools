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
#ifndef MDT_SQL_SCHEMA_TABLE_NAME_H
#define MDT_SQL_SCHEMA_TABLE_NAME_H

#include <QString>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Wrapper for compile time checking
   */
  class TableName
  {
   public:

    /*! \breif Constructor
     */
    explicit TableName(const QString & name)
     : pvTableName(name) {}

    /*! \brief Get table name as string
     */
    QString toString() const
    {
      return pvTableName;
    }

   private:

    QString pvTableName;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // MDT_SQL_SCHEMA_TABLE_NAME_H
