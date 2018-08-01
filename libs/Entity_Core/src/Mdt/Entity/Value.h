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
#ifndef MDT_ENTITY_VALUE_H
#define MDT_ENTITY_VALUE_H

#include "TypeTraits/IsEntityFieldDef.h"
#include "TypeTraits/FieldDef.h"
#include <boost/fusion/include/at_key.hpp>

// #include <QDebug>

namespace Mdt{ namespace Entity{

  /*! \brief Get the value for a field in a data struct
   */
  template<typename EntityDataStruct, typename EntityFieldDef>
  const auto & value(const EntityDataStruct & dataStruct)
  {
    static_assert( TypeTraits::IsEntityFieldDef<EntityFieldDef>::value , "EntityFieldDef must be a field definition" );

    return boost::fusion::at_key<EntityFieldDef>(dataStruct);
  }

  /*! \brief Check if a unique id is null
   *
   * Returns true if the value refered by EntityFieldDef
   *  in \a dataStruct is part of the primary key,
   *  is integral and is null.
   *
   * \tparam UniqueId value object that wraps \a id
   *    Must be callable like:
   *    \code
   *    UniqueId(id).isNull()
   *    \endcode
   *
   * \sa IntegralUniqueIdTemplate
   */
  template<typename EntityDataStruct, typename EntityFieldDef, typename UniqueId>
  typename std::enable_if< TypeTraits::isIntegralPrimaryKey<EntityDataStruct, EntityFieldDef>(), bool >::type
  isNullUniqueId(const EntityDataStruct & dataStruct) noexcept
  {
    static_assert( TypeTraits::IsEntityFieldDef<EntityFieldDef>::value , "EntityFieldDef must be a field definition" );

    return UniqueId( value<EntityDataStruct, EntityFieldDef>(dataStruct) ).isNull();
  }

  template<typename EntityDataStruct, typename EntityFieldDef, typename UniqueId>
  typename std::enable_if< !TypeTraits::isIntegralPrimaryKey<EntityDataStruct, EntityFieldDef>(), bool >::type
  isNullUniqueId(const EntityDataStruct &) noexcept
  {
    return false;
  }

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_VALUE_H
