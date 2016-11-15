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
#ifndef MDT_SQL_FIELD_NAME_H
#define MDT_SQL_FIELD_NAME_H

#include "Schema/Field.h"
#include "Schema/AutoIncrementPrimaryKey.h"
// #include "Schema/SingleFieldPrimaryKey.h"
#include <QString>

namespace Mdt{ namespace Sql{

  /*! \brief Wrapper for compile time checking
   */
  class FieldName
  {
   public:

    /*! \breif Construct explicitly from a field name
     */
    explicit FieldName(const QString & name)
     : mFieldName(name) {}

    /*! \brief Construct from a field
     */
    FieldName(const Schema::Field & field)
     : mFieldName(field.name()) {}

    /*! \brief Construct from a auto increment primary key
     */
    FieldName(const Schema::AutoIncrementPrimaryKey & pk)
     : mFieldName(pk.fieldName()) {}

    /*! \brief Construct from a single field primary key
     */
//     FieldName(const Schema::SingleFieldPrimaryKey & pk)
//      : mFieldName(pk.fieldName()) {}

    /*! \brief Check if this field name is null
     */
    bool isNull() const
    {
      return mFieldName.isEmpty();
    }

    /*! \brief Get filed name as string
     */
    QString toString() const
    {
      return mFieldName;
    }

   private:

    QString mFieldName;
  };

}} // namespace Mdt{ namespace Sql{

#endif // MDT_SQL_FIELD_NAME_H
