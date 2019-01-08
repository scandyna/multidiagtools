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
#ifndef MDT_REFLECTION_REFLECT_STRUCT_H
#define MDT_REFLECTION_REFLECT_STRUCT_H

#include "ReflectField.h"
#include "TypeTraits/StructDefTag.h"
#include <boost/preprocessor/config/config.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/reverse.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>
#include <boost/preprocessor/tuple/pop_back.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/pop_back.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/transform.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/fusion/include/adapt_assoc_struct.hpp>
#include <boost/mpl/vector.hpp>

#if !BOOST_PP_VARIADICS
 #error "The macro MDT_REFLECT_STRUCT() requires a compiler that supports preprocessor variadics"
#endif

#define MDT_REFLECTION_STRUCT_NAMESPACE_APPLY_OP(structTuple, op)                       \
  BOOST_PP_IF(                                                                          \
    BOOST_PP_SEQ_SIZE( BOOST_PP_SEQ_POP_BACK( BOOST_PP_TUPLE_TO_SEQ( structTuple ) ) ), \
    BOOST_PP_SEQ_FOR_EACH(                                                              \
      op, ~, BOOST_PP_TUPLE_TO_SEQ( BOOST_PP_TUPLE_POP_BACK(structTuple) )              \
    ),                                                                                  \
  )

#define MDT_REFLECTION_STRUCT_NAMESPACE_BEGIN_OP(r, unused, elem) \
  namespace elem {

#define MDT_REFLECTION_STRUCT_NAMESPACE_END_OP(r, unused, elem) \
  }

#define MDT_REFLECTION_STRUCT_NAMESPACE_REF_OP(r, unused, elem) \
  elem::

/*! \internal Begin a namespace from a struct tuple
 *
 * \code
 * MDT_REFLECTION_STRUCT_NAMESPACE_BEGIN(
 *   (A,B,MyStruct)
 * )
 * \endcode
 * will expand to:
 * \code
 * namespace A{ namespace B{
 * \endcode
 */
#define MDT_REFLECTION_STRUCT_NAMESPACE_BEGIN(structTuple) \
  MDT_REFLECTION_STRUCT_NAMESPACE_APPLY_OP(structTuple, MDT_REFLECTION_STRUCT_NAMESPACE_BEGIN_OP)

/*! \internal End a namespace from a struct tuple
 *
 * \code
 * MDT_REFLECTION_STRUCT_NAMESPACE_END(
 *   (A,B,MyStruct)
 * )
 * \endcode
 * will expand to:
 * \code
 * }}
 * \endcode
 */
#define MDT_REFLECTION_STRUCT_NAMESPACE_END(structTuple) \
  MDT_REFLECTION_STRUCT_NAMESPACE_APPLY_OP(structTuple, MDT_REFLECTION_STRUCT_NAMESPACE_END_OP)

/*! \internal Reference a namespace from a struct tuple
 *
 * \code
 * MDT_REFLECTION_STRUCT_NAMESPACE_REF(
 *   (A,B,MyStruct)
 * )
 * \endcode
 * will expand to:
 * \code
 * A::B::
 * \endcode
 */
#define MDT_REFLECTION_STRUCT_NAMESPACE_REF(structTuple) \
  MDT_REFLECTION_STRUCT_NAMESPACE_APPLY_OP(structTuple, MDT_REFLECTION_STRUCT_NAMESPACE_REF_OP)

/*! \internal Get the sutrct def from name
 *
 * \code
 * MDT_REFLECTION_STRUCT_GET_STRUCT_DEF_NAME( Person )
 * \endcode
 * will expand to:
 * \code
 * PersonDef
 * \endcode
 */
#define MDT_REFLECTION_STRUCT_GET_STRUCT_DEF_NAME(name) \
  BOOST_PP_CAT( name, Def )

/*! \internal Get the sutrct def from name
 *
 * \code
 * MDT_REFLECTION_STRUCT_GET_STRUCT_DEF_NAMESPACE_NAME(
 *  (A, PersonDataStruct),
 *  Person
 * )
 * \endcode
 * will expand to:
 * \code
 * PersonDef
 * \endcode
 */
