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
#ifndef MDT_SQL_SCHEMA_PRIMARY_KEY_CONTAINER_H
#define MDT_SQL_SCHEMA_PRIMARY_KEY_CONTAINER_H

#include "PrimaryKeyType.h"
#include "AutoIncrementPrimaryKey.h"
#include "PrimaryKey.h"
#include "FieldType.h"
#include <QStringList>
#include <boost/variant.hpp>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Contains one type of primary key
   *
   * To help defining SQL schema in most possible easy way,
   *  several primary key types exists.
   *
   * To store this variants of primary key in a unified way,
   *  for example in a Table, this container class helps.
   */
  class PrimaryKeyContainer
  {
   public:

    /*! \brief Construct a null primary key container
     */
    PrimaryKeyContainer() = default;

    /*! \brief Check if this container is null
     */
    bool isNull() const
    {
      return (mType == PrimaryKeyType::Unknown);
    }

    /*! \brief Set primary key
     */
    void setPrimaryKey(const AutoIncrementPrimaryKey & pk)
    {
      mPrimaryKey = pk;
      mType = PrimaryKeyType::AutoIncrementPrimaryKey;
    }

    /*! \brief Set \a primaryKey to this container
     */
    void setPrimaryKey(const PrimaryKey & pk)
    {
      mPrimaryKey = pk;
      mType = PrimaryKeyType::PrimaryKey;
    }

    /*! \brief Set primary key with list of fields
     */
    template<typename...Ts>
    void setPrimaryKey(const Ts & ...fieldList)
    {
      setPrimaryKey( PrimaryKey(fieldList...) );
    }

    /*! \brief Get field name
     *
     * If primary key is of type AutoIncrementPrimaryKey,
     *  its field name is returned,
     *  else a empty string is returned.
     */
    QString fieldName() const;

    /*! \brief Get a list of field names of the primary key hold by this container
     */
    QStringList fieldNameList() const;

    /*! \brief Get field type
     *
     * If this container holds a AutoIncrementPrimaryKey,
     *  its field type is returned,
     *  else UnknownType is returned.
     */
    FieldType fieldType() const;

    /*! \brief Get stored primary key type
     */
    PrimaryKeyType primaryKeyType() const
    {
      return mType;
    }

    /*! \brief Get primary key
     *
     * \pre Stored primary key must be of type AutoIncrementPrimaryKey
     */
    AutoIncrementPrimaryKey autoIncrementPrimaryKey() const;

    /*! \brief Get primary key
     *
     * \pre Stored primary key must be of type PrimaryKey
     */
    PrimaryKey primaryKey() const;

    /*! \brief Clear
     */
    void clear();

   private:

    boost::variant<PrimaryKey, AutoIncrementPrimaryKey> mPrimaryKey;
    PrimaryKeyType mType = PrimaryKeyType::Unknown;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_PRIMARY_KEY_CONTAINER_H
