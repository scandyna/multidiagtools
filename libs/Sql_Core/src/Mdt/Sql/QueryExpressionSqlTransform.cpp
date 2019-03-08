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
#include "QueryExpressionSqlTransform.h"
#include "StatementAlgorithm.h"
#include "Mdt/QueryExpression/AbstractExpressionTreeVisitor.h"
#include "Mdt/QueryExpression/TravserseTreeGraph.h"
#include "Mdt/Algorithm.h"
#include <QStringBuilder>
#include <QLatin1Char>
#include <QLatin1String>
#include <QSqlField>
#include <QSqlDriver>
#include <boost/variant.hpp>

namespace Mdt{ namespace Sql{

QString fieldToWhereSql(const Mdt::QueryExpression::EntityAndField & field, const QSqlDatabase & db)
{
  Q_ASSERT(db.isValid());

  QString sql;

  if(field.hasEntity()){
    sql = escapeTableName(field.entityAliasOrName(), db) % QLatin1Char('.');
  }
  sql += escapeFieldName(field.fieldName(), db);

  return sql;
}

QString queryFieldToSql(const QueryField & field, const QSqlDatabase & db)
{
  Q_ASSERT(db.isValid());

  return fieldToWhereSql(field.entityAndField(), db);
}

class SelectFieldSqlTransformVisitor : public boost::static_visitor<QString>
{
 public:

  SelectFieldSqlTransformVisitor(const QSqlDatabase & db)
   : mDatabase(db)
  {
    Q_ASSERT(mDatabase.isValid());
  }

  QString operator()(const Mdt::QueryExpression::NullSelectField &) const
  {
    Q_ASSERT_X(false, "QueryExpressionSqlTransform", "A null SelectField is not allowed");
    return QString();
  }

  QString operator()(const Mdt::QueryExpression::SelectAllField & field) const
  {
    if(field.hasEntity()){
      return escapeTableName(field.entityAliasOrName(), mDatabase) % QLatin1String(".*");
    }
    return QLatin1String("*");
  }

  QString operator()(const Mdt::QueryExpression::EntityAndField & field) const
  {
    QString sql;

    if(field.hasEntity()){
      sql = escapeTableName(field.entityAliasOrName(), mDatabase) % QLatin1Char('.');
    }
    sql += escapeFieldName(field.fieldName(), mDatabase);
    if(field.hasFieldAlias()){
      sql += QLatin1String(" AS ") % escapeFieldName(field.fieldAlias(), mDatabase);
    }

    return sql;
  }

 private:

  const QSqlDatabase & mDatabase;
};

QString selectFieldToSql(const Mdt::QueryExpression::SelectField & field, const QSqlDatabase & db)
{
  Q_ASSERT(db.isValid());

  return boost::apply_visitor( SelectFieldSqlTransformVisitor(db) , field.internalVariant().internalVariant() );
}

QString selectFieldListToSql(const Mdt::QueryExpression::SelectFieldList & fieldList, const QSqlDatabase & db)
{
  Q_ASSERT(!fieldList.isEmpty());
  Q_ASSERT(db.isValid());

  QString sql;

  sql = QLatin1Char(' ') % selectFieldToSql(fieldList.at(0), db);
  for(int i = 1; i < fieldList.fieldCount(); ++i){
    sql += QLatin1String(",\n ") % selectFieldToSql(fieldList.at(i), db);
  }

  return sql;
}

QString operatorToSql(Mdt::QueryExpression::ComparisonOperator op)
{
  using Mdt::QueryExpression::ComparisonOperator;

  switch(op){
    case ComparisonOperator::Equal:
      return QLatin1String("=");
    case ComparisonOperator::Like:
      return QString();
    case ComparisonOperator::NotEqual:
      return QLatin1String("<>");
    case ComparisonOperator::Less:
      return QLatin1String("<");
    case ComparisonOperator::LessEqual:
      return QLatin1String("<=");
    case ComparisonOperator::Greater:
      return QLatin1String(">");
    case ComparisonOperator::GreaterEqual:
      return QLatin1String(">=");
  }
  return QString();
}

QString operatorToSql(Mdt::QueryExpression::LogicalOperator op)
{
  using Mdt::QueryExpression::LogicalOperator;

  switch(op){
    case LogicalOperator::And:
      return QLatin1String("AND");
    case LogicalOperator::Or:
      return QLatin1String("OR");
  }
  return QString();
}

void escapeLikeMetacharacters(QString & str)
{
  str.replace(QLatin1Char('_'), QLatin1String("\\_"));
  str.replace(QLatin1Char('%'), QLatin1String("\\%"));
}

QString likeExpressionToSql(const Mdt::QueryExpression::LikeExpressionData & data, const QSqlDatabase & db)
{
  Q_ASSERT(db.isValid());

  QString sql;
  QString pattern = data.toString();

  escapeLikeMetacharacters(pattern);
  pattern = Algorithm::replaceNonEscapedTokens(pattern, {{QLatin1Char('?'),QLatin1String("_")},{QLatin1Char('*'),QLatin1String("%")}}, QLatin1Char('\\'));
  pattern = Algorithm::unescapeEscapedTokens(pattern, {QLatin1Char('?'),QLatin1Char('*')}, QLatin1Char('\\'));
  sql = QLatin1String(" LIKE ") % valueToSql(pattern, db) % QLatin1String(" ESCAPE ") % valueToSql(QLatin1String("\\"), db);

  return sql;
}

class FilterExpressionToSqlVisitor : public Mdt::QueryExpression::AbstractExpressionTreeVisitor
{
 public:

