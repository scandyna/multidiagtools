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
#ifndef MDT_REFLECTION_STRUCT_ALGORITHM_H
#define MDT_REFLECTION_STRUCT_ALGORITHM_H

#include "TypeTraits/IsField.h"
#include "TypeTraits/IsStructDef.h"
#include <QString>
#include <boost/fusion/include/size.hpp>
#include <boost/fusion/include/as_map.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/mpl/for_each.hpp>

namespace Mdt{ namespace Reflection{

  /*! \brief Get the name given to a reflected struct from a struct def
   *
   * \pre \a StructDef must be a struct definition assiocated with a reflected struct
   * \sa nameFromStructDefQString()
   */
  template<typename StructDef>
  static constexpr const char* nameFromStructDef() noexcept
  {
    static_assert( TypeTraits::IsStructDef<StructDef>::value, "StructDef must be a struct definition assiocated with a reflected struct" );

    return StructDef::name_();
  }

  /*! \brief Get the name given to a reflected struct from a struct def
   *
   * \sa nameFromStructDef()
   */
  template<typename StructDef>
  static const QString nameFromStructDefQString()
  {
    // Note: the name given to the reflected struct cannot contain any non ASCII characters (will not compile)
    return QString::fromLatin1( nameFromStructDef<StructDef>() );
  }

  /*! \brief Get the count of reflected fields of a reflected struct
   *
   * \pre \a Strcut must have been reflected with MDT_REFLECT_STRUCT()
   */
  template<typename Struct>
  static constexpr int reflectedFieldCountFromStruct() noexcept
  {
    return boost::fusion::result_of::size<Struct>::type::value;
  }

  /*! \brief Get the count of reflected fields of a reflected struct from struct def
   *
   * \pre \a StructDef must be a struct definition assiocated with a reflected struct
   */
  template<typename StructDef>
  static constexpr int reflectedFieldCountFromStructDef() noexcept
  {
    static_assert( TypeTraits::IsStructDef<StructDef>::value,
                   "StructDef must be a struct definition assiocated with a reflected struct" );

    return boost::fusion::result_of::size<typename StructDef::reflected_struct>::type::value;
  }

  /*! \brief Get the name given to a reflected struct from a field
   *
   * \pre \a Field must be a field defined in a struct definition associated with a reflected struct
   * \sa nameFromFieldQString()
   */
  template<typename Field>
  static constexpr const char* nameFromField() noexcept
  {
    static_assert( TypeTraits::IsField<Field>::value , "Field must be a field defined in a struct definition associated with a reflected struct" );

    return nameFromStructDef<typename Field::struct_def>();
  }

  /*! \brief Get the name given to a reflected struct from a field
   *
   * \sa nameFromField()
   */
  template<typename Field>
  static const QString nameFromFieldQString()
  {
    // Note: the name given to the reflected struct cannot contain any non ASCII characters (will not compile)
    return QString::fromLatin1( nameFromField<Field>() );
  }

  /*! \brief Iterate over each element on a const reflected struct
   *
   * \a f is a functor like:
   * \code
   * struct MyFunctor
   * {
   *   template<typename T>
   *   void operator()(const T & value) const
   *   {
   *   }
   * };
   * \endcode
   *
   * Example of call:
   * \code
   * MyFunctor f;
   * PersonDataStruct person{25, "Name 25"};
   *
   * Mdt::Reflection::forEachValueInConstStruct(person, f);
   * \endcode
   *
   * \pre \a Strcut must have been reflected with MDT_REFLECT_STRUCT()
   */
  template<typename Struct, typename F>
  void forEachValueInConstStruct(const Struct & s, F f)
  {
    boost::fusion::for_each(s, f);
  }

  /*! \brief Iterate over each element on a reflected struct
   *
   * \a f is a functor like:
   * \code
   * struct MyFunctor
   * {
   *   template<typename T>
   *   void operator()(T & value) const
   *   {
   *   }
   * };
   * \endcode
   *
   * \note The call operator of the functor must be const.
   *   This is required by boost::proto::for_each()
   *   which is internally used.
   *
   * Example of call:
   * \code
   * MyFunctor f;
   * PersonDataStruct person;
   *
   * Mdt::Reflection::forEachValueInStruct(person, f);
   * \endcode
   *
   * \pre \a Strcut must have been reflected with MDT_REFLECT_STRUCT()
   */
  template<typename Struct, typename F>
  void forEachValueInStruct(Struct & s, F f)
  {
    boost::fusion::for_each(s, f);
  }

  /*! \brief Iterate over each element on a reflected struct
   *
   * \a f is a functor like:
   * \code
   * struct MyFunctor
   * {
   *   template<typename FieldValuePair>
   *   void operator()(const FieldValuePair & p) const
   *   {
   *   }
   * };
   * \endcode
   *
   * \a FieldValuePair will be a Boost Fusion pair.
   *   FieldValuePair::first_type will be a Field in the StructDef assiocated to \a Struct .
   *   FieldValuePair::second_type will be the value type.
   *   The value is available with p.second .
   *
   * \pre \a Strcut must have been reflected with MDT_REFLECT_STRUCT()
   */
  template<typename Struct, typename F>
  void forEachFieldValuePairInStruct(const Struct & s, F f)
  {
    // Note: this does not copy s
    const auto map = boost::fusion::as_map(s);
    boost::fusion::for_each(map, f);
  }

  /*! \brief Iterate over each field in a struct def
   *
   * \a f is a functor like:
   * \code
   * struct MyFunctor
   * {
   *   template<typename Field>
   *   void operator()(const Field) const
   *   {
   *   }
   * };
   * \endcode
   *
   * \pre \a StructDef must be a struct definition assiocated with a reflected struct
   */
  template<typename StructDef, typename F>
  void forEachFieldInStructDef(F f)
  {
    static_assert( TypeTraits::IsStructDef<StructDef>::value,
                   "StructDef must be a struct definition assiocated with a reflected struct" );

    boost::mpl::for_each< typename StructDef::field_list >(f);
  }

}} // namespace Mdt{ namespace Reflection{


#endif // #ifndef MDT_REFLECTION_STRUCT_ALGORITHM_H
