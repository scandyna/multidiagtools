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
#ifndef MDT_REFLECTION_TYPE_TRAITS_IS_AUTO_INCREMENT_ID_PRIMARYKEY_H
#define MDT_REFLECTION_TYPE_TRAITS_IS_AUTO_INCREMENT_ID_PRIMARYKEY_H

#include "../AutoIncrementIdPrimaryKey.h"
#include <boost/mpl/at.hpp>
#include <type_traits>

namespace Mdt{ namespace Reflection{ namespace TypeTraits{

  /*! \brief Check if Pk is a auto increment id primary key
   */
  template<typename Pk, typename Field = typename boost::mpl::at<typename Pk::field_list, boost::mpl::int_<0> >::type >
  struct IsAutoIncrementIdPrimaryKey : std::is_same< Pk, AutoIncrementIdPrimaryKey<Field> >
  {
  };

}}} // namespace Mdt{ namespace Reflection{ namespace TypeTraits{

#endif // #ifndef MDT_REFLECTION_TYPE_TRAITS_IS_AUTO_INCREMENT_ID_PRIMARYKEY_H
