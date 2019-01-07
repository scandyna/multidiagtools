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
#ifndef MDT_REFLECTION_PRIMARY_KEY_H
#define MDT_REFLECTION_PRIMARY_KEY_H

#include "TypeTraits/PrimaryKeyTag.h"
#include <boost/mpl/vector.hpp>
#include <boost/mpl/at.hpp>

namespace Mdt{ namespace Reflection{

  /*! \brief Primary key for a reflected struct
   */
  template<typename ...FieldList>
  class PrimaryKey : TypeTraits::PrimaryKeyTag
  {
    static_assert( sizeof...(FieldList) >= 1 , "A primary key must contain at least 1 field" );

   public:

    using field_list = boost::mpl::vector<FieldList...>;
    using struct_def = typename boost::mpl::at_c<field_list, 0>::type::struct_def;
  };


}} // namespace Mdt{ namespace Reflection{

#endif // #ifndef MDT_REFLECTION_PRIMARY_KEY_H
