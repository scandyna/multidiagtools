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
#include "mdtTtTestModelItemRouteData.h"

void mdtTtTestModelItemRouteData::clearData()
{
  pvId.clear();
  pvTestModelItemId.clear();
  pvTestLinkId.clear();
  pvDutConnectionId.clear();
  pvTestConnectionId.clear();
  pvMeasureConnectionId.clear();
  pvSetupDataList.clear();
}

void mdtTtTestModelItemRouteData::setId(const QVariant& id)
{
  int i;

  pvId = id;
  for(i = 0; i < pvSetupDataList.size(); ++i){
    pvSetupDataList[i].setValue("TestModelItemRoute_Id_FK", pvId);
  }
}

void mdtTtTestModelItemRouteData::setTestModelItemId(const QVariant& id)
{
  int i;

  pvTestModelItemId = id;
  for(i = 0; i < pvSetupDataList.size(); ++i){
    pvSetupDataList[i].setValue("TestModelItem_Id_FK", pvTestModelItemId);
  }
}

void mdtTtTestModelItemRouteData::setTestLinkId(const QVariant& id)
{
  pvTestLinkId = id;
}

void mdtTtTestModelItemRouteData::setDutConnectionId(const QVariant& id)
{
  pvDutConnectionId = id;
}

void mdtTtTestModelItemRouteData::setTestConnectionId(const QVariant& id)
{
  pvTestConnectionId = id;
}

void mdtTtTestModelItemRouteData::setMeasureConnectionId(const QVariant& id)
{
  pvMeasureConnectionId = id;
}

void mdtTtTestModelItemRouteData::addSetupData(mdtTtTestNodeUnitSetupData data)
{
  Q_ASSERT(data.contains("TestModelItem_Id_FK"));
  Q_ASSERT(data.contains("TestModelItemRoute_Id_FK"));

  data.setValue("TestModelItem_Id_FK", pvTestModelItemId);
  data.setValue("TestModelItemRoute_Id_FK", pvId);
  pvSetupDataList.append(data);
}
