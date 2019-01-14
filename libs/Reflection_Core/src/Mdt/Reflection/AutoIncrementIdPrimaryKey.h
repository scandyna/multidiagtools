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
#ifndef MDT_REFLECTION_AUTO_INCREMENT_ID_PRIMARY_KEY_H
#define MDT_REFLECTION_AUTO_INCREMENT_ID_PRIMARY_KEY_H

#include "FieldAlgorithm.h"
#include "TypeTraits/PrimaryKeyTag.h"
#include "TypeTraits/IsField.h"
#include <boost/mpl/vector.hpp>
#include <type_traits>

namespace Mdt{ namespace Reflection{

  /*! \brief Auto increment primary key for a reflected struct
   *
   * \pre \a Field must be a field defined in a struct definition associated with a reflected struct
   * \pre \a Field must refer to a integral type in the reflected struct
   */
  template<typename Field>
  class AutoIncrementIdPrimaryKey : TypeTraits::PrimaryKeyTag
  {
    static_assert( TypeTraits::IsField<Field>::value , "Field must be a field defined in a struct definition associated with a reflected struct" );
    static_assert( std::is_integral< typename TypeFromField<Field>::type >::value, "Field must refer to a integral type in the reflected struct" );

   public:

    using struct_def = typename Field::struct_def;
    using field_list = boost::mpl::vector<Field>;
  };


}} // namespace Mdt{ namespace Reflection{

#endif // #ifndef MDT_REFLECTION_AUTO_INCREMENT_ID_PRIMARY_KEY_H
