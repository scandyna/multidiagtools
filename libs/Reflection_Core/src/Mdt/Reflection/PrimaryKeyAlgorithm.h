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
#ifndef MDT_REFLECTION_PRIMARY_KEY_ALGORITHM_H
#define MDT_REFLECTION_PRIMARY_KEY_ALGORITHM_H

#include "PrimaryKey.h"
#include "FieldAlgorithm.h"
#include "TypeTraits/IsField.h"
#include "TypeTraits/IsAutoIncrementIdPrimaryKey.h"
#include "TypeTraits/IsIdPrimaryKey.h"
#include "TypeTraits/IsPrimaryKeyClass.h"
#include <QStringList>
#include <boost/mpl/for_each.hpp>

namespace Mdt{ namespace Reflection{

  /*! \brief Get the field QMetaType type from a auto increment id primary key
   *
   * \pre \a Pk must be a auto increment id primary key for a reflected struct
   */
  template<typename Pk>
  static constexpr QMetaType::Type qMetaTypeFromAutoIncrementIdPrimaryKeyField() noexcept
  {
    static_assert( Mdt::Reflection::TypeTraits::IsAutoIncrementIdPrimaryKey<Pk>::value, "Pk must be a auto increment id primary key for a reflected struct" );

    return qMetaTypeFromField<typename Pk::field>();
  }

  /*! \brief Get the field QMetaType type from a id primary key
   *
   * \pre \a Pk must be a id primary key for a reflected struct
   */
  template<typename Pk>
  static constexpr QMetaType::Type qMetaTypeFromIdPrimaryKeyField() noexcept
  {
    static_assert( Mdt::Reflection::TypeTraits::IsIdPrimaryKey<Pk>::value, "Pk must be a id primary key for a reflected struct" );

    return qMetaTypeFromField<typename Pk::field>();
  }

  /*! \brief Get the field name from a auto increment id primary key
   *
   * \pre \a Pk must be a auto increment id primary key for a reflected struct
   */
  template<typename Pk>
  static constexpr const char *fieldNameFromAutoIncrementIdPrimaryKeyField() noexcept
  {
    static_assert( Mdt::Reflection::TypeTraits::IsAutoIncrementIdPrimaryKey<Pk>::value, "Pk must be a auto increment id primary key for a reflected struct" );

    return fieldName<typename Pk::field>();
  }

  /*! \brief Get the field name from a id primary key
   *
   * \pre \a Pk must be a id primary key for a reflected struct
   */
  template<typename Pk>
  static constexpr const char *fieldNameFromIdPrimaryKeyField() noexcept
  {
    static_assert( Mdt::Reflection::TypeTraits::IsIdPrimaryKey<Pk>::value, "Pk must be a id primary key for a reflected struct" );

    return fieldName<typename Pk::field>();
  }

  /*! \brief Apply a functor for each field in a primary key
   *
   * \code
   * struct MyFunctor
   * {
   *   template<typename Field>
   *   void operator()(Field)
   *   {
   *   }
   * };
   * \endcode
   *
   * \pre \a PrimaryKey must be a primary key class
   */
  template<typename PrimaryKey, typename UnaryFunction>
  void forEachPrimaryKeyField(UnaryFunction f)
  {
    boost::mpl::for_each< typename PrimaryKey::field_list >(f);
  }

  namespace Impl{

    struct AddFieldNameToList
    {
      AddFieldNameToList(QStringList & list)
      : mFieldNameList(list)
      {
      }

      template<typename Field>
      void operator()(Field)
      {
        static_assert( TypeTraits::IsField<Field>::value , "Field must be a field defined in a struct definition associated with a reflected struct" );

        mFieldNameList << fieldName<Field>();
      }

    private:

      QStringList & mFieldNameList;
    };

  } // namespace Impl{

  /*! \brief Get a list of field names from a primary key
   *
   * \pre \a Pk must be primary key class for a reflected struct
   */
  template<typename Pk>
  QStringList fieldNameListFromPrimaryKey()
  {
    static_assert( Mdt::Reflection::TypeTraits::IsPrimaryKeyClass<Pk>::value, "Pk must be a primary key class for a reflected struct" );

    QStringList fieldNameList;
    Impl::AddFieldNameToList f(fieldNameList);

    forEachPrimaryKeyField<Pk>(f);

    return fieldNameList;
  }

}} // namespace Mdt{ namespace Reflection{

#endif // #ifndef MDT_REFLECTION_PRIMARY_KEY_ALGORITHM_H
