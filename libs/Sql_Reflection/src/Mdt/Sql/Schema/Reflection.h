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
#ifndef MDT_SQL_SCHEMA_REFLECTION_H
#define MDT_SQL_SCHEMA_REFLECTION_H

#include "Reflection/TableAlgorithm.h"
#include "Reflection/UniqueIndexAlgorithm.h"
#include "Mdt/Sql/Schema/Table.h"
#include "Mdt/Sql/Schema/FieldTypeMap.h"
#include "Mdt/Reflection/TypeTraits/IsStructDef.h"
#include "Mdt/Reflection/TypeTraits/IsPrimaryKeyClass.h"
#include "Mdt/Reflection/TypeTraits/IsUniqueConstraint.h"
// #include "Mdt/Reflection/StructAlgorithm.h"
#include <QLatin1String>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Get a SQL schema table from a reflected struct
   *
   * Example to create a table without specifiying a primary key:
   * \code
   * using namespace Mdt::Sql::Schema;
   *
   * auto Table = tableFromReflected<PersonDef>();
   * \endcode
   *
   * \pre \a StructDef must be a struct definition assiocated with a reflected struct
   */
  template<typename StructDef>
  Table tableFromReflected(const FieldTypeMap & fieldTypeMap = FieldTypeMap::make())
  {
    static_assert( Mdt::Reflection::TypeTraits::IsStructDef<StructDef>::value,
                   "StructDef must be a struct definition assiocated with a reflected struct" );

    return Reflection::tableFromReflectedImpl<StructDef>(fieldTypeMap);
  }

  /*! \brief Get a SQL schema table from a reflected struct
   *
   * Example with a auto increment id:
   * \code
   * using namespace Mdt::Sql::Schema;
   *
   * using PersonPrimaryKey = Mdt::Reflection::AutoIncrementUniqueId<PersonDef::id>;
   *
   * auto Table = tableFromReflected<PersonDef, PersonPrimaryKey>();
   * \endcode
   *
   * \pre \a StructDef must be a struct definition assiocated with a reflected struct
   */
  template<typename StructDef, typename PrimaryKey>
  Table tableFromReflected(const FieldTypeMap & fieldTypeMap = FieldTypeMap::make())
  {
    static_assert( Mdt::Reflection::TypeTraits::IsStructDef<StructDef>::value,
                   "StructDef must be a struct definition assiocated with a reflected struct" );
    static_assert( Mdt::Reflection::TypeTraits::IsPrimaryKeyClass<PrimaryKey>::value,
                   "PrimaryKey must be a primary key" );

    return Reflection::tableFromReflectedImpl<StructDef, PrimaryKey>(fieldTypeMap);
  }

    /*! \brief Add a unique constraint to \a table
     *
     * \pre \a UniqueConstraint must be a unique constraint
     */
    template<typename UniqueConstraint>
    static void addUniqueConstraintToTable(Table & table)
    {
      static_assert( Mdt::Reflection::TypeTraits::IsUniqueConstraint<UniqueConstraint>::value , "UniqueConstraint must be a unique constraint" );

      table.addIndex( Reflection::uniqueIndexFromReflected<UniqueConstraint>() );
    }

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_REFLECTION_H
