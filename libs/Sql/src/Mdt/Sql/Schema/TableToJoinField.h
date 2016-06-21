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
#ifndef MDT_SQL_SCHEMA_TABLE_TO_JOIN_FIELD_H
#define MDT_SQL_SCHEMA_TABLE_TO_JOIN_FIELD_H

#include "AutoIncrementPrimaryKey.h"
#include "SingleFieldPrimaryKey.h"
#include "Field.h"
#include <QString>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Wrapper for compile time checking
   */
  class TableToJoinField
  {
   public:

    /*! \brief Constructor
     */
    explicit TableToJoinField(const AutoIncrementPrimaryKey & pk)
     : pvFieldName(pk.fieldName()) {}

    /*! \brief Constructor
     */
    explicit TableToJoinField(const SingleFieldPrimaryKey & pk)
     : pvFieldName(pk.fieldName()) {}

    /*! \brief Constructor
     */
    explicit TableToJoinField(const Field & field)
     : pvFieldName(field.name()) {}

    /*! \brief Constructor
     */
    explicit TableToJoinField(const QString & name)
     : pvFieldName(name) {}

    /*! \brief Get field name
     */
    QString fieldName() const
    {
      return pvFieldName;
    }

   private:

    QString pvFieldName;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_TABLE_TO_JOIN_FIELD_H
