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
#ifndef MDT_SQL_UPDATE_STATEMENT_H
#define MDT_SQL_UPDATE_STATEMENT_H

#include "FieldName.h"
#include "PrimaryKeyRecord.h"
#include "FieldNameValueMap.h"
#include "MdtSql_CoreExport.h"
#include <QString>
#include <QVariant>
#include <QSqlDatabase>

namespace Mdt{ namespace Sql{

  /*! \brief Representation of a SQL UPDATE statement
   *
   * \sa UpdateQuery
   */
  class MDT_SQL_CORE_EXPORT UpdateStatement
  {
   public:

    /*! \brief Set table name
     *
     * \pre \a name must not be empty
     */
    void setTableName(const QString & name);

    /*! \brief Get table name
     */
    QString tableName() const
    {
      return mTableName;
    }

    /*! \brief Add a value to this statement
     *
     * \pre \a fieldName must not be null
     * \pre \a fieldName must not allready exist in this statement
     */
    void addValue(const FieldName & fieldName, const QVariant & value);

    /*! \brief Set the conditions for this update statement
     *
     * \pre \a primaryKeyRecord must not be null
     */
    void setConditions(const PrimaryKeyRecord & primaryKeyRecord);

    /*! \brief Clear this statement
     */
    void clear();

    /*! \brief Get a SQL UPDATE statement for prepared query
     *
     * Will generate SQL of the form:
     * \code
     * UPDATE Person
     * SET FirstName = ?, LastName = ?
     * WHERE Person.Id_PK = ?
     * \endcode
     *
     * This can be used with QSqlQuery
     *
     * \sa UpdateQuery
     * \pre \a db must be valid (must have a driver loaded)
     */
    QString toPrepareStatementSql(const QSqlDatabase & db) const;

    /*! \brief Get a list of values this query is containing
     *
     * \note The order of values in the returned list
     *    is the result of consecutive calls to addValue() .
     */
    QVariantList toValueList() const
    {
      return mFieldValueMap.toValueList();
    }

    /*! \brief Get a list of values for the conditions this query is containing
     */
    QVariantList toConditionsValueList() const
    {
      return mPkrConditions.toValueList();
    }

   private:

    static QString escapeFieldName(const QString & fieldName, const QSqlDatabase & db);
    static QString escapeTableName(const QString & tableName, const QSqlDatabase & db);

    QString mTableName;
    FieldNameValueMap mFieldValueMap;
    PrimaryKeyRecord mPkrConditions;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_UPDATE_STATEMENT_H
