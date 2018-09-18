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
#ifndef MDT_ENTITY_UNIQUE_CONSTRAINT_ALGORITHM_H
#define MDT_ENTITY_UNIQUE_CONSTRAINT_ALGORITHM_H

#include "UniqueConstraint.h"
#include "TypeTraits/IsUniqueConstraint.h"
#include "TypeTraits/IsEntity.h"
#include "TypeTraits/IsEntityFieldDef.h"
#include "Impl/AddFieldNameToFieldNameList.h"
#include <QStringList>
#include <initializer_list>

namespace Mdt{ namespace Entity{

  namespace Impl{

    template<typename UnaryFunction,typename Field , typename ...Fields>
    void applyFunctorToUniqueConstraintField(UnaryFunction & f)
    {
      f(Field());
    }

    template<typename Entity, typename ...Fields, typename UnaryFunction>
    void forEachUniqueConstraintField(const UniqueConstraint<Entity, Fields...> &, UnaryFunction & f)
    {
      (void)std::initializer_list<int>{ (applyFunctorToUniqueConstraintField<UnaryFunction, Fields>(f) ,0)... };
    }

  } // namespace Impl{

  /*! \brief Apply a function object for each field in a unique constraint
   *
   * \param f a function object with a signature equivalent to:
   *    \code
   *    template<typename Field>
   *    void f(const Field & field);
   *    \endcode
   *
   * \pre \a UniqueConstraint must be based on UniqueConstraint
   */
  template<typename UniqueConstraint, typename UnaryFunction>
  void forEachUniqueConstraintField(UnaryFunction & f)
  {
    static_assert( TypeTraits::IsUniqueConstraint<UniqueConstraint>::value, "UniqueConstraint must be based on UniqueConstraint" );

    Impl::forEachUniqueConstraintField(UniqueConstraint(), f);
  }

  /*! \brief Get the list of field names part of the unique constraint
   */
  template<typename UniqueConstraint>
  QStringList getUniqueConstraintFieldNameList()
  {
    static_assert( TypeTraits::IsUniqueConstraint<UniqueConstraint>::value, "UniqueConstraint must be based on UniqueConstraint" );

    Impl::AddFieldNameToFieldNameList f;

    forEachUniqueConstraintField<UniqueConstraint>(f);

    return f.fieldNameList();
  }

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_UNIQUE_CONSTRAINT_ALGORITHM_H
