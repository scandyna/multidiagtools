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
#ifndef MDT_SQL_SCHEMA_FOREIGN_FIELD_H
#define MDT_SQL_SCHEMA_FOREIGN_FIELD_H

#include "Field.h"
#include "AutoIncrementPrimaryKey.h"
#include <QString>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Wrapper to be explicit when declaring foreign keys
   */
  class ForeignField
  {
   public:

    /*! \brief Construct from a field name
     *
     * \pre fieldName must not be empty
     */
    explicit ForeignField(const QString & fieldName)
     : mFieldName(fieldName)
    {
      Q_ASSERT(!mFieldName.isEmpty());
    }

    /*! \brief Construct from a field
     *
     * \pre field's name must not be empty
     */
    explicit ForeignField(const Field & field)
     : mFieldName(field.name())
    {
      Q_ASSERT(!mFieldName.isEmpty());
    }

    /*! \brief Construct from a AutoIncrementPrimaryKey
     *
     * \pre pk's field name must not be empty
     */
    explicit ForeignField(const AutoIncrementPrimaryKey & pk)
     : mFieldName(pk.fieldName())
    {
      Q_ASSERT(!mFieldName.isEmpty());
    }

    /*! \brief Get field name
     */
    QString fieldName() const
    {
      return mFieldName;
    }

   private:

    QString mFieldName;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_FOREIGN_FIELD_H
