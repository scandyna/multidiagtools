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
#ifndef MDT_SQL_INSERT_STATEMENT_H
#define MDT_SQL_INSERT_STATEMENT_H

#include "FieldName.h"
#include "FieldNameValueMap.h"
#include "MdtSql_CoreExport.h"
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantList>
#include <QSqlDatabase>
#include <QMetaType>

namespace Mdt{ namespace Sql{

  /*! \brief Representation of a SQL INSERT statement
   *
   * \sa InsertQuery
   */
  class MDT_SQL_CORE_EXPORT InsertStatement
  {
   public:

    /*! \brief Construct a empty statement
     */
    InsertStatement() = default;

    /*! \brief Copy construct a statement from \a other
     */
    InsertStatement(const InsertStatement & other) = default;

    /*! \brief Copy assign \a other to this statement
     */
    InsertStatement & operator=(const InsertStatement & other) = default;

    /*! \brief Move construct a statement from \a other
     */
    InsertStatement(InsertStatement && other) = default;

    /*! \brief Move assign \a other to this statement
     */
    InsertStatement & operator=(InsertStatement && other) = default;

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

    /*! \brief Add a value for a specified field
     *
     * \pre \a fieldName must not be null
     * \pre \a fieldName must not allready exist in this statement
     */
    void addValue(const FieldName & fieldName, const QVariant & value);

    /*! \brief Check if this statement contains \a fieldName
     */
    bool containsFieldName(const QString & fieldName) const
    {
      return mFieldValueMap.containsFieldName(fieldName);
    }

    /*! \brief Clear this statement
     */
    void clear();

    /*! \brief Get a SQL INSERT statement for prepared query
     *
     * Will generate SQL of the form:
     * \code
     * INSERT INTO Person (Id_PK,Name) VALUES (?,?)
     * \endcode
     *
     * This can be used with QSqlQuery
     *
     * \todo Using prepared queries seems to be a obtimisation for some DBMS
     *    It should maybe not be removed
     *    Note that this is possibly a gain to add multiple records..
     *    And, maybe check about features of driver at runtime ??
     *
     * \sa InsertQuery
     * \pre \a db must be valid (must have a driver loaded)
     */
    QString toPrepareStatementSql(const QSqlDatabase & db) const;

    /*! \brief Get a list of field names this statement is containing
     *
     * \note The order of field names in the returned list
     *    is the result of consecutive calls to addValue() .
     */
    QStringList toFieldNameList() const
    {
      return mFieldValueMap.toFieldNameList();
    }

    /*! \brief Get a list of values this query is containing
     *
     * \note The order of values in the returned list
     *    is the result of consecutive calls to addValue() .
     */
    QVariantList toValueList() const
    {
      return mFieldValueMap.toValueList();
    }

   private:

    static QString escapeFieldName(const QString & fieldName, const QSqlDatabase & db);
    static QString escapeTableName(const QString & tableName, const QSqlDatabase & db);

    QString mTableName;
    FieldNameValueMap mFieldValueMap;
  };

}} // namespace Mdt{ namespace Sql{
Q_DECLARE_METATYPE(Mdt::Sql::InsertStatement)

#endif // #ifndef MDT_SQL_INSERT_STATEMENT_H
