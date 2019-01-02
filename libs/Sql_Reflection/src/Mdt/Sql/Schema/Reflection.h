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

#include "Mdt/Sql/Schema/Table.h"

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Get a SQL schema table from a reflected struct
   *
   * Example to create a table without specifiying a primary key:
   * \code
   * using namespace Mdt::Sql::Schema;
   *
   * auto Table = tableFromReflected<PersonDef>(PersonDataStruct());
   * \endcode
   *
   * Example with a auto increment id:
   * \code
   * using namespace Mdt::Sql::Schema;
   *
   * using PersonPrimaryKey = Mdt::Reflection::AutoIncrementUniqueId<PersonDef, PersonDef::id>;
   *
   * auto Table = tableFromReflected<PersonDef, PersonPrimaryKey>(PersonDataStruct());
   * \endcode
   *
   * \note \a s is a instance of the struct that have beend reflected.
   *   Passing a instance is required to retrieve default values.
   *
   * \sa tableFromReflected<StructDef, PrimaryKey, Struct>(const Struct &)
   */
  template<typename StructDef, typename Struct>
  Table tableFromReflected(const Struct & s)
  {
  }

  /*! \brief
   *
   * \sa tableFromReflected<StructDef, Struct>(const Struct &)
   */
  template<typename StructDef, typename PrimaryKey, typename Struct>
  Table tableFromReflected(const Struct & s)
  {
  }

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_REFLECTION_H
