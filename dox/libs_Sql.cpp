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

/*! \page libs_Sql Sql library
 *
 * 
 *
 * \section sql_schema SQL Schema
 *
 * \section sql_queries SQL queries
 *
 * In Mdt Sql the notion of statement and query are separated.
 *
 * The statement, that is copiable, holds the informations,
 *  and can generate a SQL statement string.
 *
 * Here is a example of a insert statement:
 * \code
 * #include <Mdt/Sql/InsertStatement.h>
 *
 * using namespace Mdt::Sql;
 *
 * InsertStatement statement;
 * statement.setTableName("Person_tbl");
 * statement.addValue( FieldName("FirstName"), "First name 1" );
 * statement.addValue( FieldName("LastName"), "Last name 1" );
 * \endcode
 *
 * The query will use the statement to excute the query:
 * \code
 * #include <Mdt/Sql/InsertStatement.h>
 * #include <Mdt/Sql/InsertQuery.h>
 * #include <QSqlDatabase>
 *
 * using namespace Mdt::Sql;
 *
 * InsertStatement statement;
 * // Setup statement here
 *
 * QSqlDatabase dbConnection;
 * // To setup dbConnection see QSqlDatabase documentation
 *
 * InsertQuery query(dbConnection);
 * if(!query.execStatement(statement)){
 *   // Error handling. query.lastError() constains a error description.
 * }
 * \endcode
 *
 * Internally, QSqlQuery is used, and Mdt Sql helps to avoid
 *  some boilerplate code for the common queries.
 *
 * It is not mandatory to use the statement,
 *  a query can be used directly:
 * \code
 * #include <Mdt/Sql/InsertQuery.h>
 * #include <QSqlDatabase>
 *
 * using namespace Mdt::Sql;
 *
 * QSqlDatabase dbConnection;
 * // To setup dbConnection see QSqlDatabase documentation
 *
 * InsertQuery query(dbConnection);
 * query.setTableName("Person_tbl");
 * query.addValue( FieldName("FirstName"), "First name 1" );
 * query.addValue( FieldName("LastName"), "Last name 1" );
 * if(!query.exec()){
 *   // Error handling. query.lastError() constains a error description.
 * }
 * \endcode
 *
 * To insert elements into a table,
 *  a list of field value pairs is provided to the query.
 *
 * Other type of queries, like delete, update and select, will need
 *  some conditions to limit the actions:
 * \code
 * #include <Mdt/Sql/DeleteStatement.h>
 *
 * using namespace Mdt::Sql;
 *
 * DeleteStatement statement;
 * statement.setTableName("Person_tbl");
 * statement.setConditions( ?? ); // How do we specify the conditions ?
 * \endcode
 *
 * The above example opens the concept of filter expression.
 *
 * \subsection sql_filter_expression Filter expression
 *
 * To express somewhat arbitrary conditions,
 *  the \ref libs_QueryExpression library is used.
 *
 * The above delete statement example could become:
 * \code
 * #include <Mdt/Sql/DeleteStatement.h>
 *
 * using namespace Mdt::Sql;
 *
 * QueryField id("Id_PK");
 *
 * DeleteStatement statement;
 * statement.setTableName("Person_tbl");
 * statement.setConditions( id == 21 );
 * \endcode
 *
 * Notice that Mdt::Sql::QueryField is a type alias to Mdt::QueryExpression::QueryField.
 *  The more explicit code will look like:
 * \code
 * #include <Mdt/Sql/DeleteStatement.h>
 * #include <Mdt/QueryExpression/QueryField.h>
 *
 * using namespace Mdt::Sql;
 * using Mdt::QueryExpression::QueryField;
 *
 * QueryField id("Id_PK");
 *
 * DeleteStatement statement;
 * statement.setTableName("Person_tbl");
 * statement.setConditions( id == 21 );
 * \endcode
 *
 * If the \ref libs_Reflection is used,
 *  the delete statement could become:
 * \code
 * #include "PersonDataStruct.h"
 * #include <Mdt/Sql/ReflectionDeleteStatement.h>
 *
 * using namespace Mdt::Sql;
 *
 * ReflectionQueryField<PersonDef::id> id;
 *
 * ReflectionDeleteStatement<PersonDef> statement;
 * statement.setConditions( id == 21 );
 * \endcode
 *
 * \section sql_storage_table SQL storage table
 *
 * A storage table can be used to implement a repository.
 *
 * Here is a example of a repository declaration:
 * \code
 * #include "PersonRepository.h"
 * #include "PersonDataStruct.h"
 * #include <Mdt/Expected.h>
 * #include <Mdt/Sql/ReflectionStorageTableWithId.h>
 *
 * class SqlPersonRepository : public PersonRepository
 * {
 *  public:
 *
 *   SqlPersonRepository(const QSqlDatabase & db);
 *   Mdt::Expected<PersonId> add(const Person & person) override;
 *   Mdt::Expected<Person> get(PersonId id) const override;
 *   bool update(const Person & person) override;
 *   bool remove(PersonId id) override;
 *   bool removeAll() override;
 *
 *  private:
 *
 *   Mdt::Sql::ReflectionStorageTableWithId<PersonDef, PersonPrimaryKey, PersonId::value_type> mTable;
 * };
 * \endcode
 *
 * The implementation could look like:
 * \code
 * #include "SqlPersonRepository.h"
 * #include "Person_p.h"
 *
 * SqlPersonRepository::SqlPersonRepository(const QSqlDatabase & db)
 *  : mTable(db)
 * {
 * }
 *
 * Mdt::Expected<PersonId> SqlPersonRepository::add(const Person & person)
 * {
 *   const auto id = mTable.add( personPrivateConstDataStruct(person) );
 *   if(!id){
 *     // Maybe generate a more domain specific error message here
 *     return id.error();
 *   }
 *   return PersonId(*id);
 * }
 *
 * Mdt::Expected<Person> SqlPersonRepository::get(PersonId id) const
 * {
 *   const auto personData = mTable.get( id.value() );
 *   if(!personData){
 *     // Maybe generate a more domain specific error message here
 *     return personData.error();
 *   }
 *   return personFromDataStruct(*personData);
 * }
 *
 * bool SqlPersonRepository::update(const Person & person)
 * {
 *   bool ok = mTable.update( personPrivateConstDataStruct(person) );
 *   if(!ok){
 *     // Maybe generate a more domain specific error message here
 *     setLastError(mTable.lastError());
 *     return false;
 *   }
 *   return true;
 * }
 *
 * bool SqlPersonRepository::remove(PersonId id)
 * {
 *   bool ok = mTable.remove( id.value() );
 *   if(!ok){
 *     // Maybe generate a more domain specific error message here
 *     setLastError(mTable.lastError());
 *     return false;
 *   }
 *   return true;
 * }
 *
 * bool SqlPersonRepository::removeAll()
 * {
 *   bool ok = mTable.removeAll();
 *   if(!ok){
 *     // Maybe generate a more domain specific error message here
 *     setLastError(mTable.lastError());
 *     return false;
 *   }
 *   return true;
 * }
 * \endcode
 *
 * \sa Mdt::Sql::ReflectionStorageTableWithId
 *
 * Lets take a example with a entity having a composed id.
 *
 * Here is a example of a repository declaration:
 * \code
 * #include "CableLinkRepository.h"
 * #include "CableLinkDataStruct.h"
 * #include <Mdt/Expected.h>
 * #include <Mdt/Sql/ReflectionStorageTable.h>
 *
 * class SqlCableLinkRepository : public CableLinkRepository
 * {
 *  public:
 *
 *   SqlCableLinkRepository(const QSqlDatabase & db);
 *   Mdt::Expected<CableLinkId> add(const CableLink & link) override;
 *   Mdt::Expected<CableLink> get(CableLinkId id) const override;
 *   bool update(const CableLink & link) override;
 *   bool remove(CableLinkId id) override;
 *   bool removeAll() override;
 *
 *  private:
 *
 *   Mdt::Sql::ReflectionStorageTable<CableLinkDef, CableLinkPrimaryKey, CableLinkIdRecord> mTable;
 * };
 * \endcode
 *
 * The implementation could look like:
 * \code
 * #include "SqlCableLinkRepository.h"
 * #include "CableLink_p.h"
 *
 * SqlCableLinkRepository::SqlCableLinkRepository(const QSqlDatabase & db)
 *  : mTable(db)
 * {
 * }
 *
 * Mdt::Expected<CableLinkId> SqlCableLinkRepository::add(const CableLink & link)
 * {
 *   const auto id = mTable.add( cableLinkPrivateConstDataStruct(link) );
 *   if(!id){
 *     // Maybe generate a more domain specific error message here
 *     return id.error();
 *   }
 *   return CableLinkId::fromRecord(*id);
 * }
 *
 * Mdt::Expected<CableLink> SqlCableLinkRepository::get(CableLinkId id) const
 * {
 *   const auto linkData = mTable.get( id.record() );
 *   if(!linkData){
 *     // Maybe generate a more domain specific error message here
 *     return linkData.error();
 *   }
 *   return cableLinkFromDataStruct(*linkData);
 * }
 *
 * bool SqlCableLinkRepository::update(const CableLink & link)
 * {
 *   bool ok = mTable.update( cableLinkPrivateConstDataStruct(person) );
 *   if(!ok){
 *     // Maybe generate a more domain specific error message here
 *     setLastError(mTable.lastError());
 *     return false;
 *   }
 *   return true;
 * }
 *
 * bool SqlCableLinkRepository::remove(CableLinkId id)
 * {
 *   bool ok = mTable.remove( id.record() );
 *   if(!ok){
 *     // Maybe generate a more domain specific error message here
 *     setLastError(mTable.lastError());
 *     return false;
 *   }
 *   return true;
 * }
 *
 * bool SqlCableLinkRepository::removeAll()
 * {
 *   bool ok = mTable.removeAll();
 *   if(!ok){
 *     // Maybe generate a more domain specific error message here
 *     setLastError(mTable.lastError());
 *     return false;
 *   }
 *   return true;
 * }
 * \endcode
 *
 * \sa Mdt::Sql::ReflectionStorageTable
 */
