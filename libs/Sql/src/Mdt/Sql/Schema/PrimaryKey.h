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
#ifndef MDT_SQL_SCHEMA_PRIMARY_KEY_H
#define MDT_SQL_SCHEMA_PRIMARY_KEY_H

#include "Field.h"
#include <QStringList>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Primary key
   */
  class PrimaryKey
  {
   public:

    /*! \brief Add a field to this primary key
     */
    void addField(const Field & field)
    {
      pvFieldNameList.append(field.name());
    }

    /*! \brief Get field count
     */
    int fieldCount() const
    {
      return pvFieldNameList.count();
    }

    /*! \brief Get field name list
     */
    QStringList fieldNameList() const
    {
      return pvFieldNameList;
    }

    /*! \brief Check if primary key is null
     */
    bool isNull() const
    {
      return pvFieldNameList.isEmpty();
    }

    /*! \brief Clear
     */
    void clear()
    {
      pvFieldNameList.clear();
    }

   private:

    QStringList pvFieldNameList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_PRIMARY_KEY_H
