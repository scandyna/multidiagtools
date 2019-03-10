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
#ifndef MDT_REFLECTION_ID_PRIMARY_KEY_H
#define MDT_REFLECTION_ID_PRIMARY_KEY_H

#include "TypeTraits/PrimaryKeyClassTag.h"
#include "TypeTraits/IdPrimaryKeyClassTag.h"
#include "TypeTraits/IsField.h"
#include <boost/mpl/vector.hpp>

namespace Mdt{ namespace Reflection{

  /*! \brief Id primary key for a reflected struct
   *
   * \pre \a Field must be a field defined in a struct definition associated with a reflected struct
   */
  template<typename Field>
  class IdPrimaryKey : TypeTraits::PrimaryKeyClassTag, TypeTraits::IdPrimaryKeyClassTag
  {
    static_assert( TypeTraits::IsField<Field>::value , "Field must be a field defined in a struct definition associated with a reflected struct" );

   public:

    using struct_def = typename Field::struct_def;
    using field_list = boost::mpl::vector<Field>;
    using field = Field;
  };


}} // namespace Mdt{ namespace Reflection{

#endif // #ifndef MDT_REFLECTION_ID_PRIMARY_KEY_H