#define MDT_REFLECTION_STRUCT_GET_STRUCT_DEF_NAMESPACE_NAME(structTuple, name) \
  MDT_REFLECTION_STRUCT_NAMESPACE_REF(structTuple) MDT_REFLECTION_STRUCT_GET_STRUCT_DEF_NAME(name)

/*! \internal Get the name as string
 *
 * \code
 * MDT_REFLECTION_STRUCT_GET_NAME_STR( Person )
 * \endcode
 * will expand to:
 * \code
 * "Person"
 * \endcode
 */
#define MDT_REFLECTION_STRUCT_GET_NAME_STR(name) \
  BOOST_PP_STRINGIZE(name)

/*! \internal Get the (reflected) struct name from a struct tuple
 *
 * \code
 * MDT_REFLECTION_STRUCT_GET_STRUCT_NAME( (A,PersonDataStruct) )
 * \endcode
 * will expand to:
 * \code
 * PersonDataStruct
 * \endcode
 */
#define MDT_REFLECTION_STRUCT_GET_STRUCT_NAME(structTuple)     \
  BOOST_PP_TUPLE_ELEM( 0, BOOST_PP_TUPLE_REVERSE(structTuple) )

/*! \internal Get the (reflected) struct name and namespace from a struct tuple
 *
 * \code
 * MDT_REFLECTION_STRUCT_GET_STRUCT_NAMESPACE_NAME(
 *   (A,PersonDataStruct)
 * )
 * \endcode
 * will expand to:
 * \code
 * A::PersonDataStruct
 * \endcode
 */
#define MDT_REFLECTION_STRUCT_GET_STRUCT_NAMESPACE_NAME(structTuple) \
  MDT_REFLECTION_STRUCT_NAMESPACE_REF(structTuple) MDT_REFLECTION_STRUCT_GET_STRUCT_NAME(structTuple)


#define MDT_REFLECTION_STRUCT_FIELD_GET_FIELD_OP(s, data, fieldTuple) \
  MDT_REFLECTION_STRUCT_FIELD_GET_FIELD(fieldTuple)

/*! \internal Transform a field tuple sequence to a field sequence
 *
 * \code
 * MDT_REFLECTION_STRUCT_FIELD_TUPLE_SEQ_TO_FIELD_SEQ(
 *   (id, FieldFlag::IsRequired, FieldFlag::IsUnique)
 *   (firstName, FieldMaxLength(250))
 *   (remarks)
 * )
 * \endcode
 * will expand to:
 * \code
 * (id)
 * (firstName)
 * (remarks)
 * \endcode
 */
#define MDT_REFLECTION_STRUCT_FIELD_TUPLE_SEQ_TO_FIELD_SEQ(fieldTupleSeq) \
  BOOST_PP_SEQ_TRANSFORM( MDT_REFLECTION_STRUCT_FIELD_GET_FIELD_OP, _, fieldTupleSeq )

/*! \internal Transform a field tuple argument list to a field enum
 *
 * \code
 * MDT_REFLECTION_STRUCT_FIELD_TUPLE_ARG_LIST_TO_FIELD_ENUM(
 *   (id, FieldFlag::IsRequired, FieldFlag::IsUnique),
 *   (firstName, FieldMaxLength(250)),
 *   (remarks)
 * )
 * \endcode
 * will expand to:
 * \code
 * id,firstName,remarks
 * \endcode
 */
#define MDT_REFLECTION_STRUCT_FIELD_TUPLE_ARG_LIST_TO_FIELD_ENUM(...) \
  BOOST_PP_SEQ_ENUM(                                                  \
    MDT_REFLECTION_STRUCT_FIELD_TUPLE_SEQ_TO_FIELD_SEQ(               \
      BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)                           \
    )                                                                 \
  )


#define MDT_REFLECTION_STRUCT_FIELD_DEF_LIST_OP(r, structDef, fieldTuple) \
  MDT_REFLECTION_STRUCT_FIELD_DEF(structDef, fieldTuple)

