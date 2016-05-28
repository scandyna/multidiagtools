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
#ifndef MDT_SQL_SCHEMA_FIELD_TYPE_NAME_H
#define MDT_SQL_SCHEMA_FIELD_TYPE_NAME_H

#include "FieldType.h"
#include <QString>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Mapping between a FieldType and its name
   */
  class FieldTypeName
  {
   public:

    /*! \brief Get field type name from type
     */
    static QString nameFromType(FieldType ft)
    {
      return pvNames[static_cast<int>(ft)];
    }

    /*! \brief Get field type from name
     */
    static FieldType typeFromName(const QString & name);

   private:

    static const char *pvNames[];
  };

  const char *FieldTypeName::pvNames[] = {
    "",
    "BOOLEAN",
    "INTEGER",
    "FLOAT",
    "DOUBLE",
    "VARCHAR",
    "DATE",
    "TIME",
    "DATETIME"
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_FIELD_TYPE_NAME_H
