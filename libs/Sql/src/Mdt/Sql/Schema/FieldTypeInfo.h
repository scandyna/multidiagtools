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
#ifndef MDT_SQL_SCHEMA_FIELD_TYPE_INFO_H
#define MDT_SQL_SCHEMA_FIELD_TYPE_INFO_H

#include "FieldType.h"
#include <QString>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Field type info
   *
   * FieldTypeInfo contains a FieldType and its related name used in SQL DDL.
   *  It contains no flags.
   *  The main role of FieldTypeInfo is to be able to list
   *  available field types in a user interface.
   */
  class FieldTypeInfo
  {
   public:

    /*! \brief Create a field type info
     */
    FieldTypeInfo(FieldType fieldType, const QString & name)
     : pvType(fieldType), pvName(name) {}

    /*! \brief Get field type
     */
    FieldType type() const
    {
      return pvType;
    }

    /*! \brief Get field type name
     */
    QString name() const
    {
      return pvName;
    }

   private:

    FieldType pvType;
    QString pvName;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_FIELD_TYPE_INFO_H
