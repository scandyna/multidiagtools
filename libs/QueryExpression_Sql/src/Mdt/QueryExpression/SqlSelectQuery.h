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

#include "Mdt/QueryExpression/AbstractSelectQuery.h"
#include "MdtQueryExpression_SqlExport.h"
#include <QSqlDatabase>
#include <QSqlQuery>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Execute a select statement to fetch data from a SQL database
   *
   * It is also possible to execute a Mdt::QueryExpression::SelectStatement:
   * Example to create a storage independent statement and then execute it to fetch data from a SQL database:
   * \code
   * using namespace Mdt::QueryExpression;
   *
   * SelectField name( FieldName("name") );
   * SelectField age( FieldName("age"), "A" );
   *
   * SelectStatement stm;
   * stm.setEntityName("Person");
   * stm.addField(name);
   * stm.addField(age);
   * stm.addField( FieldName("remarks") );
   * stm.setFilter( (name == "A") && (age > 29) );
   *
   * QSqlDatabase db; // To setup db, see Qt and Mdt::Sql documentation
   * auto selectQuery = SelectQuery::make<SqlSelectQuery>();
   * auto sqlSelectQuery = selectQuery.impl<SqlSelectQuery>();
   * sqlSelectQuery.setDatabase(db);
   * if(!selectQuery.exec(stm)){
   *   // Error handling. selectQuery.lastError() constains a error description.
   * }
   * \endcode
   */
  class MDT_QUERYEXPRESSION_SQL_EXPORT SqlSelectQuery : public Mdt::QueryExpression::AbstractSelectQuery
  {
   Q_OBJECT

   public:

//     /*! \brief Construct a select query that acts on db
//      *
//      * \pre \a db must be valid (must have a driver loaded)
//      */
//     SqlSelectQuery(const QSqlDatabase & db);

//     /*! \brief Construct a select query that acts on db
//      *
//      * \pre \a db must be valid (must have a driver loaded)
//      */
//     SqlSelectQuery(QObject *parent, const QSqlDatabase & db);

    /*! \brief Construct a select query
     *
     * The application default database connection will be used.
     *  To use a specific database connection, use setDatabase().
     *
     * \sa QSqlDatabase
     */
    SqlSelectQuery(QObject *parent = nullptr);

    /*! \brief Set the database connection to \a db
     *
     * \pre \a db must be valid (must have a driver loaded)
     */
    void setDatabase(const QSqlDatabase & db);

    /*! \brief Execute a select statement
     */
    bool exec(const SelectStatement & statement) override;

    /*! \brief Get the next record, if avaliable, and position this query to that record
     */
    bool next() override;

    /*! \brief Get count of field for the last executed statement
     */
    int fieldCount() const override;

    /*! \brief Get the field index of \a field
     *
     * Returns the index of \a field if it exists,
     *  otherwise -1
     *  If more than 1 field matches, the first one is returned.
     *
     * \pre \a field must contain a EntityAndField
     */
    int fieldIndex(const EntityAndField & field) const override;

    /*! \brief Get the value for \a fieldIndex from the current record
     *
     * \pre \a fieldIndex must be in a valid range ( 0 <= \a fieldIndex < fieldCount() ).
     */
    QVariant value(int fieldIndex) const override;

   private:

    QSqlQuery mQuery;
    QSqlDatabase mDb;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_SQL_SELECT_QUERY_H
