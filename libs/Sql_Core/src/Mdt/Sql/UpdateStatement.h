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
#ifndef MDT_SQL_UPDATE_STATEMENT_H
#define MDT_SQL_UPDATE_STATEMENT_H

#include "FieldName.h"
#include "PrimaryKeyRecord.h"
#include "FieldNameValueMap.h"
#include "Mdt/QueryExpression/FilterExpression.h"
#include "Mdt/QueryExpression/TypeTraits/FilterExpression.h"
#include "MdtSql_CoreExport.h"
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantList>
#include <QMetaType>
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
     * \pre \a filter must be a filter expression
     */
    template<typename Expr>
    void setConditions(const Expr & filter)
    {
      static_assert(Mdt::QueryExpression::TypeTraits::isFilterExpression<Expr>() ,
                    "filter must be a valid filter expression");

      mConditionsFilter.setFilter(filter);
    }

    /*! \brief Set the conditions for this update statement
     *
     * \pre \a filter must not be null
     */
    void setConditionsFilterExpression(const Mdt::QueryExpression::FilterExpression & filter)
    {
      Q_ASSERT(!filter.isNull());

      mConditionsFilter = filter;
    }

    /*! \brief Set the conditions for this update statement
     *
     * \pre \a primaryKeyRecord must not be null
     */
    void setConditions(const PrimaryKeyRecord & primaryKeyRecord);

    /*! \brief Clear this statement
     */
    void clear();

    /*! \brief Get the SQL string from this statement
     *
     * Will generate SQL of the form:
     * \code
     * UPDATE Person
     * SET FirstName = "Some name", LastName = "Some last name"
     * WHERE Person.Id_PK = 21
     * \endcode
     *
     * The returned string can be executed from QSqlQuery
     *
     * \pre \a db must be valid (must have a driver loaded)
     * \sa UpdateQuery
     */
    QString toSql(const QSqlDatabase & db) const;

    /*! \brief Get a list of field names this statement is containing
     *
     * \note The order of field names in the returned list
     *    is the result of consecutive calls to addValue() .
     */
    QStringList toFieldNameList() const
    {
      return mFieldValueMap.toFieldNameList();
    }

    /*! \brief Get a list of values this statement is containing
     *
     * \note The order of values in the returned list
     *    is the result of consecutive calls to addValue() .
     */
    QVariantList toValueList() const
    {
      return mFieldValueMap.toValueList();
    }

    /*! \brief Access internal filter expression
     */
    const Mdt::QueryExpression::FilterExpression & internalConditionsFilterExpression() const noexcept
    {
      return mConditionsFilter;
    }

   private:

    QString mTableName;
    FieldNameValueMap mFieldValueMap;
    Mdt::QueryExpression::FilterExpression mConditionsFilter;
  };

}} // namespace Mdt{ namespace Sql{
Q_DECLARE_METATYPE(Mdt::Sql::UpdateStatement)

#endif // #ifndef MDT_SQL_UPDATE_STATEMENT_H
