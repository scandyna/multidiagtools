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
#ifndef MDT_SQL_REFLECTION_SCHEMA_REFLECTION_UNIQUE_INDEX_ALGORITHM_H
#define MDT_SQL_REFLECTION_SCHEMA_REFLECTION_UNIQUE_INDEX_ALGORITHM_H

#include "Mdt/Sql/Schema/Index.h"
#include "Mdt/Reflection/UniqueConstraintAlgorithm.h"
#include "Mdt/Reflection/TypeTraits/IsUniqueConstraint.h"
#include <QLatin1String>

namespace Mdt{ namespace Sql{ namespace Schema{ namespace Reflection{

  /*! \brief Get a unique index from a reflected unique constraint
   *
   * \pre \a UniqueConstraint must be a unique constraint
   */
  template<typename UniqueConstraint>
  Index uniqueIndexFromReflected()
  {
    static_assert( Mdt::Reflection::TypeTraits::IsUniqueConstraint<UniqueConstraint>::value , "UniqueConstraint must be a unique constraint" );

    Index index;

    index.setUnique(true);
    index.setTableName( QLatin1String(Mdt::Reflection::nameFromUniqueConstraint<UniqueConstraint>()) );
    index.setFieldNameList( Mdt::Reflection::fieldNameListFromUniqueConstraint<UniqueConstraint>() );
    index.generateName();

    return index;
  }

}}}} // namespace Mdt{ namespace Sql{ namespace Schema{ namespace Reflection{


#endif // #ifndef MDT_SQL_REFLECTION_SCHEMA_REFLECTION_UNIQUE_INDEX_ALGORITHM_H
