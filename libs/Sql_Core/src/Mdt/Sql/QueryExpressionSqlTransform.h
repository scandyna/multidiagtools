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
#ifndef MDT_SQL_QUERY_EXPRESSION_SQL_TRANSFORM_H
#define MDT_SQL_QUERY_EXPRESSION_SQL_TRANSFORM_H

#include "QueryEntity.h"
#include "QueryField.h"
#include "Mdt/QueryExpression/SelectField.h"
#include "Mdt/QueryExpression/SelectFieldList.h"
#include "Mdt/QueryExpression/EntityAndField.h"
#include "Mdt/QueryExpression/ComparisonOperator.h"
#include "Mdt/QueryExpression/LogicalOperator.h"
#include "Mdt/QueryExpression/LikeExpression.h"
#include "Mdt/QueryExpression/FilterExpression.h"
#include "Mdt/QueryExpression/JoinOperator.h"
#include "Mdt/QueryExpression/JoinConstraintExpression.h"
#include "Mdt/QueryExpression/JoinClause.h"
#include "Mdt/QueryExpression/JoinClauseList.h"
#include "Mdt/QueryExpression/SelectStatement.h"
#include "MdtSql_CoreExport.h"
#include <QString>
#include <QVariant>
#include <QSqlDatabase>

namespace Mdt{ namespace Sql{

  /*! \internal
   */
  QString MDT_SQL_CORE_EXPORT fieldToWhereSql(const Mdt::QueryExpression::EntityAndField & field, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_SQL_CORE_EXPORT queryFieldToSql(const QueryField & field, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_SQL_CORE_EXPORT selectFieldToSql(const Mdt::QueryExpression::SelectField & field, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_SQL_CORE_EXPORT selectFieldListToSql(const Mdt::QueryExpression::SelectFieldList & fieldList, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_SQL_CORE_EXPORT operatorToSql(Mdt::QueryExpression::ComparisonOperator op);

  /*! \internal
   */
  QString MDT_SQL_CORE_EXPORT operatorToSql(Mdt::QueryExpression::LogicalOperator op);

  /*! \internal
   */
  void MDT_SQL_CORE_EXPORT escapeLikeMetacharacters(QString & str);

  /*! \internal
   */
  QString MDT_SQL_CORE_EXPORT likeExpressionToSql(const Mdt::QueryExpression::LikeExpressionData & data, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_SQL_CORE_EXPORT filterExpressionToSql(const Mdt::QueryExpression::FilterExpression & expression, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_SQL_CORE_EXPORT joinOperatorToSql(Mdt::QueryExpression::JoinOperator op);

  /*! \internal
   */
  QString MDT_SQL_CORE_EXPORT joinConstraintExpressionToSql(const Mdt::QueryExpression::JoinConstraintExpression & expression, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_SQL_CORE_EXPORT joinClauseToSql(const Mdt::QueryExpression::JoinClause & joinClause, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_SQL_CORE_EXPORT joinClauseListToSql(const Mdt::QueryExpression::JoinClauseList & joinClauseList, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_SQL_CORE_EXPORT selectFromEntityToSql(const Mdt::QueryExpression::QueryEntity & entity, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_SQL_CORE_EXPORT selectStatementToSqlLimitSyntax(const Mdt::QueryExpression::SelectStatement & stm, int maxRows, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_SQL_CORE_EXPORT selectStatementToSqlTopSyntax(const Mdt::QueryExpression::SelectStatement & stm, int maxRows, const QSqlDatabase & db);

  /*! \internal
   */
  QString MDT_SQL_CORE_EXPORT selectStatementToSql(const Mdt::QueryExpression::SelectStatement & stm, int maxRows, const QSqlDatabase & db);

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_QUERY_EXPRESSION_SQL_TRANSFORM_H
