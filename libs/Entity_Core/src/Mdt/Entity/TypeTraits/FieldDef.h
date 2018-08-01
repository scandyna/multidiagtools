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
#ifndef MDT_ENTITY_TYPE_TRAITS_H
#define MDT_ENTITY_TYPE_TRAITS_H

#include "IsEntityFieldDef.h"
#include "../FieldAttributes.h"
#include <boost/fusion/include/at_key.hpp>
#include <type_traits>

namespace Mdt{ namespace Entity{ namespace TypeTraits{

  /*! \brief Check if a field is part of the primary key
   */
  template<typename EntityFieldDef>
  constexpr bool isPrimaryKey() noexcept
  {
    static_assert( IsEntityFieldDef<EntityFieldDef>::value , "EntityFieldDef must be a field definition" );

    return EntityFieldDef::fieldAttributes().isPrimaryKey();
  }

  /*! \brief Check if a field related to a data struct is of integral type
   */
  template<typename EntityDataStruct, typename EntityFieldDef>
  constexpr bool isIntegralType() noexcept
  {
    static_assert( IsEntityFieldDef<EntityFieldDef>::value , "EntityFieldDef must be a field definition" );

    using RefValueType = typename boost::fusion::result_of::at_key<EntityDataStruct, EntityFieldDef>::type;
    using ValueType = typename std::remove_reference<RefValueType>::type;

    return std::is_integral<ValueType>::value;
  }

  /*! \brief Check if a field related to a data struct is a integral primary key
   */
  template<typename EntityDataStruct, typename EntityFieldDef>
  constexpr bool isIntegralPrimaryKey() noexcept
  {
    static_assert( IsEntityFieldDef<EntityFieldDef>::value , "EntityFieldDef must be a field definition" );

    return isIntegralType<EntityDataStruct, EntityFieldDef>() && isPrimaryKey<EntityFieldDef>();
  }

}}} // namespace Mdt{ namespace Entity{ namespace TypeTraits{

#endif // #ifndef MDT_ENTITY_TYPE_TRAITS_H