/*! \internal
 *
 * \code
 * MDT_REFLECTION_STRUCT_FIELD_DEF_LIST(
 *   PersonDef,
 *   (id),
 *   (firstName, FieldMaxLength(250))
 * )
 * \endcode
 * will expand to:
 * \code
 * MDT_REFLECTION_FIELD_DEF( PersonDef, (id) )
 * MDT_REFLECTION_FIELD_DEF( PersonDef, (firstName, FieldMaxLength(250)) )
 * \endcode
 */
#define MDT_REFLECTION_STRUCT_FIELD_DEF_LIST(structDef, ...)  \
  BOOST_PP_SEQ_FOR_EACH(                                      \
    MDT_REFLECTION_STRUCT_FIELD_DEF_LIST_OP,                  \
    structDef,                                                \
    BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)                     \
  )                                                           \

/*! \internal
 *
 * \code
 * MDT_REFLECTION_STRUCT_FIELD_ADAPT_ASSOC_LIST_OP(
 *   PersonDef,
 *   (firstName, FieldMaxLength(250))
 * )
 * \endcode
 * will expand to:
 * \code
 * firstName,PersonDef::firstName
 * \endcode
 */
#define MDT_REFLECTION_STRUCT_FIELD_ADAPT_ASSOC_LIST_OP(s, structDef, fieldTuple) \
  MDT_REFLECTION_STRUCT_FIELD_GET_FIELD(fieldTuple), structDef::MDT_REFLECTION_STRUCT_FIELD_GET_FIELD(fieldTuple)

/*! \internal
 *
 * \code
 * MDT_REFLECTION_STRUCT_FIELD_ADAPT_ASSOC_LIST(
 *   (A, PersonDataStruct),
 *   Person,
 *   (id),
 *   (firstName, FieldMaxLength(250))
 * )
 * \endcode
 * will expand to:
 * \code
 * (id,A::PersonDef::id)
 * (firstName,A::PersonDef::firstName)
 * \endcode
 */
#define MDT_REFLECTION_STRUCT_FIELD_ADAPT_ASSOC_LIST(structTuple, name, ...)  \
  BOOST_PP_SEQ_TRANSFORM(                                                     \
    MDT_REFLECTION_STRUCT_FIELD_ADAPT_ASSOC_LIST_OP,                          \
    MDT_REFLECTION_STRUCT_GET_STRUCT_DEF_NAMESPACE_NAME(structTuple, name),   \
    BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)                                     \
  )


