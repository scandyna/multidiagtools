/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_QUERY_EXPRESSION_SQL_SELECT_QUERY_H
#define MDT_QUERY_EXPRESSION_SQL_SELECT_QUERY_H

// #include "AbstractQuery.h"
#include "Mdt/QueryExpression/SelectQuery.h"
#include "Mdt/Sql/AbstractSelectQuery.h"
#include "MdtQueryExpression_SqlExport.h"
#include <QSqlDatabase>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Execute a query to fetch data from a SQL database
   *
   * It is also possible to execute a Mdt::QueryExpression::SelectQuery:
   * Example to create a storage independent query and then execute it to fetch data from a SQL database:
   * \code
   * using namespace Mdt::QueryExpression;
   *
   * SelectField name( FieldName("name") );
   * SelectField age( FieldName("age"), "A" );
   *
   * SelectQuery query;
   * query.setEntityName("Person");
   * query.addField(name);
   * query.addField(age);
   * query.addField( FieldName("remarks") );
   * query.setFilter( (name == "A") && (age > 29) );
   *
   * QSqlDatabase db; // To setup db, see Qt and Mdt::Sql documentation
   * SqlSelectQuery sqlQuery(db);
   * if(!sqlQuery.exec(query)){
   *   // Error handling. sqlQuery.lastError() constains a error description.
   * }
   * \endcode
   */
  class MDT_QUERYEXPRESSION_SQL_EXPORT SqlSelectQuery : public Mdt::Sql::AbstractSelectQuery
  {
   Q_OBJECT

   public:

    /*! \brief Construct a select query that acts on db
     *
     * \pre \a db must be valid (must have a driver loaded)
     */
    SqlSelectQuery(const QSqlDatabase & db);

    /*! \brief Construct a select query that acts on db
     *
     * \pre \a db must be valid (must have a driver loaded)
     */
    SqlSelectQuery(QObject *parent, const QSqlDatabase & db);

    /*! \brief Execute a Mdt::QueryExpression::SelectQuery
     */
    bool exec(const SelectQuery & query);

   private:

    
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_SQL_SELECT_QUERY_H
