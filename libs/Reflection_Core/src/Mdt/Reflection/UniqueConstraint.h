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
#ifndef MDT_REFLECTION_UNIQUE_CONSTRAINT_H
#define MDT_REFLECTION_UNIQUE_CONSTRAINT_H

#include "TypeTraits/UniqueConstraintTag.h"
#include <boost/mpl/vector.hpp>
#include <boost/mpl/at.hpp>

namespace Mdt{ namespace Reflection{

  /*! \brief Unique constraint on one or more fields in a reflected struct
   *
   * For example, define a %Person entity:
   * \code
   * struct PersonDataStruct
   * {
   *   int id;
   *   QString firstName;
   *   QString lastName;
   * };
   *
   * MDT_REFLECT_STRUCT(
   *   (PersonDataStruct),
   *   Person,
   *   (id),
   *   (firstName),
   *   (lastName)
   * )
   * \endcode
   *
   * To define a unique constraint on first name and last name:
   * \code
   * using PersonNameUniqueConstraint = Mdt::Reflection::UniqueConstraint<PersonDef::firstName, PersonDef::lastName>;
   * \endcode
   *
   * \pre The count of fields in the constraint must be at least 1
   */
  template<typename ...FieldList>
  class UniqueConstraint : TypeTraits::UniqueConstraintTag
  {
    static_assert( sizeof...(FieldList) >= 1 , "A unique constraint must contain at least 1 field" );

   public:

    using field_list = boost::mpl::vector<FieldList...>;
    using struct_def = typename boost::mpl::at_c<field_list, 0>::type::struct_def;
  };

}} // namespace Mdt{ namespace Reflection{

#endif // #ifndef MDT_REFLECTION_UNIQUE_CONSTRAINT_H
