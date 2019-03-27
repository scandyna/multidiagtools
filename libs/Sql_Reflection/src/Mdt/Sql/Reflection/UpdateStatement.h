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
#ifndef MDT_SQL_REFLECTION_UPDATE_STATEMENT_H
#define MDT_SQL_REFLECTION_UPDATE_STATEMENT_H

#include "Mdt/Sql/UpdateStatement.h"
#include "Mdt/Sql/FieldName.h"
#include "Mdt/Reflection/StructAlgorithm.h"
#include "Mdt/Reflection/PrimaryKeyRecord.h"
#include "Mdt/Reflection/PrimaryKeyRecordAlgorithm.h"
#include "Mdt/Reflection/TypeTraits/IsStructDef.h"
#include "Mdt/Reflection/TypeTraits/IsStructDefAssociatedWithReflectedStruct.h"
#include "Mdt/Reflection/TypeTraits/IsPrimaryKeyClass.h"
#include "Mdt/Reflection/TypeTraits/IsPrimaryKeyRecord.h"
#include "Mdt/QueryExpression/ReflectionPrimaryKeyRecordAlgorithm.h"
#include <QLatin1String>

namespace Mdt{ namespace Sql{ namespace Reflection{

  namespace Impl{

    /*! \internal
     */
    template<typename Pk>
    struct AddValueToUpdateStatement
    {
      AddValueToUpdateStatement(UpdateStatement & statement)
       : mStatement(statement)
      {
      }

      template<typename FieldValuePair>
      void operator()(const FieldValuePair & p) const
      {
        using Mdt::Sql::FieldName;

        if( mustAddValueToStatement<typename FieldValuePair::first_type>() ){
          mStatement.addValue( FieldName(Mdt::Reflection::fieldNameQString<typename FieldValuePair::first_type>()), p.second );
        }
      }

     private:

      template<typename Field>
      static bool mustAddValueToStatement() noexcept
      {
        return !Mdt::Reflection::isFieldPartOfPrimaryKey<Pk, Field>();
      }

      UpdateStatement & mStatement;
    };

  } // namespace Impl{

  /*! \brief Get a update statement from a reflected struct
   *
   * Create a SQL update statement:
   * \code
   * const auto statement = Mdt::Sql::Reflection::updateStatementFromReflectedByPrimaryKey(personData, personPkRecord);
   * \endcode
   * This statement can be used with a SQL update query:
   * \code
   * Mdt::Sql::UpdateQuery query(dbConnection);
   *
   * if(!query.execStatement(statement)){
   *   // Error handling
   * }
   * \endcode
   *
   * \pre \a PrimaryKeyRecord must be a Mdt::Reflection::PrimaryKeyRecord for the reflected struct \a Struct ( \a data )
   *
   * \sa updateStatementFromReflected()
   * \sa Mdt::Sql::UpdateStatement
   * \sa Mdt::Sql::UpdateQuery
   */
  template<typename Struct, typename PrimaryKeyRecord>
  UpdateStatement updateStatementFromReflectedByPrimaryKey(const Struct & data, const PrimaryKeyRecord & pkRecord)
  {
    static_assert( Mdt::Reflection::TypeTraits::IsPrimaryKeyRecord<PrimaryKeyRecord>::value,
                   "PrimaryKeyRecord must be a Mdt::Reflection::PrimaryKeyRecord for the reflected struct \a Struct ( \a data )" );

    using primary_key = typename PrimaryKeyRecord::primary_key;
    using struct_def = typename primary_key::struct_def;
    static_assert( Mdt::Reflection::TypeTraits::IsStructDefAssociatedWithReflectedStruct<struct_def, Struct>::value ,
                   "PrimaryKeyRecord must be associated with given data of type Struct" );

    UpdateStatement statement;
    Impl::AddValueToUpdateStatement<primary_key> f(statement);

    statement.setTableName( Mdt::Reflection::nameFromStructDefQString<struct_def>() );
    Mdt::Reflection::forEachFieldValuePairInStruct(data, f);
    statement.setConditionsFilterExpression( Mdt::QueryExpression::filterExpressionFromPrimaryKeyRecord(pkRecord) );

    return statement;
  }

  /*! \brief Get a update statement from a reflected struct
   *
   * Create a SQL update statement:
   * \code
   * const auto statement = Mdt::Sql::Reflection::updateStatementFromReflected<PersonPrimaryKey>(personData);
   * \endcode
   * This statement can be used with a SQL update query:
   * \code
   * Mdt::Sql::UpdateQuery query(dbConnection);
   *
   * if(!query.execStatement(statement)){
   *   // Error handling
   * }
   * \endcode
   *
   * \pre \a PrimaryKey must be primary key class for ther reflected struct \a Struct ( \a data )
   * \pre \a pkRecord must contain the same fields than defined in \a PrimaryKey, and in the same order
   *
   * \sa Mdt::Sql::UpdateStatement
   * \sa Mdt::Sql::UpdateQuery
   */
  template<typename PrimaryKey, typename Struct>
  UpdateStatement updateStatementFromReflected(const Struct & data)
  {
    static_assert( Mdt::Reflection::TypeTraits::IsPrimaryKeyClass<PrimaryKey>::value,
                   "PrimaryKey must be a primary key class for a reflected struct" );

    using struct_def = typename PrimaryKey::struct_def;
    static_assert( Mdt::Reflection::TypeTraits::IsStructDefAssociatedWithReflectedStruct<struct_def, Struct>::value ,
                   "PrimaryKey must be associated with given data of type Struct" );

    using primary_key_record = Mdt::Reflection::PrimaryKeyRecord<PrimaryKey>;

    const auto pkr = Mdt::Reflection::primaryKeyRecordFromStruct<primary_key_record>(data);

    return updateStatementFromReflectedByPrimaryKey(data, pkr);
  }

}}} // namespace Mdt{ namespace Sql{ namespace Reflection{

#endif // #ifndef MDT_SQL_REFLECTION_UPDATE_STATEMENT_H
