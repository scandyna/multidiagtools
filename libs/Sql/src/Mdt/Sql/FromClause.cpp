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
class FromClauseIsNullVisitor : public boost::static_visitor<bool>
{
 public:

  bool operator()(const SelectTable & table) const
  {
    return table.isNull();
  }

  bool operator()(const QString & sql) const
  {
    return sql.isEmpty();
  }
};


/*
 * FromClause implementation
 */

void FromClause::setTable(const SelectTable& table)
{
  mClause = table;
}

void FromClause::setSqlString(const QString& sql)
{
  mClause = sql;
}

bool FromClause::isNull() const
{
  return boost::apply_visitor( FromClauseIsNullVisitor() , mClause );
}

}} // namespace Mdt{ namespace Sql{
