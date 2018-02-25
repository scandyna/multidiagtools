/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_ENTITY_FIELD_DEF_H
#define MDT_ENTITY_FIELD_DEF_H

#include "FieldAttributes.h"
#include "TypeTraits/EntityFieldDefTag.h"
#include <QString>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/to_array.hpp>
#include <boost/preprocessor/array/enum.hpp>
#include <boost/preprocessor/array/pop_front.hpp>

#if !BOOST_PP_VARIADICS
 #error "The macro MDT_ENTITY_FIELD_DEF() requires a compiler that supports preprocessor variadics"
#endif

#define MDT_ENTITY_FIELD_DEF_NAME(fieldName) \
  BOOST_PP_CAT(fieldName, Field)

#define MDT_ENTITY_FIELD_ELEM_NAME(elem) \
  BOOST_PP_TUPLE_ELEM(0, elem)

#define MDT_ENTITY_FIELD_ELEM_NAME_STR(elem) \
  BOOST_PP_STRINGIZE( MDT_ENTITY_FIELD_ELEM_NAME(elem) )

#define MDT_ENTITY_FIELD_ELEM_DEF_NAME(elem) \
  MDT_ENTITY_FIELD_DEF_NAME( MDT_ENTITY_FIELD_ELEM_NAME(elem) )

#define MDT_ENTITY_FIELD_ELEM_ATTRIBUTES_ARGS(elem) \
  BOOST_PP_ARRAY_ENUM( \
    BOOST_PP_ARRAY_POP_FRONT( \
      BOOST_PP_TUPLE_TO_ARRAY(elem) \
    ) \
  )

  /*! \brief Create a field definition class
   *
   * This is a helper macro for MDT_ENTITY_DEF() .
   *
   * Starting with a data struct:
   * \code
   * struct ClientDataStruct
   * {
   *   qlonlong id = 0;
   *   QString firstName;
   * };
   * \endcode
   *
   * To make a entity of this struct, some meta data must be available,
   *  like the field name, max length, ...
   *
   * Defining fields in the def struct:
   * \code
   * struct ClientDef
   * {
   *   MDT_ENTITY_FIELD_DEF( (id, FieldFlag::IsRequired | FieldFlag::IsUnique) )
   * };
   * \endcode
   *
   * will create fields as struct with meta data:
   * \code
   * struct ClientDef
   * {
   *   struct idField
   *   {
   *     static const QString fieldName()
   *     {
   *       return QStringLiteral("id");
   *     }
   *
   *     static const Mdt::Entity::FieldAttributes fieldAttributes()
   *     {
   *       return Mdt::Entity::FieldAttributes(Mdt::Entity::FieldFlag::IsRequired | Mdt::Entity::FieldFlag::IsUnique);
   *     }
   *   };
   * };
   * \endcode
   *
   * \sa MDT_ENTITY_DEF()
   *
   * \todo See if:
   *    static const QString fieldName()
   *   could be replaced with:
   *    constexpr auto fieldName()
   */
#define MDT_ENTITY_FIELD_DEF(fieldTuple)                                                          \
  struct MDT_ENTITY_FIELD_ELEM_DEF_NAME(fieldTuple) : Mdt::Entity::TypeTraits::EntityFieldDefTag  \
  {                                                                                               \
    constexpr MDT_ENTITY_FIELD_ELEM_DEF_NAME(fieldTuple)() noexcept {}                            \
                                                                                                  \
    static const QString fieldName()                                                              \
    {                                                                                             \
      return QStringLiteral( MDT_ENTITY_FIELD_ELEM_NAME_STR(fieldTuple) );                        \
    }                                                                                             \
                                                                                                  \
    static const Mdt::Entity::FieldAttributes fieldAttributes()                                   \
    {                                                                                             \
      using namespace Mdt::Entity;                                                                \
      return Mdt::Entity::FieldAttributes(MDT_ENTITY_FIELD_ELEM_ATTRIBUTES_ARGS(fieldTuple));     \
    }                                                                                             \
  };

#define MDT_ENTITY_FIELD_DEF_INSTANCE(fieldTuple) \
  MDT_ENTITY_FIELD_ELEM_DEF_NAME(fieldTuple) MDT_ENTITY_FIELD_ELEM_NAME(fieldTuple);

#endif // #ifndef MDT_ENTITY_FIELD_DEF_H
