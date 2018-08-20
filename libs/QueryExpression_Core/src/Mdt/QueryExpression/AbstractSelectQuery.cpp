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

namespace Mdt{ namespace QueryExpression{

class CallSelectQueryImplFieldIndex : public boost::static_visitor<int>
{
 public:

  CallSelectQueryImplFieldIndex(const AbstractSelectQuery & query)
   : mQuery(query)
  {
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

int AbstractSelectQuery::fieldIndex(const SelectField& field) const
{
  CallSelectQueryImplFieldIndex visitor(*this);

  return boost::apply_visitor(visitor, field.internalVariant());
}

void AbstractSelectQuery::setLastError(const Error& error)
{
  mLastError = error;
}

}} // namespace Mdt{ namespace QueryExpression{
