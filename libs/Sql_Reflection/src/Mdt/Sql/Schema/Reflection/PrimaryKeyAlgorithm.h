/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_SQL_REFLECTION_SCHEMA_REFLECTION_PRIMARY_KEY_ALGORITHM_H
#define MDT_SQL_REFLECTION_SCHEMA_REFLECTION_PRIMARY_KEY_ALGORITHM_H

#include "Mdt/Sql/Schema/AutoIncrementPrimaryKey.h"
#include "Mdt/Sql/Schema/PrimaryKey.h"
#include "Mdt/Sql/Schema/PrimaryKeyContainer.h"
#include "Mdt/Sql/Schema/FieldLength.h"
#include "Mdt/Sql/Schema/FieldTypeMap.h"
#include "Mdt/Reflection/FieldAlgorithm.h"
#include "Mdt/Reflection/AutoIncrementIdPrimaryKey.h"
#include "Mdt/Reflection/IdPrimaryKey.h"
#include "Mdt/Reflection/PrimaryKey.h"
#include "Mdt/Reflection/PrimaryKeyAlgorithm.h"
#include "Mdt/Reflection/TypeTraits/IsAutoIncrementIdPrimaryKey.h"
#include "Mdt/Reflection/TypeTraits/IsIdPrimaryKey.h"
#include "Mdt/Reflection/TypeTraits/IsPrimaryKey.h"
#include "Mdt/Reflection/TypeTraits/IsPrimaryKeyClass.h"
#include <QString>
#include <type_traits>

namespace Mdt{ namespace Sql{ namespace Schema{ namespace Reflection{

  /*! \brief Get a auto increment primary key from reflected one
   *
   * \pre \a Pk must be a auto increment id primary key for a reflected struct
   * \pre \a fieldTypeMap must be valid (must have a implementation)
   */
  template<typename Pk>
  AutoIncrementPrimaryKey autoIncrementPrimaryKeyFromReflected(const FieldTypeMap & fieldTypeMap)
  {
    static_assert( Mdt::Reflection::TypeTraits::IsAutoIncrementIdPrimaryKey<Pk>::value, "Pk must be a auto increment id primary key for a reflected struct" );
    Q_ASSERT(fieldTypeMap.isValid());

    AutoIncrementPrimaryKey pk;

    pk.setFieldName( QString::fromLatin1( Mdt::Reflection::fieldNameFromAutoIncrementIdPrimaryKeyField<Pk>() ) );
    pk.setFieldType( fieldTypeMap.fieldTypeFromQMetaType( Mdt::Reflection::qMetaTypeFromAutoIncrementIdPrimaryKeyField<Pk>(), FieldLength() ) );

    return pk;
  }

  /*! \brief Get a primary key from reflected id primary key
   *
   * \pre \a Pk must a id primary key for a reflected struct
   */
  template<typename Pk>
  PrimaryKey primaryKeyFromReflectedId()
  {
    static_assert( Mdt::Reflection::TypeTraits::IsIdPrimaryKey<Pk>::value, "Pk must be a id primary key for a reflected struct" );

    PrimaryKey pk;

    pk.setFieldNameList( Mdt::Reflection::fieldNameListFromPrimaryKey<Pk>() );
    Q_ASSERT(pk.fieldCount() == 1);

    return pk;
  }

  /*! \brief Get a primary key from reflected one
   *
   * \pre \a Pk must a primary key for a reflected struct
   */
  template<typename Pk>
  PrimaryKey primaryKeyFromReflected()
  {
    static_assert( Mdt::Reflection::TypeTraits::IsPrimaryKey<Pk>::value, "Pk must be a primary key for a reflected struct" );

    PrimaryKey pk;

    pk.setFieldNameList( Mdt::Reflection::fieldNameListFromPrimaryKey<Pk>() );
    Q_ASSERT(!pk.isNull());

    return pk;
  }

  /*! \internal Get a auto increment primary key from reflected one
   *
   * \pre \a fieldTypeMap must be valid (must have a implementation)
   */
  template<typename Pk>
  typename std::enable_if< Mdt::Reflection::TypeTraits::IsAutoIncrementIdPrimaryKey<Pk>::value , AutoIncrementPrimaryKey >::type
  primaryKeyFromReflectedImpl(const FieldTypeMap & fieldTypeMap)
  {
    Q_ASSERT(fieldTypeMap.isValid());
    return autoIncrementPrimaryKeyFromReflected<Pk>(fieldTypeMap);
  }

  /*! \internal Get a primary key from reflected id primary key
   */
  template<typename Pk>
  typename std::enable_if< Mdt::Reflection::TypeTraits::IsIdPrimaryKey<Pk>::value , PrimaryKey >::type
  primaryKeyFromReflectedImpl(const FieldTypeMap &)
  {
    return primaryKeyFromReflectedId<Pk>();
  }

  /*! \internal Get a primary key from reflected one
   */
  template<typename Pk>
  typename std::enable_if< Mdt::Reflection::TypeTraits::IsPrimaryKey<Pk>::value , PrimaryKey >::type
  primaryKeyFromReflectedImpl(const FieldTypeMap &)
  {
    return primaryKeyFromReflected<Pk>();
  }

  /*! \brief Get a primary key container from a reflected primary key
   *
   * \pre \a Pk must be primary key class for a reflected struct
   * \pre \a fieldTypeMap must be valid (must have a implementation)
   */
  template<typename Pk>
  PrimaryKeyContainer primaryKeyContainerFromReflected(const FieldTypeMap & fieldTypeMap)
  {
    static_assert( Mdt::Reflection::TypeTraits::IsPrimaryKeyClass<Pk>::value, "Pk must be a primary key class for a reflected struct" );
    Q_ASSERT(fieldTypeMap.isValid());

    PrimaryKeyContainer pkc;

    pkc.setPrimaryKey( primaryKeyFromReflectedImpl<Pk>(fieldTypeMap) );

    return pkc;
  }

}}}} // namespace Mdt{ namespace Sql{ namespace Schema{ namespace Reflection{


#endif // #ifndef MDT_SQL_REFLECTION_SCHEMA_REFLECTION_PRIMARY_KEY_ALGORITHM_H
