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
#ifndef MDT_SQL_SCHEMA_PRIMARY_KEY_CONTAINER_H
#define MDT_SQL_SCHEMA_PRIMARY_KEY_CONTAINER_H

#include "AutoIncrementPrimaryKey.h"
#include "SingleFieldPrimaryKey.h"
#include "PrimaryKey.h"
#include "FieldType.h"
#include <boost/variant.hpp>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Conatin one type of primary key
   *
   * To help defining SQL schema in most possible easy way,
   *  several primary key types exsits.
   *
   * To store this varaints of primary key in a unified way,
   *  for example in a Table, this container class helps.
   */
  class PrimaryKeyContainer
  {
   public:

    /*! \brief Default container
     */
    PrimaryKeyContainer()
     : pvType(PrimaryKeyType) {}

    /*! \brief Primary key type
     *
     * If a method exist to get infromation
     *  from stored primary key,
     *  its recommended to use it.
     *  If getting the stored primary key object itself,
     *  this type is usefull to know which type of object is stored.
     */
    enum Type
    {
      AutoIncrementPrimaryKeyType,  /*!< This container stores a AutoIncrementPrimaryKey */
      SingleFieldPrimaryKeyType,    /*!< This container stores a SingleFieldPrimaryKey */
      PrimaryKeyType                /*!< This container stores a PrimaryKey */
    };

    /*! \brief Set primary key
     */
    void setPrimaryKey(const AutoIncrementPrimaryKey & pk)
    {
      pvPrimaryKey = pk;
      pvType = AutoIncrementPrimaryKeyType;
    }

    /*! \brief Set primary key
     */
    void setPrimaryKey(const SingleFieldPrimaryKey & pk)
    {
      pvPrimaryKey = pk;
      pvType = SingleFieldPrimaryKeyType;
    }

    /*! \brief Set primary key
     */
    void setPrimaryKey(const PrimaryKey & pk)
    {
      pvPrimaryKey = pk;
      pvType = PrimaryKeyType;
    }

    /*! \brief Get field name
     *
     * If primary key is of type
     *  AutoIncrementPrimaryKey or SingleFieldPrimaryKey,
     *  its field name is returned,
     *  else a empty string is returned.
     */
    QString fieldName() const;

    /*! \brief Get field type
     *
     *  AutoIncrementPrimaryKey or SingleFieldPrimaryKey,
     *  its field type is returned,
     *  else UnknownType is returned.
     */
    FieldType fieldType() const;

    /*! \brief Get field length
     *
     *  AutoIncrementPrimaryKey or SingleFieldPrimaryKey,
     *  its field length is returned,
     *  else UnknownType is returned.
     */
    int fieldLength() const;

    /*! \brief Get stored primary key type
     */
    Type primaryKeyType() const
    {
      return pvType;
    }

   private:

    boost::variant<PrimaryKey, AutoIncrementPrimaryKey, SingleFieldPrimaryKey> pvPrimaryKey;
    Type pvType;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_PRIMARY_KEY_CONTAINER_H
