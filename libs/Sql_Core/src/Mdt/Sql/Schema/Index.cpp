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
#include "Index.h"
#include "Table.h"

namespace Mdt{ namespace Sql{ namespace Schema{

void Index::generateName()
{
  Q_ASSERT(!pvTableName.isEmpty());
  Q_ASSERT(!pvFieldNameList.isEmpty());

  QString name = pvTableName + QStringLiteral("_");
  for(const auto & fieldName : pvFieldNameList){
    name += fieldName + QStringLiteral("_");
  }
  name += "index";
  setName(name);
}

void Index::setTable(const Table& table)
{
  pvTableName = table.tableName();
}

void Index::clear()
{
  pvIsUnique = false;
  pvName.clear();
  pvTableName.clear();
  pvFieldNameList.clear();
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{