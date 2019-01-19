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
#ifndef MDT_REFLECTION_PRIMARY_UNIQUE_CONSTRAINT_ALGORITHM_H
#define MDT_REFLECTION_PRIMARY_UNIQUE_CONSTRAINT_ALGORITHM_H

#include "UniqueConstraint.h"
#include "FieldAlgorithm.h"
#include "TypeTraits/IsField.h"
#include "TypeTraits/IsUniqueConstraint.h"
#include "Impl/AddFieldNameToList.h"
#include <QStringList>
#include <boost/mpl/for_each.hpp>

namespace Mdt{ namespace Reflection{

  /*! \brief Apply a functor for each field in a unique constraint
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
   *
   * \pre \a UniqueConstraint must be a unique constraint
   */
  template<typename UniqueConstraint, typename UnaryFunction>
  void forEachUniqueConstraintField(UnaryFunction f)
  {
    static_assert( TypeTraits::IsUniqueConstraint<UniqueConstraint>::value , "UniqueConstraint must be a unique constraint" );

    boost::mpl::for_each< typename UniqueConstraint::field_list >(f);
  }

  /*! \brief Get a list of field names from a unique constraint
   *
   * \pre \a UniqueConstraint must be a unique constraint
   */
  template<typename UniqueConstraint>
  QStringList fieldNameListFromUniqueConstraint()
  {
    static_assert( TypeTraits::IsUniqueConstraint<UniqueConstraint>::value , "UniqueConstraint must be a unique constraint" );

    QStringList fieldNameList;
    Impl::AddFieldNameToList f(fieldNameList);

    forEachUniqueConstraintField<UniqueConstraint>(f);

    return fieldNameList;
  }

}} // namespace Mdt{ namespace Reflection{

#endif // #ifndef MDT_REFLECTION_PRIMARY_UNIQUE_CONSTRAINT_ALGORITHM_H
