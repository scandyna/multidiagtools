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
#include "SelectQuery.h"
#include <boost/variant.hpp>

namespace Mdt{ namespace QueryExpression{

class CallSelectQueryImplFieldIndex : public boost::static_visitor<int>
{
  using Impl = std::shared_ptr<AbstractSelectQuery>;

 public:

  CallSelectQueryImplFieldIndex(const Impl & impl)
   : mImpl(impl)
  {
  }

    int operator()(const SelectAllField &)
    {
      Q_ASSERT_X(false, "SelectQuery", "SelectAllField is not allowed in a condition expression");

      return -1;
    }

    int operator()(const EntityAndField & field)
    {
      return mImpl->fieldIndex(field);
    }

 private:

  const Impl & mImpl;
};

bool SelectQuery::exec(const SelectStatement& statement)
{
  Q_ASSERT(!isNull());

  return mImpl->exec(statement);
}

bool SelectQuery::next()
{
  Q_ASSERT(!isNull());

  return mImpl->next();
}

int SelectQuery::fieldCount() const
{
  Q_ASSERT(!isNull());

  return mImpl->fieldCount();
}

int SelectQuery::fieldIndex(const SelectField & field) const
{
  Q_ASSERT(!isNull());

  CallSelectQueryImplFieldIndex visitor(mImpl);

  return boost::apply_visitor(visitor, field.internalVariant());
}

QVariant SelectQuery::value(int fieldIndex) const
{
  Q_ASSERT(!isNull());

  return mImpl->value(fieldIndex);
}

Error SelectQuery::lastError() const
{
  Q_ASSERT(!isNull());

  return mImpl->lastError();
}

}} // namespace Mdt{ namespace QueryExpression{
