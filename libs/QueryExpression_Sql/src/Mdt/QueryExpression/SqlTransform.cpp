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
#include "SqlTransform.h"
#include "Mdt/QueryExpression/SelectAllField.h"
#include "Mdt/QueryExpression/AbstractExpressionTreeVisitor.h"
#include "Mdt/QueryExpression/TravserseTreeGraph.h"
#include "Mdt/Algorithm.h"
#include <QStringBuilder>
#include <QLatin1Char>
#include <QLatin1String>
#include <QSqlField>
#include <QSqlDriver>
#include <boost/variant.hpp>

namespace Mdt{ namespace QueryExpression{

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

void escapeLikeMetacharacters(QString & str)
{
  str.replace(QLatin1Char('_'), QLatin1String("\\_"));
  str.replace(QLatin1Char('%'), QLatin1String("\\%"));
}

QString likeExpressionToSql(const LikeExpressionData & data, const QSqlDatabase & db)
{
  QString sql;
  QString pattern = data.toString();

  escapeLikeMetacharacters(pattern);
  pattern = Algorithm::replaceNonEscapedTokens(pattern, {{QLatin1Char('?'),QLatin1String("_")},{QLatin1Char('*'),QLatin1String("%")}}, QLatin1Char('\\'));
  pattern = Algorithm::unescapeEscapedTokens(pattern, {QLatin1Char('?'),QLatin1Char('*')}, QLatin1Char('\\'));
  sql = QLatin1String(" LIKE ") % valueToSql(pattern, db) % QLatin1String(" ESCAPE ") % valueToSql(QLatin1String("\\"), db);

  return sql;
}

class SelectFieldDeclarationSqlTransformVisitor : public boost::static_visitor<QString>
{
 public:

  SelectFieldDeclarationSqlTransformVisitor(const QSqlDatabase & db)
   : mDatabase(db)
  {
    Q_ASSERT(mDatabase.isValid());
  }

  QString operator()(const NullSelectField &) const
  {
    Q_ASSERT_X(false, "SqlTransform", "A null SelectField is not allowed");
    return QString();
  }

  QString operator()(const SelectAllField & field) const
  {
    if(field.hasEntity()){
      return escapeTableName(field.entityAliasOrName(), mDatabase) % QLatin1String(".*");
    }
    return QLatin1String("*");
  }

