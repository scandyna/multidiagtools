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
#ifndef MDT_SQL_SCHEMA_SINGLE_FIELD_PRIMARY_KEY_H
#define MDT_SQL_SCHEMA_SINGLE_FIELD_PRIMARY_KEY_H

#include "FieldType.h"
#include "Collation.h"
#include <QString>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Single field primary key
   */
  class SingleFieldPrimaryKey
  {
   public:

    /*! \brief Create a null primary key
     */
    SingleFieldPrimaryKey()
    : pvType(FieldType::UnknownType),
      pvLength(-1)
    {
    }

    /*! \brief Set field type
     */
    void setFieldType(FieldType t)
    {
      pvType = t;
    }

    /*! \brief Get field type
     */
    FieldType fieldType() const
    {
      return pvType;
    }

    /*! \brief Set field name
     */
    void setFieldName(const QString & name)
    {
      pvName = name;
    }

    /*! \brief Get field name
     */
    QString fieldName() const
    {
      return pvName;
    }

    /*! \brief Set field length
     *
     * \sa fieldLength()
     */
    void setFieldLength(int length)
    {
      pvLength = length;
    }

    /*! \brief Get field length
     *
     * A value < 0 means that length is not set.
     */
    int fieldLength() const
    {
      return pvLength;
    }

    /*! \brief Set collation
     */
    void setCollation(const Collation & collation)
    {
      pvCollation = collation;
    }

    /*! \brief Set case sensitivity
     *
     * Case sensitivity will update collation
     *
     * By default, the primary key is case sensitive
     */
    void setCaseSensitive(bool cs)
    {
      pvCollation.setCaseSensitive(cs);
    }

    /*! \brief Get collation
     */
    Collation collation() const
    {
      return pvCollation;
    }

    /*! \brief Check if field is null
     *
     * Field is null at least if one of the mandatory attribute is not set.
     *  Mandatory attributes are:
     *  - fieldType \sa setFieldType() , fieldType()
     *  - fieldName \sa setFieldName() , fieldName()
     */
    bool isNull() const
    {
      return ( (pvType == FieldType::UnknownType) || pvName.isEmpty());
    }

    /*! \brief Clear
     */
    void clear()
    {
      pvType = FieldType::UnknownType;
      pvName.clear();
      pvLength = -1;
      pvCollation.clear();
    }

   private:

    FieldType pvType;
    int pvLength;
    QString pvName;
    Collation pvCollation;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_SINGLE_FIELD_PRIMARY_KEY_H