  FilterExpressionToSqlVisitor(const QSqlDatabase & db)
   : mDatabase(db)
  {
  }

  void processPreorder(Mdt::QueryExpression::LogicalOperator) override
  {
    mSql += QLatin1Char('(');
  }

  void processPostorder(Mdt::QueryExpression::LogicalOperator) override
  {
    mSql += QLatin1Char(')');
  }

  void processInorder(Mdt::QueryExpression::ComparisonOperator op) override
  {
    mSql += operatorToSql(op);
  }

  void processInorder(Mdt::QueryExpression::LogicalOperator op) override
  {
    mSql += QLatin1Char(')') % operatorToSql(op) % QLatin1Char('(');
  }

  void processInorder(const Mdt::QueryExpression::EntityAndField & field) override
  {
    mSql += fieldToWhereSql(field, mDatabase);
  }

  void processInorder(const Mdt::QueryExpression::LikeExpressionData & data) override
  {
    mSql += likeExpressionToSql(data, mDatabase);
  }

  void processInorder(const QVariant & value) override
  {
    mSql += valueToSql(value, mDatabase);
  }

  QString toSql() const
  {
    return mSql;
  }

 private:

  QString mSql;
  const QSqlDatabase & mDatabase;
};

QString filterExpressionToSql(const Mdt::QueryExpression::FilterExpression & expression, const QSqlDatabase & db)
{
  Q_ASSERT(db.isValid());

  FilterExpressionToSqlVisitor visitor(db);
  traverseExpressionTree(expression.internalTree(), visitor);

  return visitor.toSql();
}

QString selectFromEntityToSql(const Mdt::QueryExpression::QueryEntity & entity, const QSqlDatabase & db)
{
  Q_ASSERT(db.isValid());

  if(entity.hasAlias()){
    return escapeTableName(entity.name(), db) % QLatin1Char(' ') % escapeTableName(entity.alias(), db);
  }
  return escapeTableName(entity.name(), db);
}

QString selectStatementToSqlLimitSyntax(const Mdt::QueryExpression::SelectStatement & stm, int maxRows, const QSqlDatabase & db)
{
  Q_ASSERT(!stm.fieldList().isEmpty());
  Q_ASSERT(maxRows >= 0);
  Q_ASSERT(db.isValid());

  QString sql;

  sql = QLatin1String("SELECT") \
      % QLatin1Char('\n') \
      % selectFieldListToSql(stm.fieldList(), db) \
      % QLatin1Char('\n') \
      % QLatin1String("FROM ") % selectFromEntityToSql(stm.entity(), db);
  if(stm.hasJoin()){
    ///sql += QLatin1Char('\n') % joinClauseListToSql(stm.joinClauseList(), db);
  }
  if(stm.hasFilter()){
    sql += QLatin1String("\nWHERE ") % filterExpressionToSql(stm.filter(), db);
  }
  if(maxRows > 0){
    sql += QLatin1String("\nLIMIT ") % QString::number(maxRows);
  }

  return sql;
}

QString selectStatementToSqlTopSyntax(const Mdt::QueryExpression::SelectStatement & stm, int maxRows, const QSqlDatabase & db)
{
  Q_ASSERT(!stm.fieldList().isEmpty());
  Q_ASSERT(maxRows >= 0);
  Q_ASSERT(db.isValid());

  QString sql;

  if(maxRows > 0){
    sql = QLatin1String("SELECT TOP ") % QString::number(maxRows);
  }else{
    sql = QLatin1String("SELECT");
  }
  sql += QLatin1Char('\n') \
      % selectFieldListToSql(stm.fieldList(), db) \
      % QLatin1Char('\n') \
      % QLatin1String("FROM ") % selectFromEntityToSql(stm.entity(), db);
  if(stm.hasJoin()){
    ///sql += QLatin1Char('\n') % joinClauseListToSql(stm.joinClauseList(), db);
  }
  if(stm.hasFilter()){
    sql += QLatin1String("\nWHERE ") % filterExpressionToSql(stm.filter(), db);
  }

  return sql;
}

QString selectStatementToSql(const Mdt::QueryExpression::SelectStatement & stm, int maxRows, const QSqlDatabase & db)
{
  Q_ASSERT(!stm.fieldList().isEmpty());
  Q_ASSERT(db.isValid());
  Q_ASSERT(db.driver() != nullptr);

  if(db.driver()->dbmsType() == QSqlDriver::MSSqlServer){
    return selectStatementToSqlTopSyntax(stm, maxRows, db);
  }

  return selectStatementToSqlLimitSyntax(stm, maxRows, db);
}

}} // namespace Mdt{ namespace Sql{

