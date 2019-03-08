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
#ifndef MDT_SQL_STATEMENT_ALGRORITHM_H
#define MDT_SQL_STATEMENT_ALGRORITHM_H

#include "PrimaryKeyRecord.h"
#include "Mdt/QueryExpression/FilterExpression.h"
#include "MdtSql_CoreExport.h"
#include <QString>
#include <QVariant>
#include <QSqlDatabase>

namespace Mdt{ namespace Sql{

  /*! \brief Escape field name
   *
   * \pre \a db must be valid (must have a driver loaded)
   */
  QString MDT_SQL_CORE_EXPORT escapeFieldName(const QString & fieldName, const QSqlDatabase & db);

  /*! \brief Escape table name
   *
   * \pre \a db must be valid (must have a driver loaded)
   */
  QString MDT_SQL_CORE_EXPORT escapeTableName(const QString & tableName, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_SQL_CORE_EXPORT valueToSql(const QVariant & value, const QSqlDatabase & db);

  /*! \brief Get a filter expression from a primary key record
   *
   * \pre \a pkr must not be null
   */
  Mdt::QueryExpression::FilterExpression MDT_SQL_CORE_EXPORT filterExpressionFromPrimaryKeyRecord(const PrimaryKeyRecord & pkr);

  /*! \brief Get a filter expression from a primary key record
   *
   * \pre \a tableName must not be empty
   * \pre \a pkr must not be null
   */
  Mdt::QueryExpression::FilterExpression MDT_SQL_CORE_EXPORT filterExpressionFromPrimaryKeyRecord(const QString & tableName, const PrimaryKeyRecord & pkr);

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_STATEMENT_ALGRORITHM_H
