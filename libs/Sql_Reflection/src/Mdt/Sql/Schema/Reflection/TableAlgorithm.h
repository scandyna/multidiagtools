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
#ifndef MDT_SQL_REFLECTION_SCHEMA_REFLECTION_TABLE_ALGORITHM_H
#define MDT_SQL_REFLECTION_SCHEMA_REFLECTION_TABLE_ALGORITHM_H

#include "FieldAlgorithm.h"
#include "PrimaryKeyAlgorithm.h"
#include "Mdt/Sql/Schema/Table.h"
#include "Mdt/Sql/Schema/FieldTypeMap.h"
#include "Mdt/Reflection/TypeTraits/IsStructDef.h"
#include "Mdt/Reflection/TypeTraits/IsPrimaryKeyClass.h"
#include "Mdt/Reflection/StructAlgorithm.h"
// #include "MdtSql_ReflectionExport.h"
#include <QMetaType>
#include <QLatin1String>

namespace Mdt{ namespace Sql{ namespace Schema{ namespace Reflection{

  /*! \brief Functor called from tableFromReflectedImpl()
   */
  template<typename Struct>
  class AddFieldToTable
  {
   public:

    AddFieldToTable(Table & table, const Struct & s, const FieldTypeMap & fieldTypeMap)
     : mTable(table),
       mStruct(s),
       mFieldTypeMap(fieldTypeMap)
    {
    }

    template<typename Field>
    void operator()(const Field)
    {
      mTable.addField( fieldFromReflected<Field>(mStruct, mFieldTypeMap) );
    }

   private:

    Table & mTable;
    const Struct & mStruct;
    const FieldTypeMap & mFieldTypeMap;
  };

  /*! \internal Get a SQL schema table from a reflected struct
   *
   * \pre \a StructDef must be a struct definition assiocated with a reflected struct
   */
  template<typename StructDef>
  Table tableFromReflectedImpl(const FieldTypeMap & fieldTypeMap)
  {
    static_assert( Mdt::Reflection::TypeTraits::IsStructDef<StructDef>::value,
                   "StructDef must be a struct definition assiocated with a reflected struct" );

    using namespace Mdt::Reflection;
    using Struct = typename StructDef::reflected_struct;

    Table table;

    table.setTableName( QLatin1String( nameFromStructDef<StructDef>() ) );
    Struct s;
    AddFieldToTable<Struct> f(table, s, fieldTypeMap);
    forEachFieldInStructDef<StructDef>(f);

    return table;
  }

  /*! \internal Get a SQL schema table from a reflected struct
   *
   *
   * \pre \a StructDef must be a struct definition assiocated with a reflected struct
   */
  template<typename StructDef, typename PrimaryKey>
  Table tableFromReflectedImpl(const FieldTypeMap & fieldTypeMap)
  {
    static_assert( Mdt::Reflection::TypeTraits::IsStructDef<StructDef>::value,
                   "StructDef must be a struct definition assiocated with a reflected struct" );
    static_assert( Mdt::Reflection::TypeTraits::IsPrimaryKeyClass<PrimaryKey>::value,
                   "PrimaryKey must be a primary key" );

    auto table = tableFromReflectedImpl<StructDef>(fieldTypeMap);
    table.setPrimaryKeyContainer( primaryKeyContainerFromReflected<PrimaryKey>(fieldTypeMap) );

    return table;
  }

}}}} // namespace Mdt{ namespace Sql{ namespace Schema{ namespace Reflection{


#endif // #ifndef MDT_SQL_REFLECTION_SCHEMA_REFLECTION_TABLE_ALGORITHM_H
