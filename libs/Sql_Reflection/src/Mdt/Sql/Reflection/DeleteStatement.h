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
#ifndef MDT_SQL_REFLECTION_DELETE_STATEMENT_FUNCTIONS_H
#define MDT_SQL_REFLECTION_DELETE_STATEMENT_FUNCTIONS_H

#include "Mdt/Sql/DeleteStatement.h"
#include "Mdt/Reflection/StructAlgorithm.h"
#include "Mdt/Reflection/PrimaryKeyAlgorithm.h"
#include "Mdt/Reflection/TypeTraits/IsStructDef.h"
#include "Mdt/Reflection/TypeTraits/IsStructDefAssociatedWithReflectedStruct.h"
#include "Mdt/Reflection/TypeTraits/IsPrimaryKeyClass.h"
#include "Mdt/Reflection/TypeTraits/IsPrimaryKeyRecord.h"
#include "Mdt/QueryExpression/ReflectionPrimaryKeyRecordAlgorithm.h"
#include <QLatin1String>
#include <type_traits>

namespace Mdt{ namespace Sql{ namespace Reflection{

  /*! \brief Get a delete statement from a reflected struct
   *
   * Create a SQL delete statement:
   * \code
   * using PersonPrimaryKey = Mdt::Reflection::PrimaryKey<PersonDef::id>;
   * using PersonPrimaryKeyRecord = Mdt::Reflection::PrimaryKeyRecord<PersonPrimaryKey>;
   *
   * PersonPrimaryKeyRecord personPkRecord;
   * personPkRecord.setValue<PersonDef::id>(5);
   *
   * const auto statement = Mdt::Sql::Reflection::deleteStatementFromReflectedPrimaryKey(personPkRecord);
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
   * \pre \a PrimaryKeyRecord must be a Mdt::Reflection::PrimaryKeyRecord for a reflected struct
   *
   * \pre \a PrimaryKey must be primary key class for a reflected struct
   * \pre \a pkRecord must contain the same fields than defined in \a PrimaryKey, and in the same order
   *
   * \sa Mdt::Sql::DeleteStatement
   * \sa Mdt::Sql::DeleteQuery
   */
  template<typename PrimaryKeyRecord>
  DeleteStatement deleteStatementFromReflectedByPrimaryKey(const PrimaryKeyRecord & pkRecord)
  {
    static_assert( Mdt::Reflection::TypeTraits::IsPrimaryKeyRecord<PrimaryKeyRecord>::value,
                   "PrimaryKeyRecord must be a Mdt::Reflection::PrimaryKeyRecord for the reflected struct \a Struct ( \a data )" );

    using primary_key = typename PrimaryKeyRecord::primary_key;

    DeleteStatement statement;

    statement.setTableName( Mdt::Reflection::nameFromPrimaryKeyQString<primary_key>() );
    statement.setConditionsFilterExpression( Mdt::QueryExpression::filterExpressionFromPrimaryKeyRecord(pkRecord) );

    return statement;
  }

  /*! \brief Get a delete statement from a reflected struct
   *
   * Create a SQL delete statement:
   * \code
   * const auto statement = Mdt::Sql::Reflection::deleteStatementFromReflectedById<PersonPrimaryKey>(personId);
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
   * \pre \a PrimaryKey must be a primary key class for a reflected struct
   *          and must contain exactly 1 field
   * \pre \a id must be integral type
   *
   * \sa Mdt::Sql::DeleteStatement
   * \sa Mdt::Sql::DeleteQuery
   */
  template<typename PrimaryKey, typename Id>
  DeleteStatement deleteStatementFromReflectedById(Id id)
  {
    static_assert( Mdt::Reflection::TypeTraits::IsPrimaryKeyClass<PrimaryKey>::value,
                   "PrimaryKey must be a primary key class for a reflected struct" );
    static_assert( Mdt::Reflection::primaryKeyFieldCount<PrimaryKey>() == 1,
                   "PrimaryKey must contain exactly 1 field" );
    static_assert( std::is_integral<Id>::value,
                   "id must be a integral type" );


    DeleteStatement statement;

    statement.setTableName( Mdt::Reflection::nameFromPrimaryKeyQString<PrimaryKey>() );
    statement.setConditionsFilterExpression( Mdt::QueryExpression::filterExpressionFromIdPrimaryKeyValue<PrimaryKey>(id) );

    return statement;
  }

  /*! \brief Get a delete all statement from a reflected struct
   *
   * Create a SQL delete statement:
   * \code
   * const auto statement = Mdt::Sql::Reflection::deleteAllStatementFromReflected<PersonDef>();
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
   * \pre \a StructDef must be a struct definition assiocated with a reflected struct
   *
   * \sa Mdt::Sql::DeleteStatement
   * \sa Mdt::Sql::DeleteQuery
   */
  template<typename StructDef>
  DeleteStatement deleteAllStatementFromReflected()
  {
    static_assert( Mdt::Reflection::TypeTraits::IsStructDef<StructDef>::value,
                   "StructDef must be a struct definition assiocated with a reflected struct" );

    DeleteStatement statement;

    statement.setTableName( Mdt::Reflection::nameFromStructDefQString<StructDef>() );

    return statement;
  }

}}} // namespace Mdt{ namespace Sql{ namespace Reflection{

#endif // #ifndef MDT_SQL_REFLECTION_DELETE_STATEMENT_FUNCTIONS_H
