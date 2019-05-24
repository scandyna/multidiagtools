/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_SQL_SELECT_QUERY_H
#define MDT_SQL_SELECT_QUERY_H

#include "AbstractQuery.h"
#include "Mdt/QueryExpression/SelectStatement.h"
#include "MdtSql_CoreExport.h"
#include <QVariant>
#include <QSqlQuery>
#include <QSqlRecord>

namespace Mdt{ namespace Sql{

  /*! \brief Select data from a database
   *
   * Example to execute a select statement:
   * \code
   * #include <Mdt/QueryExpression/SelectStatement.h>
   * #include <Mdt/Sql/SelectQuery.h>
   * #include <Mdt/Sql/Connection.h>
   *
   * using namespace Mdt::QueryExpression;
   *
   * SelectStatement stm;
   * stm.setEntityName("Person");
   * stm.addField("id");
   * stm.addField("name");
   *
   * Mdt::Sql::Connection connection = ... // See Mdt::Sql::Connection to setup db handle and get a connection to it
   * Mdt::Sql::SelectQuery query(connection);
   *
   * if(!query.execStatement(stm)){
   *   // Error handling. query.lastError() constains a error description.
   * }
   *
   * while(query.next()){
   *   const QString name = query.value(1).toString();
   * }
   * \endcode
   *
   * To get a record for a primary key, the code could look like:
   * \code
   * #include <Mdt/QueryExpression/SelectStatement.h>
   * #include <Mdt/Sql/SelectQuery.h>
   * #include <Mdt/Sql/Connection.h>
   *
   * using namespace Mdt::QueryExpression;
   *
   * QueryField id("id");
   *
   * SelectStatement stm;
   * stm.setEntityName("Person");
   * stm.addField("id");
   * stm.addField("name");
   * stm.setFilter(id == 25);
   *
   * Mdt::Sql::Connection connection = ... // See Mdt::Sql::Connection to setup db handle and get a connection to it
   * Mdt::Sql::SelectQuery query(connection);
   *
   * if(!query.execStatement(stm)){
   *   // Error handling. query.lastError() constains a error description.
   * }
   *
   * const QSqlRecord record = query.fetchSingleRecord();
   * if(record.isEmpty()){
   *   // Error handling. query.lastError() constains a error description.
   * }
   *
   * const QString name = record.value(1).toString();
   * \endcode
   *
   * Note that SelectQuery uses QSqlQuery internally.
   *  If SelectStatement can not handle some query, and a raw SQL string must be executed,
   *  consider using QSqlQuery directly.
   *
   * \sa Mdt::QueryExpression::SelectStatement
   */
  class MDT_SQL_CORE_EXPORT SelectQuery : public AbstractQuery
  {
   Q_OBJECT

   public:

    /*! \brief Construct a select query that acts on connection
     */
    explicit SelectQuery(const Connection & connection, QObject *parent = nullptr);

    /*! \brief Execute a select statement
     *
     * If \a maxRows is > 0, the count of records for the result will be limited to its value
     */
    bool execStatement(const Mdt::QueryExpression::SelectStatement & statement, int maxRows = 0);

    /*! \brief Get the count of fields the result of this query contains
     *
     * If not query have been executed, 0 is returned
     */
    int fieldCount() const;

    /*! \brief Retrieves the next record in the result, if available, and positions the query on the retrieved record
     *
     * This is a call to QSqlQuery::next()
     */
    bool next()
    {
      return mQuery.next();
    }

    /*! \brief Retrieves a single record in the result
     *
     * If no record is available,
     *  this method stores a error containing Mdt::ErrorCode::NotFound and returns a empty record.
     *
     * If more than 1 record is available,
     *  this method stores a error and returns a empty record.
     *
     * \warning After a call to this method,
     *   this query no longer refers to the expected record.
     *
     * Example of usage:
     * \code
     * const QSqlRecord record = query.fetchSingleRecord();
     * if(record.isEmpty()){
     *   if(query.lastError().isError(Mdt::ErrorCode::NotFound)){
     *     // No record available
     *   }else{
     *     // More than one record available
     *   }
     * }
     * \endcode
     */
    QSqlRecord fetchSingleRecord();

    /*! \brief Returns the value of field index in the current record
     *
     * This is a call to QSqlQuery::value()
     */
    QVariant value(int fieldIndex) const
    {
      return mQuery.value(fieldIndex);
    }

    /*! \brief Returns the current record
     *
     * This is a call to QSqlQuery::record()
     */
    QSqlRecord record() const
    {
      return mQuery.record();
    }

   private:

    QSqlQuery mQuery;
    Mdt::QueryExpression::SelectStatement mStatement;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_SELECT_QUERY_H
