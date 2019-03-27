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
#ifndef MDT_SQL_REFLECTION_INSERT_STATEMENT_H
#define MDT_SQL_REFLECTION_INSERT_STATEMENT_H

#include "Mdt/Reflection/StructAlgorithm.h"
#include "Mdt/Reflection/FieldAlgorithm.h"
#include "Mdt/Reflection/PrimaryKeyAlgorithm.h"
#include "Mdt/Reflection/TypeTraits/IsStructDef.h"
#include "Mdt/Reflection/TypeTraits/IsStructDefAssociatedWithReflectedStruct.h"
#include "Mdt/Reflection/TypeTraits/IsPrimaryKeyClass.h"
#include "Mdt/Sql/InsertStatement.h"
#include "Mdt/Sql/FieldName.h"
#include <QLatin1String>

namespace Mdt{ namespace Sql{ namespace Reflection{

  namespace Impl{

    /*! \internal
     */
    template<typename Pk>
    struct AddValueToInsertStatement
    {
      AddValueToInsertStatement(InsertStatement & statement)
       : mStatement(statement)
      {
      }

      template<typename FieldValuePair>
      void operator()(const FieldValuePair & p) const
      {
        using Mdt::Sql::FieldName;

        if( mustAddValueToStatement<typename FieldValuePair::first_type>(p.second) ){
          mStatement.addValue( FieldName(Mdt::Reflection::fieldNameQString<typename FieldValuePair::first_type>()), p.second );
        }
      }

     private:

      template<typename Field, typename Value>
      static bool mustAddValueToStatement(const Value & value) noexcept
      {
        if(Mdt::Reflection::isNullValuePartOfAutoIncrementIdPrimaryKey<Pk, Field>(value)){
          return false;
        }
        return true;
      }

      InsertStatement & mStatement;
    };

  } // namespace Impl{

  /*! \brief Get a insert statement from a reflected struct
   *
   * Create a SQL insert statement:
   * \code
   * const auto statement = Mdt::Sql::Reflection::insertStatementFromReflected<PersonPrimaryKey>(person);
   * \endcode
   * This statement can be used with a SQL insert query:
   * \code
   * Mdt::Sql::InsertQuery query(dbConnection);
   *
   * if(!query.execStatement(statement)){
   *   // Error handling
   * }
   * \endcode
   *
   * If \a PrimaryKey is a AutoIncrementIdPrimaryKey,
   *  and the value associated to it in \a data is null,
   *  it will not be added to the set of values in the statement
   *  (This way, the DBMS will auto-generate a new id).
   *
   * \pre \a PrimaryKey must be primary key class for a reflected struct
   *
   * \sa Mdt::Sql::InsertStatement
   * \sa Mdt::Sql::InsertQuery
   */
  template<typename PrimaryKey, typename Struct>
  InsertStatement insertStatementFromReflected(const Struct & data)
  {
    static_assert( Mdt::Reflection::TypeTraits::IsPrimaryKeyClass<PrimaryKey>::value,
                   "PrimaryKey must be a primary key class for a reflected struct" );

    using struct_def = typename PrimaryKey::struct_def;
    static_assert( Mdt::Reflection::TypeTraits::IsStructDefAssociatedWithReflectedStruct<struct_def, Struct>::value ,
                   "PrimaryKey must be associated with given data of type Struct" );

    InsertStatement statement;
    Impl::AddValueToInsertStatement<PrimaryKey> f(statement);

    statement.setTableName( Mdt::Reflection::nameFromStructDefQString<struct_def>() );
    Mdt::Reflection::forEachFieldValuePairInStruct(data, f);

    return statement;
  }

}}} // namespace Mdt{ namespace Sql{ namespace Reflection{


#endif // #ifndef MDT_SQL_REFLECTION_INSERT_STATEMENT_H
