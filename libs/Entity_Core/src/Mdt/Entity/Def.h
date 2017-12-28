/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_ENTITY_DEF_H
#define MDT_ENTITY_DEF_H

#include "FieldDef.h"
#include <QString>
#include <boost/preprocessor/if.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/variadic_seq_to_seq.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/to_array.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>
#include <boost/preprocessor/tuple/reverse.hpp>
#include <boost/preprocessor/tuple/pop_back.hpp>
#include <boost/preprocessor/array/size.hpp>
#include <boost/preprocessor/array/pop_back.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/adapted/struct/adapt_assoc_struct.hpp>
#include <boost/fusion/include/adapt_assoc_struct.hpp>

#define MDT_ENTITY_DEF_NAMESPACE_APPLY_OP(defTuple, op)                                   \
  BOOST_PP_IF(                                                                            \
    BOOST_PP_ARRAY_SIZE( BOOST_PP_ARRAY_POP_BACK( BOOST_PP_TUPLE_TO_ARRAY(defTuple) ) ),  \
    BOOST_PP_SEQ_FOR_EACH(                                                                \
      op, , BOOST_PP_TUPLE_TO_SEQ( BOOST_PP_TUPLE_POP_BACK(defTuple) )                    \
    ),                                                                                    \
  )

#define MDT_ENTITY_DEF_NAMESPACE_BEGIN_OP(r, unused, elem)  \
  namespace elem {

#define MDT_ENTITY_DEF_NAMESPACE_BEGIN(defTuple)  \
  MDT_ENTITY_DEF_NAMESPACE_APPLY_OP(defTuple, MDT_ENTITY_DEF_NAMESPACE_BEGIN_OP)

#define MDT_ENTITY_DEF_NAMESPACE_END_OP(r, unused, elem)  \
  }

#define MDT_ENTITY_DEF_NAMESPACE_END(defTuple)  \
  MDT_ENTITY_DEF_NAMESPACE_APPLY_OP(defTuple, MDT_ENTITY_DEF_NAMESPACE_END_OP)

#define MDT_ENTITY_DEF_NAMESPACE_REF_OP(r, unused, elem)  \
  elem::

#define MDT_ENTITY_DEF_NAMESPACE_REF(defTuple)  \
  MDT_ENTITY_DEF_NAMESPACE_APPLY_OP(defTuple, MDT_ENTITY_DEF_NAMESPACE_REF_OP)

#define MDT_ENTITY_DEF_MEMBER_FIELD(r, data, elem)  \
  MDT_ENTITY_FIELD_DEF(elem)                        \
  MDT_ENTITY_FIELD_DEF_INSTANCE(elem)

#define MDT_ENTITY_DEF_MEMBER_FIELD_LIST(...)                                 \
  BOOST_PP_SEQ_FOR_EACH(                                                      \
    MDT_ENTITY_DEF_MEMBER_FIELD, data, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)  \
  )

#define MDT_ENTITY_DEF_MEMBER(r, data, elem)  \
  ( MDT_ENTITY_FIELD_ELEM_NAME(elem) )

#define MDT_ENTITY_DEF_MEMBER_SEQ(...)                                  \
  BOOST_PP_SEQ_FOR_EACH(                                                \
    MDT_ENTITY_DEF_MEMBER, data, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)  \
  )

#define MDT_ENTITY_DATA_STRUCT_DEF_MEMBER_ASSOC(r, defName, elem) \
  ( MDT_ENTITY_FIELD_ELEM_NAME(elem) , defName::MDT_ENTITY_FIELD_ELEM_DEF_NAME(elem) )

#define MDT_ENTITY_DATA_STRUCT_DEF_MEMBER_ASSOC_SEQ(defName, ...)                           \
  BOOST_PP_SEQ_FOR_EACH(                                                                    \
    MDT_ENTITY_DATA_STRUCT_DEF_MEMBER_ASSOC, defName, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__) \
  )

#define MDT_ENTITY_DEF_ELEM_DATA_STRUCT_NAME(defTuple)  \
  BOOST_PP_TUPLE_ELEM( 0, BOOST_PP_TUPLE_REVERSE(defTuple) )

#define MDT_ENTITY_DEF_NAME(name) \
  BOOST_PP_CAT( name, Def )

#define MDT_ENTITY_NAME_STR(name) \
  BOOST_PP_STRINGIZE(name)

#define MDT_ENTITY_DEF_NAME_NS(defTuple, name)  \
  MDT_ENTITY_DEF_NAMESPACE_REF(defTuple) MDT_ENTITY_DEF_NAME(name)

#define MDT_ENTITY_DEF_ELEM_DATA_STRUCT_NAME_NS(defTuple) \
  MDT_ENTITY_DEF_NAMESPACE_REF(defTuple) MDT_ENTITY_DEF_ELEM_DATA_STRUCT_NAME(defTuple)

