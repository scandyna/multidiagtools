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
#ifndef MDT_SQL_SCHEMA_FIELD_TYPE_FLAGS_H
#define MDT_SQL_SCHEMA_FIELD_TYPE_FLAGS_H

#include <QFlags>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief A flag used by FieldTypeFlags
   */
  enum class FieldTypeFlag
  {
    NoFlag = 0,     /*!< No flag */
    IsUnsigned = 1  /*!< Unsigned flag */
  };
  Q_DECLARE_FLAGS(FieldTypeFlags, FieldTypeFlag)

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

Q_DECLARE_OPERATORS_FOR_FLAGS(Mdt::Sql::Schema::FieldTypeFlags)

#endif // #ifndef MDT_SQL_SCHEMA_FIELD_TYPE_FLAGS_H
