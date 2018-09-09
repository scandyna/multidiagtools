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
#ifndef MDT_QUERY_EXPRESSION_SQL_TRANSFORM_H
#define MDT_QUERY_EXPRESSION_SQL_TRANSFORM_H

#include "Mdt/QueryExpression/SelectField.h"
#include "Mdt/QueryExpression/SelectFieldList.h"
#include "Mdt/QueryExpression/SelectEntity.h"
#include "Mdt/QueryExpression/EntityAndField.h"
#include "Mdt/QueryExpression/ComparisonOperator.h"
#include "Mdt/QueryExpression/LogicalOperator.h"
#include "Mdt/QueryExpression/JoinConstraintExpression.h"
#include "Mdt/QueryExpression/JoinClause.h"
#include "Mdt/QueryExpression/JoinClauseList.h"
#include "Mdt/QueryExpression/LikeExpression.h"
#include "Mdt/QueryExpression/FilterExpression.h"
#include "Mdt/QueryExpression/SelectStatement.h"
#include "MdtQueryExpression_SqlExport.h"
#include <QString>
#include <QVariant>
#include <QSqlDatabase>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Escape field name
   *
   * \pre \a db must be valid (must have a driver loaded)
   */
  QString MDT_QUERYEXPRESSION_SQL_EXPORT escapeFieldName(const QString & fieldName, const QSqlDatabase & db);

  /*! \brief Escape table name
   *
   * \pre \a db must be valid (must have a driver loaded)
   */
  QString MDT_QUERYEXPRESSION_SQL_EXPORT escapeTableName(const QString & tableName, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_QUERYEXPRESSION_SQL_EXPORT valueToSql(const QVariant & value, const QSqlDatabase & db);

  /*! \internal
   */
  void MDT_QUERYEXPRESSION_SQL_EXPORT escapeLikeMetacharacters(QString & str);

  /*! \internal
   */
  QString MDT_QUERYEXPRESSION_SQL_EXPORT likeExpressionToSql(const LikeExpressionData & data, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_QUERYEXPRESSION_SQL_EXPORT selectFieldDeclarationToSql(const SelectField & field, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_QUERYEXPRESSION_SQL_EXPORT selectFieldListDeclarationToSql(const SelectFieldList & fieldList, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_QUERYEXPRESSION_SQL_EXPORT fieldConditionUsageToSql(const EntityAndField & field, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_QUERYEXPRESSION_SQL_EXPORT selectFromEntityToSql(const SelectEntity & entity, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_QUERYEXPRESSION_SQL_EXPORT operatorToSql(ComparisonOperator op);

  /*! \internal
   */
  QString MDT_QUERYEXPRESSION_SQL_EXPORT operatorToSql(LogicalOperator op);

  /*! \internal
   */
  QString MDT_QUERYEXPRESSION_SQL_EXPORT joinConstraintExpressionToSql(const JoinConstraintExpression & expression, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_QUERYEXPRESSION_SQL_EXPORT joinOperatorToSql(JoinOperator op);

  /*! \internal
   */
  QString MDT_QUERYEXPRESSION_SQL_EXPORT joinClauseToSql(const JoinClause & joinClause, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_QUERYEXPRESSION_SQL_EXPORT joinClauseListToSql(const JoinClauseList & joinClauseList, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_QUERYEXPRESSION_SQL_EXPORT filterExpressionToSql(const FilterExpression & expression, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_QUERYEXPRESSION_SQL_EXPORT selectStatementToSqlLimitSyntax(const SelectStatement & stm, int maxRows, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_QUERYEXPRESSION_SQL_EXPORT selectStatementToSqlTopSyntax(const SelectStatement & stm, int maxRows, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_QUERYEXPRESSION_SQL_EXPORT selectStatementToSql(const SelectStatement & stm, int maxRows, const QSqlDatabase & db);

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_SQL_TRANSFORM_H
