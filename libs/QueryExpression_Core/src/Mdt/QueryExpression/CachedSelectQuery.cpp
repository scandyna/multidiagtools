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
#include "CachedSelectQuery.h"

// #include <QDebug>

namespace Mdt{ namespace QueryExpression{

bool CachedSelectQuery::exec(const SelectStatement& statement)
{
  Q_ASSERT(!isNull());

  mCache->clear();
  if(!mImpl->exec(statement)){
    return false;
  }
  while(mImpl->next()){
    mCache->push_back(buildCurrentRecord());
  }

  return true;
}

Error CachedSelectQuery::lastError() const
{
  Q_ASSERT(!isNull());

  return mImpl->lastError();
}

CachedSelectQuery::Record CachedSelectQuery::buildCurrentRecord() const
{
  Q_ASSERT(!isNull());

  Record record(columnCount());

  const int n = columnCount();
  for(int col = 0; col < n; ++col){
    record[col] = mImpl->value(col);
  }

  return record;
}

}} // namespace Mdt{ namespace QueryExpression{
