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
#include "JoinClauseList.h"
#include <algorithm>

namespace Mdt{ namespace QueryExpression{

void JoinClauseList::addClause(JoinOperator joinOperator, const QueryEntity & entity, const JoinConstraintExpression& join)
{
  Q_ASSERT(!entity.isNull());
  Q_ASSERT(!join.isNull());

  mList.emplace_back(joinOperator, entity, join);
}

void JoinClauseList::clear()
{
  mList.clear();
}

bool JoinClauseList::containsEntity(const QString & entityName) const
{
  Q_ASSERT(!entityName.trimmed().isEmpty());

  const auto pred = [&entityName](const JoinClause & jc){
    return jc.entity().name() == entityName;
  };
  const auto it = std::find_if(mList.cbegin(), mList.cend(), pred);

  return it != mList.cend();
}

}} // namespace Mdt{ namespace QueryExpression{