/*! \brief Create a entity definition class
 *
 * Starting with a data struct:
 * \code
 * struct PersonDataStruct
 * {
 *   qlonlong id = 0;
 *   QString firstName;
 *   QString remarks;
 * };
 * \endcode
 *
 * To make a entity of this struct, some meta data must be available,
 *  like the field name, max length, ...
 *  Such meta data can be generated with MDT_ENTITY_DEF() macro:
 * \code
 * MDT_ENTITY_DEF(
 *   (PersonDataStruct),
 *   Person,
 *   (id, FieldFlag::IsRequired | FieldFlag::IsUnique),
 *   (firstName, FieldMaxLength(250))
 *   (remarks)
 * )
 * \endcode
 *
 * Above code will expand to:
 * \code
 * struct PersonDef
 * {
 *   static const QString entityName()
 *   {
 *     return QStringLiteral("Person");
 *   }
 *
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
 *
 *   struct firstNameField
 *   {
 *     static const QString fieldName()
 *     {
 *       return QStringLiteral("firstName");
 *     }
 *
 *     static const Mdt::Entity::FieldAttributes fieldAttributes()
 *     {
 *       return Mdt::Entity::FieldAttributes(Mdt::Entity::FieldMaxLength(250));
 *     }
 *   };
 *
 *   struct remarksField
 *   {
 *     static const QString fieldName()
 *     {
 *       return QStringLiteral("remarks");
 *     }
 *
 *     static const Mdt::Entity::FieldAttributes fieldAttributes()
 *     {
 *       return Mdt::Entity::FieldAttributes();
 *     }
 *   };
 *
 *   idField id;
 *   firstNameField firstName;
 *   remarksField remarks;
 * };
 * \endcode
 *
 * Now examine the syntax to use the macro:
 * \code
 * MDT_ENTITY_DEF(
 *   (PersonDataStruct),
 *   Person,
 *   (id, FieldFlag::IsRequired | FieldFlag::IsUnique),
 *   (firstName, FieldMaxLength(250)),
 *   (remarks)
 * )
 * \endcode
 *
 * The first argument
 * \code
 *  (PersonDataStruct)
 * \endcode
 *  takes the data struct name.
 *  If PersonDataStruct was defined in a namespace
 *  called MyEntities, we have to provide it.
 *  Note that the macro must be called oustide
 *  any namespace.
 *  In our case, the first argument will become:
 * \code
 *  (MyEntities, PersonDataStruct)
 * \endcode
 * This strange syntax is due to some limitations
 *  of the preprocessor (in my knowlage).
 *  MDT_ENTITY_DEF() have to create the PersonDef
 *  in the MyEntities namespace.
 *  As far as I know, there is no way to parse ::
 *  in the preprocessor.
 *
 * The second argument is the entity name:
 * \code
 *  Person
 * \endcode
 *
 * Then, all fields are listed as a couple of tuples:
 * \code
 *  (id, FieldFlag::IsRequired | FieldFlag::IsUnique),
 *  (firstName, FieldMaxLength(250)),
 *  (remarks)
 * \endcode
 *
 * The first argument of a field tuple is its name,
 *  as it ws declared in PersonDataStruct.
 *  The following optional arguments are attributes.
 *  See the FieldAttributes class for details.
 */
#define MDT_ENTITY_DEF(defTuple, name, ...)                     \
  MDT_ENTITY_DEF_NAMESPACE_BEGIN(defTuple)                      \
  struct MDT_ENTITY_DEF_NAME(name)                              \
  {                                                             \
    constexpr MDT_ENTITY_DEF_NAME(name)() noexcept {}           \
                                                                \
    static const QString entityName()                           \
    {                                                           \
      return QStringLiteral( MDT_ENTITY_NAME_STR(name) );       \
    }                                                           \
                                                                \
    MDT_ENTITY_DEF_MEMBER_FIELD_LIST(__VA_ARGS__)               \
  };                                                            \
  MDT_ENTITY_DEF_NAMESPACE_END(defTuple)                        \
  BOOST_FUSION_ADAPT_STRUCT(                                    \
    MDT_ENTITY_DEF_NAME_NS(defTuple, name) ,                    \
    BOOST_PP_SEQ_ENUM( MDT_ENTITY_DEF_MEMBER_SEQ(__VA_ARGS__) ) \
  )                                                             \
  BOOST_FUSION_ADAPT_ASSOC_STRUCT(                              \
    MDT_ENTITY_DEF_ELEM_DATA_STRUCT_NAME_NS(defTuple),          \
    MDT_ENTITY_DATA_STRUCT_DEF_MEMBER_ASSOC_SEQ(                \
      MDT_ENTITY_DEF_NAME_NS(defTuple, name), __VA_ARGS__       \
    )                                                           \
  )

#endif // #ifndef MDT_ENTITY_DEF_H
