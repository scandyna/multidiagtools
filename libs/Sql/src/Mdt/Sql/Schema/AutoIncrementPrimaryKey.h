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
#ifndef MDT_SQL_SCHEMA_AUTO_INCREMENT_PRIMARY_KEY_H
#define MDT_SQL_SCHEMA_AUTO_INCREMENT_PRIMARY_KEY_H

#include "FieldType.h"
#include <QString>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Auto increment primary key
   */
  class AutoIncrementPrimaryKey
  {
   public:

    /*! \brief Set field name
     */
    void setFieldName(const QString & name)
    {
      pvFieldName = name;
    }

    /*! \brief Get field name
     */
    QString fieldName() const
    {
      return pvFieldName;
    }

    /*! \brief Get field type
     */
    FieldType fieldType() const
    {
      return FieldType::Integer;
    }

    /*! \brief Check if primary key is null
     */
    bool isNull() const
    {
      return pvFieldName.isNull();
    }

    /*! \brief Clear
     */
    void clear()
    {
      pvFieldName.clear();
    }

   private:

    QString pvFieldName;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_AUTO_INCREMENT_PRIMARY_KEY_H
