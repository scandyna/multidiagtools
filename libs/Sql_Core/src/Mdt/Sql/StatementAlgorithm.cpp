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
#include "StatementAlgorithm.h"
#include "QueryEntity.h"
#include "Mdt/QueryExpression/ExpressionTree.h"
// #include "Mdt/Algorithm.h"
#include <QStringBuilder>
#include <QLatin1Char>
#include <QLatin1String>
#include <QSqlField>
#include <QSqlDriver>
// #include <boost/variant.hpp>

namespace Mdt{ namespace Sql{

QString escapeFieldName(const QString& fieldName, const QSqlDatabase& db)
{
  Q_ASSERT(db.isValid());
  Q_ASSERT(db.driver() != nullptr);

  return db.driver()->escapeIdentifier(fieldName, QSqlDriver::FieldName);
}

QString escapeTableName(const QString& tableName, const QSqlDatabase& db)
{
  Q_ASSERT(db.isValid());
  Q_ASSERT(db.driver() != nullptr);

  return db.driver()->escapeIdentifier(tableName, QSqlDriver::TableName);
}

QString valueToSql(const QVariant& value, const QSqlDatabase& db)
{
  Q_ASSERT(db.isValid());
  Q_ASSERT(db.driver() != nullptr);

  QSqlField field(QString(), value.type());
  field.setValue(value);

  return db.driver()->formatValue(field);
}

Mdt::QueryExpression::FilterExpression filterExpressionFromPrimaryKeyRecord(const PrimaryKeyRecord & pkr)
{
  Q_ASSERT(!pkr.isNull());

  using Mdt::QueryExpression::EntityAndField;
  using Mdt::QueryExpression::ComparisonOperator;
  using Mdt::QueryExpression::LogicalOperator;

  Mdt::QueryExpression::FilterExpression filter;
  Mdt::QueryExpression::ExpressionTree tree;
  Mdt::QueryExpression::ExpressionTreeVertex leftVertex;
  Mdt::QueryExpression::ExpressionTreeVertex rightVertex;

  /*
   *              (AND)
   *             /     \
   *         (AND)      (==)
   *        /    \      /  \
   *     (==)     (==) (FC) (c)
   *     /  \     /  \
   *  (FA)  (a) (FB) (b)
   */
  const int n = pkr.fieldCount();
  Q_ASSERT(n > 0);
  leftVertex = tree.addNode( EntityAndField(pkr.fieldNameAt(0)), ComparisonOperator::Equal, pkr.valueAt(0) );
  for(int i=1; i < n; ++i){
    rightVertex = tree.addNode( EntityAndField(pkr.fieldNameAt(i)), ComparisonOperator::Equal, pkr.valueAt(i) );
    leftVertex = tree.addNode(leftVertex, LogicalOperator::And, rightVertex);
  }
  filter.setFilterExpressionTree(tree);

  return filter;
}

Mdt::QueryExpression::FilterExpression filterExpressionFromPrimaryKeyRecord(const QString & tableName, const PrimaryKeyRecord & pkr)
{
  Q_ASSERT(!tableName.trimmed().isEmpty());
  Q_ASSERT(!pkr.isNull());

  using Mdt::QueryExpression::EntityAndField;
  using Mdt::QueryExpression::ComparisonOperator;
  using Mdt::QueryExpression::LogicalOperator;

  Mdt::QueryExpression::FilterExpression filter;
  Mdt::QueryExpression::ExpressionTree tree;
  Mdt::QueryExpression::ExpressionTreeVertex leftVertex;
  Mdt::QueryExpression::ExpressionTreeVertex rightVertex;

  /*
   *              (AND)
   *             /     \
   *         (AND)      (==)
   *        /    \      /  \
   *     (==)     (==) (FC) (c)
   *     /  \     /  \
   *  (FA)  (a) (FB) (b)
   */
  QueryEntity entity(tableName);
  const int n = pkr.fieldCount();
  Q_ASSERT(n > 0);
  leftVertex = tree.addNode( EntityAndField(entity, pkr.fieldNameAt(0)), ComparisonOperator::Equal, pkr.valueAt(0) );
  for(int i=1; i < n; ++i){
    rightVertex = tree.addNode( EntityAndField(entity, pkr.fieldNameAt(i)), ComparisonOperator::Equal, pkr.valueAt(i) );
    leftVertex = tree.addNode(leftVertex, LogicalOperator::And, rightVertex);
  }
  filter.setFilterExpressionTree(tree);

  return filter;
}

}} // namespace Mdt{ namespace Sql{

