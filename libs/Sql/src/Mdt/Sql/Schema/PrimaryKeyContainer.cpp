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
#include "PrimaryKeyContainer.h"

namespace Mdt{ namespace Sql{ namespace Schema{

class FieldNameVisitor : public boost::static_visitor<>
{
 public:

  void operator()(const AutoIncrementPrimaryKey & pk)
  {
    fieldName = pk.fieldName();
  }
  void operator()(const SingleFieldPrimaryKey & pk)
  {
    fieldName = pk.fieldName();
  }
  void operator()(const PrimaryKey &)
  {
  }
  QString fieldName;
};

QString PrimaryKeyContainer::fieldName() const
{
  FieldNameVisitor visitor;

  boost::apply_visitor(visitor, pvPrimaryKey);

  return visitor.fieldName;
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{
