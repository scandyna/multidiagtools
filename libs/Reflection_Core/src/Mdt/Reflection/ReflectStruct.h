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

// #include "ReflectField.h"
#include <boost/preprocessor/config/config.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/seq/rest_n.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/transform.hpp>
#include <boost/preprocessor/seq/variadic_seq_to_seq.hpp>

// For ReflectField.h
#include <boost/preprocessor/tuple/elem.hpp>


#include <boost/preprocessor/seq/for_each.hpp>

#include <boost/preprocessor/if.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>



#include <boost/preprocessor/tuple/to_array.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>
#include <boost/preprocessor/tuple/reverse.hpp>
#include <boost/preprocessor/tuple/pop_back.hpp>
#include <boost/preprocessor/array/size.hpp>
#include <boost/preprocessor/array/pop_back.hpp>
#include <boost/preprocessor/array/enum.hpp>
#include <boost/preprocessor/array/pop_front.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/adapted/struct/adapt_assoc_struct.hpp>
#include <boost/fusion/include/adapt_assoc_struct.hpp>
#include <boost/mpl/vector.hpp>

#if !BOOST_PP_VARIADICS
 #error "The macro MDT_REFLECT_STRUCT() requires a compiler that supports preprocessor variadics"
#endif

/*! \internal Extract the struct to reflect
 *
 * \code
 * MDT_REFLECTION_STRUCT_GET_STRUCT(
 *   (PersonDataStruct)
 *   (Person)
 *   (id, FieldFlag::IsRequired, FieldFlag::IsUnique)
 *   (firstName, FieldMaxLength(250))
 *   (remarks)
 * )
 * \endcode
 * will expand to:
 * \code
 * PersonDataStruct
 * \endcode
 */
#define MDT_REFLECTION_STRUCT_GET_STRUCT(structSeq) \
  BOOST_PP_SEQ_ELEM(0, structSeq)

/*! \internal Extract the alias
 *
 * \code
 * MDT_REFLECTION_STRUCT_GET_ALIAS(
 *   (PersonDataStruct)
 *   (Person)
 *   (id, FieldFlag::IsRequired, FieldFlag::IsUnique)
 *   (firstName, FieldMaxLength(250))
 *   (remarks)
 * )
 * \endcode
 * will expand to:
 * \code
 * Person
 * \endcode
 */
#define MDT_REFLECTION_STRUCT_GET_ALIAS(structSeq) \
  BOOST_PP_SEQ_ELEM(1, structSeq)

/*! \internal Extract the field tuples sequence
 *
 * \code
 * MDT_REFLECTION_STRUCT_GET_FIELD_TUPLE_SEQ(
 *   (PersonDataStruct)
 *   (Person)
 *   (id, FieldFlag::IsRequired, FieldFlag::IsUnique)
 *   (firstName, FieldMaxLength(250))
 *   (remarks)
 * )
 * \endcode
 * will expand to:
 * \code
 * (id, FieldFlag::IsRequired, FieldFlag::IsUnique)
 * (firstName, FieldMaxLength(250))
 * (remarks)
 * \endcode
 */
#define MDT_REFLECTION_STRUCT_GET_FIELD_TUPLE_SEQ(structSeq) \
  BOOST_PP_SEQ_REST_N(2, structSeq)

/*! \internal Get the field from a field tuple
 *
 * \todo Go to ReflectField.h
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
#define MDT_REFLECTION_STRUCT_FIELD_TUPLE_SEQ_TO_FIELD_SEQ(...) \
  BOOST_PP_SEQ_TRANSFORM( MDT_REFLECTION_STRUCT_FIELD_GET_FIELD_OP, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__) )

/*! \internal Transform a field tuple sequence to a field enum
 *
 * \code
 * MDT_REFLECTION_STRUCT_FIELD_TUPLE_SEQ_TO_FIELD_ENUM(
 *   (id, FieldFlag::IsRequired, FieldFlag::IsUnique)
 *   (firstName, FieldMaxLength(250))
 *   (remarks)
 * )
 * \endcode
 * will expand to:
 * \code
 * id,firstName,remarks
 * \endcode
 */
#define MDT_REFLECTION_STRUCT_FIELD_TUPLE_SEQ_TO_FIELD_ENUM(fieldTupleSeq) \
  BOOST_PP_SEQ_ENUM( MDT_REFLECTION_STRUCT_FIELD_TUPLE_SEQ_TO_FIELD_SEQ(fieldTupleSeq) )

/*! \internal Generate MPL vector with the fields
 *
 * \code
 * MDT_REFLECTION_STRUCT_GEN_FIELD_LIST(
 *   (id, FieldFlag::IsRequired, FieldFlag::IsUnique)
 *   (firstName, FieldMaxLength(250))
 *   (remarks)
 * )
 * \endcode
 * will expand to:
 * \code
 * using field_list = boost::mpl::vector<id, firstName, remarks>;
 * \endcode
 */
#define MDT_REFLECTION_STRUCT_GEN_FIELD_LIST(fieldTupleSeq) \
  using field_list = boost::mpl::vector<MDT_REFLECTION_STRUCT_FIELD_TUPLE_SEQ_TO_FIELD_ENUM(fieldTupleSeq)>;

/*! \internal
 *
 * \code
 * \endcode
 * will expand to:
 * \code
 * \endcode
 */

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
 *   static constexpr const char *name()
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


namespace Mdt{ namespace Reflection{

}} // namespace Mdt{ namespace Reflection{

#endif // #ifndef MDT_REFLECTION_REFLECT_STRUCT_H
