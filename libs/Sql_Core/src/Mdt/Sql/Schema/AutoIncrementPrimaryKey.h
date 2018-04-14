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
#ifndef MDT_SQL_SCHEMA_AUTO_INCREMENT_PRIMARY_KEY_H
#define MDT_SQL_SCHEMA_AUTO_INCREMENT_PRIMARY_KEY_H

#include "FieldType.h"
#include "MdtSql_CoreExport.h"
#include <QString>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Auto increment primary key
   */
  class MDT_SQL_CORE_EXPORT AutoIncrementPrimaryKey
  {
   public:

    /*! \brief Construct a null auto increment primary key
     */
    AutoIncrementPrimaryKey() = default;

    /*! \brief Construct a auto increment primary key
     *
     * \pre \a fieldType must be a valid type for a auto increment primary key
     * \sa isValidFieldType()
     */
    explicit AutoIncrementPrimaryKey(const QString & fieldName, FieldType fieldType = FieldType::Integer)
     : pvFieldName(fieldName),
       mFieldType(fieldType)
    {
      Q_ASSERT(isValidFieldType(fieldType));
    }

    AutoIncrementPrimaryKey(const AutoIncrementPrimaryKey &) = default;
    AutoIncrementPrimaryKey & operator=(const AutoIncrementPrimaryKey &) = default;
    AutoIncrementPrimaryKey(AutoIncrementPrimaryKey &&) = default;
    AutoIncrementPrimaryKey & operator=(AutoIncrementPrimaryKey &&) = default;

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

    /*! \brief Set field type
     *
     * \pre \a fieldType must be a valid type for a auto increment primary key
     * \sa isValidFieldType()
     */
    void setFieldType(FieldType fieldType)
    {
      Q_ASSERT(isValidFieldType(fieldType));
      mFieldType = fieldType;
    }

    /*! \brief Get field type
     */
    FieldType fieldType() const
    {
      return mFieldType;
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

    /*! \brief Check if \a fieldType can be used in a auto increment primary key
     */
    static bool isValidFieldType(FieldType fieldType)
    {
      return ( (fieldType == FieldType::Smallint) || (fieldType == FieldType::Integer) || (fieldType == FieldType::Bigint) );
    }

   private:

    QString pvFieldName;
    FieldType mFieldType = FieldType::Integer;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_AUTO_INCREMENT_PRIMARY_KEY_H
