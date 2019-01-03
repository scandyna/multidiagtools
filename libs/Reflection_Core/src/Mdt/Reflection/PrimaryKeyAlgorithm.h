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
#ifndef MDT_REFLECTION_PRIMARY_KEY_ALGORITHM_H
#define MDT_REFLECTION_PRIMARY_KEY_ALGORITHM_H

#include "PrimaryKey.h"
#include <boost/mpl/for_each.hpp>

namespace Mdt{ namespace Reflection{

  /*! \brief Apply a functor for each field in a primary key
   *
   * \code
   * struct MyFunctor
   * {
   *   template<typename Field>
   *   void operator()(Field)
   *   {
   *   }
   * };
   * \endcode
   */
  template<typename PrimaryKey, typename UnaryFunction>
  void forEachPrimaryKeyField(UnaryFunction f)
  {
    boost::mpl::for_each< typename PrimaryKey::field_list >(f);
  }

}} // namespace Mdt{ namespace Reflection{

#endif // #ifndef MDT_REFLECTION_PRIMARY_KEY_ALGORITHM_H
