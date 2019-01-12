/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#include "MdtSql_CoreExport.h"
#include <QString>
#include <QVariant>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief SQL schema field
   */
  class MDT_SQL_CORE_EXPORT Field final
  {
   public:

    /*! \brief Create a null field
     */
    Field()
    : mType (FieldType::UnknownType),
      mIsUnsigned(false),
      mIsRequired (false),
      mIsUnique (false),
      mLength (-1)
    {
    }

    /*! \brief Set field type
     */
    void setType(FieldType t)
    {
      mType = t;
    }

    /*! \brief Get field type
     */
    FieldType type() const
    {
      return mType;
    }

    /*! \brief Set field unsigned
     *
     * \note This has only sense for integral types
     * \warning Some DBMS do not support unsigned integers
     */
    void setUnsigned(bool u)
    {
      mIsUnsigned = u;
    }

    /*! \brief Check if this field is unsigned
     *
     * \note This has only sense for integral types
     */
    bool isUnsigned() const noexcept
    {
      return mIsUnsigned;
    }

    /*! \brief Set field name
     */
    void setName(const QString & name)
    {
      mName = name;
    }

    /*! \brief Get field name
     */
    QString name() const
    {
      return mName;
    }

    /*! \brief Set field required
     */
    void setRequired(bool r)
    {
      mIsRequired = r;
    }

    /*! \brief Check if field is required
     */
    bool isRequired() const
    {
      return mIsRequired;
    }

    /*! \brief Set field unique
     */
    void setUnique(bool u)
    {
      mIsUnique = u;
    }

    /*! \brief Check if field is unique
     */
    bool isUnique() const
    {
      return mIsUnique;
    }

    /*! \brief Set default value
     */
    void setDefaultValue(const QVariant & v)
    {
        mDefaultValue = v;
    }

    /*! \brief Get default value
     */
    QVariant defaultValue() const
    {
      return mDefaultValue;
    }

    /*! \brief Set length
     *
     * \sa length()
     */
    void setLength(int length)
    {
        mLength = length;
    }

    /*! \brief Get length
     *
     * A value < 0 means that length is not set.
     */
    int length() const
    {
      return mLength;
    }

    /*! \brief Set collation
     */
    void setCollation(const Collation & collation)
    {
      mCollation = collation;
    }

    /*! \brief Set case sensitivity
     *
     * Case sensitivity will update collation
     * \sa Collation
     */
    void setCaseSensitive(bool cs)
    {
      mCollation.setCaseSensitive(cs);
    }

    /*! \brief Get collation
     */
    Collation collation() const
    {
      return mCollation;
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
      return ( ( mType == FieldType::UnknownType) || mName.isEmpty());
    }

    /*! \brief Clear
     */
    void clear()
    {
      mType = FieldType::UnknownType;
      mIsUnsigned = false;
      mName.clear();
      mIsRequired = false;
      mIsUnique = false;
      mDefaultValue.clear();
      mLength = -1;
      mCollation.clear();
    }


   private:

    FieldType mType;
    bool mIsUnsigned;
    bool mIsRequired;
    bool mIsUnique;
    int mLength;
    QString mName;
    QVariant mDefaultValue;
    Collation mCollation;
  };

}}} //namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifnef MDT_SQL_SCHEMA_FIELD_H
