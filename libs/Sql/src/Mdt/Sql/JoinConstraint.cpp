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
#include "JoinConstraint.h"
#include "SelectTable.h"

namespace Mdt{ namespace Sql{

/*
 * JoinConstraintIsNullVisitor
 */
class JoinConstraintIsNullVisitor : boost::static_visitor<bool>
{
 public:

  bool operator()(const JoinConstraintExpression & expr) const
  {
    return expr.isNull();
  }

  bool operator()(const JoinConstraintFieldPairList & fpl) const
  {
    return fpl.isNull();
  }
};

/*
 * JoinConstraint implementation
 */

void JoinConstraint::setOnExpression(const JoinConstraintExpression & expr)
{
  mOperator = JoinConstraintOperator::On;
  mConstraint = expr;
}

void JoinConstraint::setOnExpression(const SelectTable & left, const SelectTable & right)
{
  mOperator = JoinConstraintOperator::On;
  mConstraint = JoinConstraintFieldPairList::fromTables(left, right);
}

bool JoinConstraint::isNull() const
{
  return boost::apply_visitor( JoinConstraintIsNullVisitor(), mConstraint );
}


}} // namespace Mdt{ namespace Sql{
