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
#ifndef MDT_SQL_COPIER_FIELD_TYPE_MAP_CHECK
#define MDT_SQL_COPIER_FIELD_TYPE_MAP_CHECK

#include "mdtSqlFieldType.h"
#include <QMetaType>

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Utility class to check field type compatibility
   */
  class FieldTypeMapCheck
  {
   public:

    /*! \brief Check if data of source type can be copied to database field of destination field
     */
    static bool canCopy(QMetaType::Type sourceType, const mdtSqlFieldType & detinationType);
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_FIELD_TYPE_MAP_CHECK
