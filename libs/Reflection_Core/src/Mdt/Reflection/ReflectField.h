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
#ifndef MDT_REFLECTION_REFLECT_FIELD_H
#define MDT_REFLECTION_REFLECT_FIELD_H

#include "FieldAttributes.h"
#include "TypeTraits/FieldTag.h"
#include <boost/preprocessor/config/config.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/to_array.hpp>
#include <boost/preprocessor/array/enum.hpp>
#include <boost/preprocessor/array/pop_front.hpp>

/*! \internal Get the field from a field tuple
 *
 * \code
 * MDT_REFLECTION_STRUCT_FIELD_GET_FIELD(
 *   (id, FieldFlag::IsRequired, FieldFlag::IsUnique)
 * )
 * \endcode
 * will expand to:
 * \code
 * id
 * \endcode
 */
#define MDT_REFLECTION_STRUCT_FIELD_GET_FIELD(fieldTuple) \
  BOOST_PP_TUPLE_ELEM(0, fieldTuple)

/*! \internal Get the field attributes from a field tuple
 *
 * \code
 * MDT_REFLECTION_STRUCT_FIELD_GET_FIELD_ATTRIBUTES_ENUM(
 *   (id, FieldFlag::IsRequired, FieldFlag::IsUnique)
 * )
 * \endcode
 * will expand to:
 * \code
 * FieldFlag::IsRequired, FieldFlag::IsUnique
 * \endcode
 */
#define MDT_REFLECTION_STRUCT_FIELD_GET_FIELD_ATTRIBUTES_ENUM(fieldTuple) \
  BOOST_PP_ARRAY_ENUM(                                                    \
    BOOST_PP_ARRAY_POP_FRONT(                                             \
      BOOST_PP_TUPLE_TO_ARRAY(fieldTuple)                                 \
    )                                                                     \
  )

/*! \internal Get the field definition
 *
 * \code
 * MDT_REFLECTION_STRUCT_FIELD_DEF(
 *   PersonDef,
 *   (firstName, FieldFlag::IsRequired, FieldMaxLength(250))
 * )
 * \endcode
 * will expand to:
 * \code
 * struct firstName
 * {
 *   using struct_def = PersonDef;
 *
 *   static constexpr Mdt::Reflection::FieldAttributes fieldAttributes()
 *   {
 *     using namespace Mdt::Reflection;
 *     return Mdt::Reflection::FieldAttributes(FieldFlag::IsRequired,FieldMaxLength(250));
 *   }
 * };
 * \endcode
 */
#define MDT_REFLECTION_STRUCT_FIELD_DEF(structDef, fieldTuple)                                                      \
  struct MDT_REFLECTION_STRUCT_FIELD_GET_FIELD(fieldTuple) : Mdt::Reflection::TypeTraits::FieldTag                  \
  {                                                                                                                 \
    using struct_def = structDef;                                                                                   \
                                                                                                                    \
    static constexpr Mdt::Reflection::FieldAttributes fieldAttributes() noexcept                                    \
    {                                                                                                               \
      using namespace Mdt::Reflection;                                                                              \
      return Mdt::Reflection::FieldAttributes( MDT_REFLECTION_STRUCT_FIELD_GET_FIELD_ATTRIBUTES_ENUM(fieldTuple) ); \
    }                                                                                                               \
  };

#endif // #ifndef MDT_REFLECTION_REFLECT_FIELD_H