  QString operator()(const EntityAndField & field) const
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

QString selectFieldDeclarationToSql(const SelectField & field, const QSqlDatabase & db)
{
  Q_ASSERT(db.isValid());

  return boost::apply_visitor( SelectFieldDeclarationSqlTransformVisitor(db) , field.internalVariant().internalVariant() );
}

QString selectFieldListDeclarationToSql(const SelectFieldList & fieldList, const QSqlDatabase & db)
{
  Q_ASSERT(!fieldList.isEmpty());
  Q_ASSERT(db.isValid());

  QString sql;

  sql = QLatin1Char(' ') % selectFieldDeclarationToSql(fieldList.at(0), db);
  for(int i = 1; i < fieldList.fieldCount(); ++i){
    sql += QLatin1String(",\n ") % selectFieldDeclarationToSql(fieldList.at(i), db);
  }

  return sql;
}

QString fieldConditionUsageToSql(const EntityAndField & field, const QSqlDatabase& db)
{
  Q_ASSERT(db.isValid());

  QString sql;

  if(field.hasEntity()){
    sql = escapeTableName(field.entityAliasOrName(), db) % QLatin1Char('.');
  }
  sql += escapeFieldName(field.fieldName(), db);

  return sql;
}

QString selectFromEntityToSql(const QueryEntity & entity, const QSqlDatabase& db)
{
  Q_ASSERT(db.isValid());

  if(entity.hasAlias()){
    return escapeTableName(entity.name(), db) % QLatin1Char(' ') % escapeTableName(entity.alias(), db);
  }
  return escapeTableName(entity.name(), db);
}

QString operatorToSql(ComparisonOperator op)
{
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

QString operatorToSql(LogicalOperator op)
{
  switch(op){
    case LogicalOperator::And:
      return QLatin1String("AND");
    case LogicalOperator::Or:
      return QLatin1String("OR");
  }
  return QString();
}

class JoinConstraintExpressionToSqlVisitor : public AbstractExpressionTreeVisitor
{
 public:

  JoinConstraintExpressionToSqlVisitor(const QSqlDatabase & db)
   : mDatabase(db)
  {
  }

  void processPreorder(LogicalOperator) override
  {
    mSql += QLatin1Char('(');
  }

  void processPostorder(LogicalOperator) override
  {
    mSql += QLatin1Char(')');
  }

  void processInorder(ComparisonOperator op) override
  {
    mSql += operatorToSql(op);
  }

  void processInorder(LogicalOperator op) override
  {
    mSql += QLatin1Char(')') % operatorToSql(op) % QLatin1Char('(');
  }

  void processInorder(const EntityAndField& field) override
  {
    mSql += fieldConditionUsageToSql(field, mDatabase);
  }

//   void processInorder(const QVariant & value) override
//   {
//     mSql += valueToSql(value, mDatabase);
//   }

  QString toSql() const
  {
    return mSql;
  }

 private:

  QString mSql;
  const QSqlDatabase & mDatabase;
};

QString joinConstraintExpressionToSql(const JoinConstraintExpression& expression, const QSqlDatabase& db)
{
  Q_ASSERT(db.isValid());

  JoinConstraintExpressionToSqlVisitor visitor(db);
  traverseExpressionTree(expression.internalTree(), visitor);

  return visitor.toSql();
}

QString joinOperatorToSql(JoinOperator op)
{
  switch(op){
    case JoinOperator::Join:
      return QLatin1String("JOIN");
    case JoinOperator::LeftJoin:
      return QLatin1String("LEFT JOIN");
  }
  return QString();
}

QString joinClauseToSql(const JoinClause& joinClause, const QSqlDatabase& db)
{
  Q_ASSERT(db.isValid());

  return joinOperatorToSql(joinClause.joinOperator())\
         % QLatin1String("\n ")\
         % selectFromEntityToSql(joinClause.entity(), db)\
         % QLatin1String("\n  ON ")\
         % joinConstraintExpressionToSql(joinClause.joinConstraintExpression(), db);
}

QString joinClauseListToSql(const JoinClauseList& joinClauseList, const QSqlDatabase& db)
{
  Q_ASSERT(db.isValid());
  Q_ASSERT(!joinClauseList.isEmpty());

  QString sql = joinClauseToSql(joinClauseList.clauseAt(0), db);
  for(int i = 1; i < joinClauseList.clauseCount(); ++i){
    sql += QLatin1Char('\n') % joinClauseToSql(joinClauseList.clauseAt(i), db);
  }

  return sql;
}

class FilterExpressionToSqlVisitor : public AbstractExpressionTreeVisitor
{
 public:

  FilterExpressionToSqlVisitor(const QSqlDatabase & db)
   : mDatabase(db)
  {
  }

  void processPreorder(LogicalOperator) override
  {
    mSql += QLatin1Char('(');
  }

  void processPostorder(LogicalOperator) override
  {
    mSql += QLatin1Char(')');
  }

  void processInorder(ComparisonOperator op) override
  {
    mSql += operatorToSql(op);
  }

  void processInorder(LogicalOperator op) override
  {
    mSql += QLatin1Char(')') % operatorToSql(op) % QLatin1Char('(');
  }

  void processInorder(const EntityAndField& field) override
  {
    mSql += fieldConditionUsageToSql(field, mDatabase);
  }

  void processInorder(const LikeExpressionData & data) override
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

QString filterExpressionToSql(const FilterExpression & expression, const QSqlDatabase& db)
{
  Q_ASSERT(db.isValid());

  FilterExpressionToSqlVisitor visitor(db);
  traverseExpressionTree(expression.internalTree(), visitor);

  return visitor.toSql();
}

QString selectStatementToSqlLimitSyntax(const SelectStatement& stm, int maxRows, const QSqlDatabase& db)
{
  Q_ASSERT(!stm.fieldList().isEmpty());
  Q_ASSERT(maxRows >= 0);
  Q_ASSERT(db.isValid());

  QString sql;

  sql = QLatin1String("SELECT") \
      % QLatin1Char('\n') \
      % selectFieldListDeclarationToSql(stm.fieldList(), db) \
      % QLatin1Char('\n') \
      % QLatin1String("FROM ") % selectFromEntityToSql(stm.entity(), db);
  if(stm.hasJoin()){
    sql += QLatin1Char('\n') % joinClauseListToSql(stm.joinClauseList(), db);
  }
  if(stm.hasFilter()){
    sql += QLatin1String("\nWHERE ") % filterExpressionToSql(stm.filter(), db);
  }
  if(maxRows > 0){
    sql += QLatin1String("\nLIMIT ") % QString::number(maxRows);
  }

  return sql;
}

QString selectStatementToSqlTopSyntax(const SelectStatement& stm, int maxRows, const QSqlDatabase& db)
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
      % selectFieldListDeclarationToSql(stm.fieldList(), db) \
      % QLatin1Char('\n') \
      % QLatin1String("FROM ") % selectFromEntityToSql(stm.entity(), db);
  if(stm.hasJoin()){
    sql += QLatin1Char('\n') % joinClauseListToSql(stm.joinClauseList(), db);
  }
  if(stm.hasFilter()){
    sql += QLatin1String("\nWHERE ") % filterExpressionToSql(stm.filter(), db);
  }

  return sql;
}

QString selectStatementToSql(const SelectStatement & stm, int maxRows, const QSqlDatabase & db)
{
  Q_ASSERT(!stm.fieldList().isEmpty());
  Q_ASSERT(db.isValid());
  Q_ASSERT(db.driver() != nullptr);

  if(db.driver()->dbmsType() == QSqlDriver::MSSqlServer){
    return selectStatementToSqlTopSyntax(stm, maxRows, db);
  }

  return selectStatementToSqlLimitSyntax(stm, maxRows, db);
}

}} // namespace Mdt{ namespace QueryExpression{
