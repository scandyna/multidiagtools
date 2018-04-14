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
#ifndef MDT_SQL_SCHEMA_FIELD_TYPE_H
#define MDT_SQL_SCHEMA_FIELD_TYPE_H

#include <QtGlobal>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Field type
   *
   * Field types have been choosen by taking in account
   *  MySQL, MariaDB, PostgreSQL, SQLite and QMetaType .
   */
  enum class FieldType
  {
    UnknownType,  /*!< Unknown type (or no type) */
    Boolean,      /*!< Boolean type */
    Smallint,     /*!< Smallint type , 2 bytes (16 bit) storage */
    Integer,      /*!< Integer type , 4 bytes (32 bit) storage */
    Bigint,       /*!< Bigint type , 8 bytes (64 bit) storage */
    Float,        /*!< Float type */
    Double,       /*!< Double type */
    Char,         /*!< Char type */
    Varchar,      /*!< Variable length string (permit to fix a maximum length) */
    Text,         /*!< Variable length string */
    Blob,         /*!< Blob type */
    Date,         /*!< Date type */
    Time,         /*!< Time type */
    DateTime      /*!< DateTime type */
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

QT_BEGIN_NAMESPACE
Q_DECLARE_TYPEINFO(Mdt::Sql::Schema::FieldType, Q_PRIMITIVE_TYPE);
QT_END_NAMESPACE

#endif // #ifndef MDT_SQL_SCHEMA_FIELD_TYPE_H
