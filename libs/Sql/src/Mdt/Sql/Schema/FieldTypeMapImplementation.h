/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_SQL_SCHEMA_FIELD_TYPE_MAP_IMPLEMENTATION_H
#define MDT_SQL_SCHEMA_FIELD_TYPE_MAP_IMPLEMENTATION_H

#include "FieldType.h"
#include "FieldLength.h"
#include "FieldTypeFlags.h"
#include "MdtSqlExport.h"
#include <QMetaType>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Provide a default implementation for FieldTypeMap
   */
  class MDT_SQL_EXPORT FieldTypeMapImplementation
  {
   public:

    /*! \brief Constructor
     */
    FieldTypeMapImplementation() = default;

    /*! \brief Destructor
     */
    virtual ~FieldTypeMapImplementation() = default;

    FieldTypeMapImplementation(const FieldTypeMapImplementation &) = delete;
    FieldTypeMapImplementation & operator=(const FieldTypeMapImplementation &) = delete;

    FieldTypeMapImplementation(FieldTypeMapImplementation &&) = delete;
    FieldTypeMapImplementation & operator=(FieldTypeMapImplementation &&) = delete;

    /*! \brief Get the field type that corresponds to \a qmetaType
     */
    FieldType fieldTypeFromQMetaType(QMetaType::Type qmetaType, FieldLength fieldLength) const;

    /*! \brief Get the field type flags that corresponds to \a qmetaType
     */
    FieldTypeFlags fieldTypeFlagsFromQMetaType(QMetaType::Type qmetaType) const;

    /*! \brief Get QMetaType from \a fieldType, \a fieldLength and \a fieldTypeFlags
     */
    QMetaType::Type qmetaTypeFromFieldType(FieldType fieldType, FieldLength fieldLength, FieldTypeFlags fieldTypeFlags) const;

   private:

    static FieldType fieldTypeFlagsFromQMetaTypeString(QMetaType::Type qmetaType, FieldLength fieldLength);
    static QMetaType::Type qmetaTypeFromFieldTypeString(FieldType fieldType, FieldLength fieldLength);
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_FIELD_TYPE_MAP_IMPLEMENTATION_H
