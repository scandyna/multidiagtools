/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "FromClause.h"

namespace Mdt{ namespace Sql{

/*
 * FromClauseIsNullVisitor
 */
class FromClauseIsNullVisitor : boost::static_visitor<bool>
{
 public:

  bool operator()(const boost::blank &) const
  {
    return true;
  }

  bool operator()(const JoinClause & joinClause) const
  {
    return joinClause.isNull();
  }

  bool operator()(const QString & sql) const
  {
    return sql.isEmpty();
  }
};

/*
 * FromClauseJoinTableOnExressionVisitor
 */
class FromClauseJoinTableOnExressionVisitor : boost::static_visitor<>
{
 public:

  FromClauseJoinTableOnExressionVisitor(JoinOperator op, const SelectTable & table, const JoinConstraintExpression & expr)
   : mJoinOperator(op) ,
     mTable(table) ,
     mExpression(expr)
  {
  }

  void operator()(const boost::blank &) const
  {
    Q_ASSERT_X(true, "FromClause::joinTableOn()", "Join a table while FROM table was not set is not supported.");
  }

  void operator()(JoinClause & joinClause) const
  {
    joinClause.joinTableOn(mJoinOperator, mTable, mExpression);
  }

  void operator()(const QString &) const
  {
    Q_ASSERT_X(true, "FromClause::joinTableOn()", "Join a table while FROM table was not set is not supported.");
  }

 private:

  JoinOperator mJoinOperator;
  const SelectTable & mTable;
  const JoinConstraintExpression & mExpression;
};

/*
 * FromClauseJoinTableOnFromTableVisitor
 */
class FromClauseJoinTableOnFromTableVisitor : boost::static_visitor<>
{
 public:

  FromClauseJoinTableOnFromTableVisitor(JoinOperator op, const SelectTable & table)
   : mJoinOperator(op) ,
     mTable(table)
  {
  }

  void operator()(const boost::blank &) const
  {
    Q_ASSERT_X(true, "FromClause::joinTableOn()", "Join a table while FROM table was not set is not supported.");
  }

  void operator()(JoinClause & joinClause) const
  {
    joinClause.joinTableOn(mJoinOperator, mTable);
  }

  void operator()(const QString &) const
  {
    Q_ASSERT_X(true, "FromClause::joinTableOn()", "Join a table while FROM table was not set is not supported.");
  }

 private:

  JoinOperator mJoinOperator;
  const SelectTable & mTable;
};

/*
 * FromClauseJoinTableOnConstraintTableVisitor
 */
class FromClauseJoinTableOnConstraintTableVisitor : boost::static_visitor<>
{
 public:

  FromClauseJoinTableOnConstraintTableVisitor(JoinOperator op, const SelectTable & table, const SelectTable & constraintOnTable)
   : mJoinOperator(op) ,
     mTable(table) ,
     mConstraintOnTable(constraintOnTable)
  {
  }

  void operator()(const boost::blank &) const
  {
    Q_ASSERT_X(true, "FromClause::joinTableOn()", "Join a table while FROM table was not set is not supported.");
  }

  void operator()(JoinClause & joinClause) const
  {
    joinClause.joinTableOn(mJoinOperator, mTable, mConstraintOnTable);
  }

  void operator()(const QString &) const
  {
    Q_ASSERT_X(true, "FromClause::joinTableOn()", "Join a table while FROM table was not set is not supported.");
  }

 private:

  JoinOperator mJoinOperator;
  const SelectTable & mTable;
  const SelectTable & mConstraintOnTable;
};


/*
 * FromClause implementation
 */

void FromClause::setTable(const SelectTable & table)
{
  Q_ASSERT(!table.isNull());

#ifndef QT_NO_DEBUG
  const auto *joinClause = boost::get<JoinClause>(&mClause);
  if(joinClause != nullptr){
    Q_ASSERT_X(!joinClause->hasJoinedTable(), "FromClause::setTable()", "Setting a table while some table(s) was allready joined is not supported.");
  }
#endif // #ifndef QT_NO_DEBUG

  mClause = JoinClause(table);
}

void FromClause::joinTableOn(JoinOperator op, const SelectTable & table, const JoinConstraintExpression & expr)
{
  Q_ASSERT(!table.isNull());
  Q_ASSERT(!expr.isNull());

  boost::apply_visitor( FromClauseJoinTableOnExressionVisitor(op, table, expr) , mClause );
}

void FromClause::joinTableOn(JoinOperator op, const SelectTable & table)
{
  Q_ASSERT(!table.isNull());

  boost::apply_visitor( FromClauseJoinTableOnFromTableVisitor(op, table) , mClause);
}

void FromClause::joinTableOn(JoinOperator op, const SelectTable & table, const SelectTable & constraintOnTable)
{
  Q_ASSERT(!table.isNull());
  Q_ASSERT(!constraintOnTable.isNull());

  boost::apply_visitor( FromClauseJoinTableOnConstraintTableVisitor(op, table, constraintOnTable) , mClause );
}

void FromClause::setSqlString(const QString & sql)
{
  mClause = sql;
}

bool FromClause::isNull() const
{
  return boost::apply_visitor( FromClauseIsNullVisitor() , mClause );
}

}} // namespace Mdt{ namespace Sql{
