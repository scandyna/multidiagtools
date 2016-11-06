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
#include "JoinClause.h"
#include "JoinConstraintExpression.h"
#include <QByteArray>

namespace Mdt{ namespace Sql{

JoinClause::JoinClause(const SelectTable& fromTable)
 : mFromTable(fromTable)
{
  Q_ASSERT(!mFromTable.isNull());
}

void JoinClause::joinTableOn(JoinOperator op, const SelectTable & table, const JoinConstraintExpression & expr)
{
  Q_ASSERT(!table.isNull());
  Q_ASSERT(!expr.isNull());

  mItemList.append(op, table, expr);
}

void JoinClause::joinTableOn(JoinOperator op, const SelectTable & table)
{
  Q_ASSERT(!table.isNull());
  /*
   * If a precondition is not satisfied, generate a message.
   * This seems heavy, but can be very usefull for the user.
   */
#ifndef QT_NO_DEBUG
  QString whatStr;
  // Check that at least table or mFromTable has a foreign key defined
  whatStr = QString("Deducing relation between '%1' and '%2' is not possible because both have no foreign key defined.").arg(table.aliasOrTableName(), mFromTable.aliasOrTableName());
  Q_ASSERT_X(table.hasForeignKey() || mFromTable.hasForeignKey(), "JoinClause::joinTableOn()", whatStr.toLocal8Bit().constData());
//   // Check that from table has a at least 1 foreign key
//   whatStr = QString("Deducing relation between '%1' and '%2' is not possible because '%2' has no foreign key defined.").arg(table.aliasOrTableName(), mFromTable.aliasOrTableName());
//   Q_ASSERT_X(mFromTable.hasForeignKey(), "JoinClause::joinTableOn()", whatStr.toLocal8Bit().constData());
#endif // #ifndef QT_NO_DEBUG

  mItemList.append(op, table, mFromTable);
}

void JoinClause::joinTableOn(JoinOperator op, const SelectTable & table, const SelectTable & constraintOnTable)
{
  Q_ASSERT(!table.isNull());
  /*
   * If a precondition is not satisfied, generate a message.
   * This seems heavy, but can be very usefull for the user.
   */
#ifndef QT_NO_DEBUG
  QString whatStr;
  // Check that at least table or constraintOnTable has a foreign key defined
  whatStr = QString("Deducing relation between '%1' and '%2' is not possible because both have no foreign key defined.").arg(table.aliasOrTableName(), mFromTable.aliasOrTableName());
  Q_ASSERT_X(table.hasForeignKey() || constraintOnTable.hasForeignKey(), "JoinClause::joinTableOn()", whatStr.toLocal8Bit().constData());
  // Check that constraintOnTable has a at least 1 foreign key
//   whatStr = QString("Deducing relation between '%1' and '%2' is not possible because '%2' has no foreign key defined.").arg(table.aliasOrTableName(), constraintOnTable.aliasOrTableName());
//   Q_ASSERT_X(constraintOnTable.hasForeignKey(), "JoinClause::joinTableOn()", whatStr.toLocal8Bit().constData());
#endif // #ifndef QT_NO_DEBUG

  mItemList.append(op, table, constraintOnTable);
}

}} // namespace Mdt{ namespace Sql{
