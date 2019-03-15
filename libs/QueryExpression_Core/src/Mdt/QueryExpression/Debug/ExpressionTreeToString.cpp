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
#include "ExpressionTreeToString.h"
#include "../AbstractExpressionTreeVisitor.h"
#include "../TravserseTreeGraph.h"
#include <QLatin1Char>
#include <QLatin1String>
#include <QStringBuilder>

namespace Mdt{ namespace QueryExpression{ namespace Debug{

QString comparisonOperatorToString(ComparisonOperator op)
{
  switch(op){
    case ComparisonOperator::Equal:
      return QLatin1String("==");
    case ComparisonOperator::Like:
      return QLatin1String(" Like ");
    case ComparisonOperator::NotEqual:
      return QLatin1String("!=");
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

QString logicalOperatorToString(LogicalOperator op)
{
  switch(op){
    case LogicalOperator::And:
      return QLatin1String("&&");
    case LogicalOperator::Or:
      return QLatin1String("||");
  }
  return QString();
}

class ExpressionToInfixStringVisitor : public AbstractExpressionTreeVisitor
{
 public:

  void processPreorder(Mdt::QueryExpression::LogicalOperator) override
  {
    mExpressionString += QLatin1Char('(');
  }

  void processPostorder(Mdt::QueryExpression::LogicalOperator) override
  {
    mExpressionString += QLatin1Char(')');
  }

  void processInorder(Mdt::QueryExpression::ComparisonOperator op) override
  {
    mExpressionString += comparisonOperatorToString(op);
  }

  void processInorder(Mdt::QueryExpression::LogicalOperator op) override
  {
    mExpressionString += QLatin1Char(')') % logicalOperatorToString(op) % QLatin1Char('(');
  }

  void processInorder(const Mdt::QueryExpression::EntityAndField & field) override
  {
    if( field.hasEntity() ){
      mExpressionString += field.entityAliasOrName() % QLatin1Char('.') % field.fieldName();
    }else{
      mExpressionString += field.fieldName();
    }
  }

  void processInorder(const QVariant & value) override
  {
    mExpressionString += value.toString();
  }

  void processInorder(const LikeExpressionData& data) override
  {
    mExpressionString += QLatin1Char('\'') % data.toString() % QLatin1Char('\'');
  }

  void clear()
  {
    mExpressionString.clear();
  }

  QString toString() const
  {
    return mExpressionString;
  }

 private:

  QString mExpressionString;
};


QString expressionTreeToInfixString(const ExpressionTree & tree)
{
  Q_ASSERT(!tree.isNull());

  ExpressionToInfixStringVisitor visitor;
  traverseExpressionTree(tree, visitor);

  return visitor.toString();
}

QString filterExpressionToInfixString(const FilterExpression & filter)
{
  Q_ASSERT(!filter.isNull());

  return expressionTreeToInfixString(filter.internalTree());
}

}}} // namespace Mdt{ namespace QueryExpression{ namespace Debug{
