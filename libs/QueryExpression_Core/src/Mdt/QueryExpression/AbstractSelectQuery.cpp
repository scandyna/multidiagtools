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
#include "AbstractSelectQuery.h"
#include <boost/variant.hpp>

using Mdt::Container::VariantRecord;

namespace Mdt{ namespace QueryExpression{

class CallSelectQueryImplFieldIndex : public boost::static_visitor<int>
{
 public:

  CallSelectQueryImplFieldIndex(const AbstractSelectQuery & query)
   : mQuery(query)
  {
  }

  int operator()(const NullSelectField &)
  {
    Q_ASSERT_X(false, "AbstractSelectQuery", "A null SelectField is not allowed in a condition expression");

    return -1;
  }

  int operator()(const SelectAllField &)
  {
    Q_ASSERT_X(false, "AbstractSelectQuery", "SelectAllField is not allowed in a condition expression");

    return -1;
  }

  int operator()(const EntityAndField & field)
  {
    return mQuery.fieldIndexImpl(field);
  }

 private:

  const AbstractSelectQuery & mQuery;
};

bool AbstractSelectQuery::next()
{
  return fetchNext();
}

bool AbstractSelectQuery::execAndFetchAll(const SelectStatement & statement, int maxRows)
{
  Q_ASSERT(maxRows >= 0);

  if(!exec(statement, maxRows)){
    return false;
  }
  while(next()){
    const int n = fieldCount();
    VariantRecord record(n);
    for(int i = 0; i < n; ++i){
      record.setValue(i, value(i));
    }
    emit newRecordAvailable(record);
  }

  return true;
}

int AbstractSelectQuery::fieldIndex(const SelectField& field) const
{
  CallSelectQueryImplFieldIndex visitor(*this);

  return boost::apply_visitor(visitor, field.internalVariant().internalVariant());
}

void AbstractSelectQuery::setLastError(const Error& error)
{
  mLastError = error;
  emit errorOccured(mLastError);
}

}} // namespace Mdt{ namespace QueryExpression{
