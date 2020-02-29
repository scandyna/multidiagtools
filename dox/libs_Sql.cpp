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
 * This library is based on top of Qt SQL, and provides some helpers around it.
 *
 * \section sql_connectionAndSetup Connection and setup
 *
 * The first step is to get a handle to a database:
 * \code
 * QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
 * \endcode
 *
 * here db is the handle to the database connection, and can be used:
 * \code
 * QSqlQuery query(db);
 * \endcode
 *
 * As stated in the Qt documentation, a instance of QSqlDatabase
 *  should not be keeped in objects as member.
 *
 * This is why the concept of a connection was made:
 * \code
 * #include <Mdt/Sql/Connection.h>
 *
 * using namespace Mdt::Sql;
 *
 * Connection connection("ConnectionIdName");
 * \endcode
 *
 * the connection only wraps the name of a QSqlDatabase connection.
 *
 * Here is a example to get a handle to SQLite and open a database:
 * \code
 * #include "MyAppSqlSchema.h"
 * #include <Mdt/Sql/SQLiteConnectionParameters.h>
 * #include <Mdt/Sql/SQLiteDatabase.h>
 * #include <Mdt/Sql/Connection.h>
 *
 * using namespace Mdt::Sql;
 *
 * SQLiteConnectionParameters parameters;
 * parameters.setDatabaseFile("/path/to/db.sqlite");
 *
 * const Mdt::Expected<Connection> connection = SQLiteDatabase::addConnection();
 * if(!connection){
 *   // Error: probably no SQLite driver could be loaded
 * }
 *
 * MyAppSqlSchema sqlSchema;
 * SQLiteDatabase db(*connection, parameters);
 * if(!db.openExisting()){
 *   // Error
 * }
 * checkOrCreateSqlSchema(*connection);
 * \endcode
 *
 * A QSqlDatabase can be retrived from the connection:
 * \code
 * QSqlDatabase db = connection->database();
 * \endcode
 *
 * This is similar to:
 * \code
 * QSqlDatabase db = QSqlDatabase::database(connection->name(), false);
 * \endcode
 *
 * \sa Mdt::Sql::Connection
 *
 * The connection described above can be used for the blocking API.
 *
 * A asynchronous API will be described in a later section.
 *  Here is a example to create a async query connection to a SQLite database:
 * \code
 * #include <Mdt/Sql/SQLiteAsyncQueryConnection.h>
 * #include <memory>
 *
 * using namespace Mdt::Sql;
 *
 * SQLiteConnectionParameters parameters;
 * parameters.setDatabaseFile("/path/to/db.sqlite");
 *
 * auto connection = std::make_shared<SQLiteAsyncQueryConnection>();
 * if( !connection->open(parameters) ){
 *   // Error, use connection->lastError()
 * }
 * checkOrCreateSqlSchema(connection);
 * \endcode
 *
 * \sa \ref sql_async_queries
 * \sa Mdt::Sql::AsyncQueryConnection
 *
 * \section sql_schema SQL Schema
 *
 * \subsection sql_schema_model Create a SQL schema model
 *
 * \sa Mdt::Sql::Schema::Table
 *
 * \subsection sql_schema_database Create a SQL schema on the database
 *
 * \code
 * #include <Mdt/Sql/Schema/Driver.h>
 *
 * // See above to establish a connection to a database
 * const Mdt::Sql::Connection connection = ...
 *
 * Mdt::Sql::Schema::Driver schemaDriver(connection);
 * \endcode
 *
 * Creating a database schema using the asynchronous query API
 *  is currently not supported.
 *  To be confident about thread safety, the schema should be created using the blocking API,
 *  then the connection closed before initializing the asynchronous API:
 * \code
 * // See above to establish a connection to a database
 * const Mdt::Sql::Connection schemaConnection = ...
 *
 * Mdt::Sql::Schema::Driver schemaDriver(schemaConnection);
 * // Create all the tables here
 *
 * // Close and remove
 * {
 *   QSqlDatabase db = schemaConnection.database();
 *   db.close();
 * }
 * QSqlDatabase::removeDatabase(schemaConnection.name());
 *
 * // Setup the connection using the asynchronous API
 * auto connection = std::make_shared<AsyncQueryConnection>();
 * \endcode
 *
 * \sa Mdt::Sql::Schema::Driver
 *
 * \section sql_queries SQL queries
 *
 * \todo Queries should not take QSqlDatabase objects as member,
 *     but Connection. See QSqlDatabase doc.
 *
 * In %Mdt Sql the notion of statement and query are separated.
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
 * The query can execute a statement:
 * \code
 * #include <Mdt/Sql/InsertStatement.h>
 * #include <Mdt/Sql/InsertQuery.h>
 * #include <Mdt/Sql/Connection>
 *
 * using namespace Mdt::Sql;
 *
 * InsertStatement statement;
 * // Setup statement here
 *
 * const auto connection = ...
 * // To get a connection, see above
 *
 * InsertQuery query(connection);
 * if(!query.execStatement(statement)){
 *   // Error handling. query.lastError() constains a error description.
 * }
 * \endcode
 *
 * Internally, QSqlQuery is used, and %Mdt Sql helps to avoid
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
 * const auto connection = ...
 * // To get a connection, see above
 *
 * InsertQuery query(connection);
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
 *
 * The more explicit code will look like:
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
 * \sa Mdt::Sql::InsertStatement
 * \sa Mdt::Sql::InsertQuery
 * \sa Mdt::Sql::UpdateStatement
 * \sa Mdt::Sql::UpdateQuery
 * \sa Mdt::Sql::DeleteStatement
 * \sa Mdt::Sql::DeleteQuery
 * \sa Mdt::QueryExpression::SelectStatement
 * \sa Mdt::Sql::SelectQuery
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
 * \section sql_async_queries Use SQL async queries
 *
 * %Async queries are executed in a separate thread.
 *
 * As explained in the Qt documentation, a connection should only be used by the thread that created it.
 *  When using async queries, it is possible (and probably recomanded) to access the same database
 *  from one thread. Because of this, creating a thread for each query instance is not a option.
 *
 * Before using a async query, a async connection must be created:
 * \code
 * #include <Mdt/Sql/SQLiteAsyncQueryConnection.h>
 * #include <Mdt/Sql/SQLiteConnectionParameters.h>
 *
 * using namespace Mdt::Sql;
 *
 * SQLiteConnectionParameters parameters;
 * parameters.setDatabaseFile("/path/to/dbFile.sql");
 *
 * auto connection = std::make_shared<SQLiteAsyncQueryConnection>();
 * if(!connection->open(parameters)){
 *   // error handling
 * }
 * \endcode
 *
 * \sa Mdt::Sql::SQLiteAsyncQueryConnection
 * \sa Mdt::Sql::AsyncQueryConnection
 *
 * A way to use a async query is Qt signal/slot:
 * \code
 * #include "MyWidget.h"
 * #include <Mdt/Sql/SQLiteAsyncQueryConnection.h>
 * #include <Mdt/Sql/AsyncInsertQuery.h>
 * #include <Mdt/Sql/InsertStatement.h>
 *
 * using namespace Mdt::Sql;
 *
 * auto connection = std::make_shared<SQLiteAsyncQueryConnection>();
 * // Setup connection here
 *
 * MyWidget widget;
 * AsyncInsertQuery query(connection);
 * QObject::connect(&widget, &MyWidget::addPersonRequested, &query, &AsyncInsertQuery::submitStatement);
 * QObject::connect(&query, &AsyncInsertQuery::newIdInserted, &widget, &MyWidget::updateNewPerson);
 * QObject::connect(&query, &AsyncInsertQuery::done, &widget, &MyWidget::queryDone);
 * QObject::connect(&query, &AsyncInsertQuery::errorOccurred, &widget, &MyWidget::handleQueryError);
 * \endcode
 *
 * %Async queries can also be used to execute statements synchronously.
 *  This can look strange, but provides some advantages.
 *  At first, working with synchronous and asynchronous queries
 *  can be done without having concurrent threads accessing the same database connection.
 *  Then, using the synchronous calls of those queries will not block the callers Qt event loop.
 *  On the other hand, using the synchronous calls will introduce some latency (order of 100 microseconds per call).
 *
 * Here is a example of a insert query using synchronous calls:
 * \code
 * #include <Mdt/Sql/SQLiteAsyncQueryConnection>
 * #include <Mdt/Sql/AsyncInsertQuery.h>
 * #include <Mdt/Sql/InsertStatement.h>
 *
 * using namespace Mdt::Sql;
 *
 * auto connection = std::make_shared<SQLiteAsyncQueryConnection>();
 * // Setup connection here
 *
 * InsertStatement statement;
 * // Setup statement here
 *
 * AsyncInsertQuery query(connection);
 * if( !query.execStatement(statement) ){
 *   handleError( query.lastError() );
 *   return ...
 * }
 * useNewId( query.lastInsertId() );
 * \endcode
 *
 * \sa Mdt::Sql::InsertStatement
 * \sa Mdt::Sql::AsyncInsertQuery
 * \sa Mdt::QueryExpression::SelectStatement
 * \sa Mdt::Sql::AsyncSelectQuery
 * \sa Mdt::Sql::UpdateStatement
 * \sa Mdt::Sql::AsyncUpdateQuery
 * \sa Mdt::Sql::DeleteStatement
 * \sa Mdt::Sql::AsyncDeleteQuery
 *
 * \section sql_storage_table SQL storage table
 *
 * A storage table can be used to implement a repository.
 *
 * \sa \ref example_PersonRepository
 * \sa \ref example_CableLinkRepository
 */
