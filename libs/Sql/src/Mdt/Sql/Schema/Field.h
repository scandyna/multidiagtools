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
#ifndef MDT_SQL_SCHEMA_FIELD_H
#define MDT_SQL_SCHEMA_FIELD_H

#include "FieldType.h"
#include "Collation.h"
#include <QString>
#include <QVariant>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief SQL schema field
   *
   * NOTE
   * \todo Add unique constraint
   * NOTE
   */
  class Field final
  {
   public:

    /*! \brief Create a null field
     */
    Field()
    : pvType(FieldType::UnknownType),
      pvIsRequired(false),
      pvLength(-1)
    {
    }

    /*! \brief Set field type
     */
    void setType(FieldType t)
    {
      pvType = t;
    }

    /*! \brief Get field type
     */
    FieldType type() const
    {
      return pvType;
    }

    /*! \brief Set field name
     */
    void setName(const QString & name)
    {
      pvName = name;
    }

    /*! \brief Get field name
     */
    QString name() const
    {
      return pvName;
    }

    /*! \brief Set field required
     */
    void setRequired(bool r)
    {
      pvIsRequired = r;
    }

    /*! \brief Check if field is required
     */
    bool isRequired() const
    {
      return pvIsRequired;
    }

    /*! \brief Set default value
     */
    void setDefaultValue(const QVariant & v)
    {
      pvDefaultValue = v;
    }

    /*! \brief Get default value
     */
    QVariant defaultValue() const
    {
      return pvDefaultValue;
    }

    /*! \brief Set length
     *
     * \sa length()
     */
    void setLength(int length)
    {
      pvLength = length;
    }

    /*! \brief Get length
     *
     * A value < 0 means that length is not set.
     */
    int length() const
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
     *  - type \sa setType() , type()
     *  - name \sa setName() , name()
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
      pvIsRequired = false;
      pvDefaultValue.clear();
      pvLength = -1;
      pvCollation.clear();
    }


   private:

    FieldType pvType;
    bool pvIsRequired;
    int pvLength;
    QString pvName;
    QVariant pvDefaultValue;
    Collation pvCollation;
  };

}}} //namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifnef MDT_SQL_SCHEMA_FIELD_H