/*! \brief Generate some meta data to reflect a struct
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
 * To have access to some informations about this struct,
 *  some meta data must be available,
 *  like the field name, max length, ...
 *  Such meta data can be generated with MDT_REFLECT_STRUCT() macro:
 * \code
 * MDT_REFLECT_STRUCT(
 *   (PersonDataStruct),
 *   Person,
 *   (id, FieldFlag::IsRequired, FieldFlag::IsUnique),
 *   (firstName, FieldMaxLength(250)),
 *   (remarks)
 * )
 * \endcode
 *
 * Above code will expand to:
 * \code
 * struct PersonDef
 * {
 *   using reflected_struct = PersonDataStruct;
 *
 *   static constexpr const char *name_()
 *   {
 *     return "Person";
 *   }
 *
 *   struct id
 *   {
 *     using struct_def = PersonDef;
 *
 *     static constexpr Mdt::Reflection::FieldAttributes fieldAttributes()
 *     {
 *       using namespace Mdt::Reflection;
 *       return Mdt::Reflection::FieldAttributes(FieldFlag::IsRequired, FieldFlag::IsUnique);
 *     }
 *   };
 *
 *   struct firstName
 *   {
 *     using struct_def = PersonDef;
 *
 *     static constexpr Mdt::Reflection::FieldAttributes fieldAttributes()
 *     {
 *       using namespace Mdt::Reflection;
 *       return Mdt::Reflection::FieldAttributes(FieldMaxLength(250));
 *     }
 *   };
 *
 *   struct remarks
 *   {
 *     using struct_def = PersonDef;
 *
 *     static constexpr Mdt::Reflection::FieldAttributes fieldAttributes()
 *     {
 *       using namespace Mdt::Reflection;
 *       return Mdt::Reflection::FieldAttributes();
 *     }
 *   };
 *
 *   using field_list = boost::mpl::vector<id, firstName, remarks>;
 * };
 *
 * BOOST_FUSION_ADAPT_ASSOC_STRUCT(
 *   PersonDataStruct,
 *   (id, PersonDef::id)
 *   (firstName, PersonDef::firstName)
 *   (remarks, PersonDef::remarks)
 * )
 * \endcode
 *
 * Now examine the syntax to use the macro:
 * \code
 * MDT_REFLECT_STRUCT(
 *   (PersonDataStruct),
 *   Person,
 *   (id, FieldFlag::IsRequired, FieldFlag::IsUnique),
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
 *  If %PersonDataStruct was defined in a namespace
 *  called MyEntities, we have to provide it.
 *  Note that the macro must be called oustide any namespace.
 *  In our case, the first argument will become:
 * \code
 *  (MyEntities, PersonDataStruct)
 * \endcode
 * This strange syntax is due to some limitations
 *  of the preprocessor (in my knowlage).
 *  MDT_REFLECT_STRUCT() have to create the PersonDef
 *  in the MyEntities namespace.
 *  As far as I know, there is no way to parse :: in the preprocessor.
 *
 * The second argument is the name we want to give:
 * \code
 *  Person
 * \endcode
 *
 * Then, all fields are listed as a argument list of tuples:
 * \code
 *  (id, FieldFlag::IsRequired, FieldFlag::IsUnique),
 *  (firstName, FieldMaxLength(250)),
 *  (remarks)
 * \endcode
 *
 * The first argument of a field tuple is its name,
 *  as it was declared in %PersonDataStruct.
 *  The following optional arguments are attributes.
 *  See the Mdt::Reflection::FieldAttributes class for details.
 *
 * \note If the compiler trows a lot of errors,
 *  it should first be checked if the data struct
 *  (%PersonDataStruct in this example) have beend defined before calling MDT_REFLECT_STRUCT() .
 *  Sadly, I did not find any solution to put a short error message
 *  using the preprocessor.
 */
#define MDT_REFLECT_STRUCT(structTuple, name, ...)                                                                \
  MDT_REFLECTION_STRUCT_NAMESPACE_BEGIN(structTuple)                                                              \
  struct MDT_REFLECTION_STRUCT_GET_STRUCT_DEF_NAME(name) : Mdt::Reflection::TypeTraits::StructDefTag              \
  {                                                                                                               \
    using reflected_struct = MDT_REFLECTION_STRUCT_GET_STRUCT_NAME(structTuple);                                  \
                                                                                                                  \
    constexpr MDT_REFLECTION_STRUCT_GET_STRUCT_DEF_NAME(name)() noexcept {}                                       \
                                                                                                                  \
    static constexpr const char *name_() noexcept                                                                 \
    {                                                                                                             \
      return MDT_REFLECTION_STRUCT_GET_NAME_STR(name);                                                            \
    }                                                                                                             \
                                                                                                                  \
    MDT_REFLECTION_STRUCT_FIELD_DEF_LIST(                                                                         \
      MDT_REFLECTION_STRUCT_GET_STRUCT_DEF_NAME(name),                                                            \
      __VA_ARGS__                                                                                                 \
    )                                                                                                             \
    using field_list = boost::mpl::vector<MDT_REFLECTION_STRUCT_FIELD_TUPLE_ARG_LIST_TO_FIELD_ENUM(__VA_ARGS__)>; \
  };                                                                                                              \
  MDT_REFLECTION_STRUCT_NAMESPACE_END(structTuple)                                                                \
                                                                                                                  \
  BOOST_FUSION_ADAPT_ASSOC_STRUCT(                                                                                \
    MDT_REFLECTION_STRUCT_GET_STRUCT_NAMESPACE_NAME(structTuple),                                                 \
    MDT_REFLECTION_STRUCT_FIELD_ADAPT_ASSOC_LIST(structTuple, name, __VA_ARGS__)                                  \
  )

#endif // #ifndef MDT_REFLECTION_REFLECT_STRUCT_H
