/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtTtTestModelItemData.h"

mdtTtTestModelItemData::mdtTtTestModelItemData()
 : mdtSqlRecord()
{
}

bool mdtTtTestModelItemData::setup(const QSqlDatabase & db)
{
  return addAllFields("TestModelItem_tbl", db);
}

void mdtTtTestModelItemData::clear()
{
  pvRouteDataList.clear();
  mdtSqlRecord::clear();
}

void mdtTtTestModelItemData::clearValues()
{
  pvRouteDataList.clear();
  mdtSqlRecord::clearValues();
}

void mdtTtTestModelItemData::setId(const QVariant& id)
{
  int i;

  setValue("Id_PK", id);
  for(i = 0; i < pvRouteDataList.size(); ++i){
    pvRouteDataList[i].setTestModelItemId(id);
  }
}

void mdtTtTestModelItemData::addRouteData(mdtTtTestModelItemRouteData data)
{
  data.setTestModelItemId(value("Id_PK"));
  pvRouteDataList.append(data);
}
