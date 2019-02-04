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
#ifndef MDT_SQL_REFLECTION_DELETE_STATEMENT_H
#define MDT_SQL_REFLECTION_DELETE_STATEMENT_H

#include "PrimaryKeyRecordAlgorithm.h"
#include "Mdt/Reflection/StructAlgorithm.h"
#include "Mdt/Reflection/TypeTraits/IsStructDef.h"
#include "Mdt/Reflection/TypeTraits/IsStructDefAssociatedWithReflectedStruct.h"
#include "Mdt/Reflection/TypeTraits/IsPrimaryKeyClass.h"
#include "Mdt/Sql/DeleteStatement.h"
#include "Mdt/Sql/PrimaryKeyRecord.h"
#include "Mdt/Sql/FieldName.h"
#include <QLatin1String>

namespace Mdt{ namespace Sql{ namespace Reflection{

  /*! \brief Get a delete statement from a reflected struct
   *
   * Create a SQL delete statement:
   * \code
   * const auto statement = Mdt::Sql::Reflection::deleteStatementFromReflectedPrimaryKey<PersonPrimaryKey>(personPkRecord);
   * \endcode
   * This statement can be used with a SQL delete query:
   * \code
   * Mdt::Sql::DeleteQuery query(dbConnection);
   *
   * if(!query.execStatement(statement)){
   *   // Error handling
   * }
   * \endcode
   *
   * \pre \a PrimaryKey must be primary key class for a reflected struct
   * \pre \a pkRecord must contain the same fields than defined in \a PrimaryKey, and in the same order
   *
   * \sa Mdt::Sql::DeleteStatement
   * \sa Mdt::Sql::DeleteQuery
   */
  template<typename PrimaryKey>
  DeleteStatement deleteStatementFromReflectedPrimaryKey(const PrimaryKeyRecord & pkRecord)
  {
    static_assert( Mdt::Reflection::TypeTraits::IsPrimaryKeyClass<PrimaryKey>::value,
                   "PrimaryKey must be a primary key class for a reflected struct" );
    Q_ASSERT( primaryKeyRecordHasCorrectFieldNameList<PrimaryKey>(pkRecord) );

    using struct_def = typename PrimaryKey::struct_def;

    DeleteStatement statement;

    statement.setTableName( QLatin1String(Mdt::Reflection::nameFromStructDef<struct_def>()) );
    statement.setConditions(pkRecord);

    return statement;
  }

}}} // namespace Mdt{ namespace Sql{ namespace Reflection{


#endif // #ifndef MDT_SQL_REFLECTION_DELETE_STATEMENT_H
