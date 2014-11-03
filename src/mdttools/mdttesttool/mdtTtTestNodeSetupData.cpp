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
#include "mdtTtTestNodeSetupData.h"

void mdtTtTestNodeSetupData::setNodeIdentification(const QString & id)
{
  pvNodeIdentification = id;
}

void mdtTtTestNodeSetupData::setDeviceIdentification(const QVariant& id)
{
  pvDeviceIdentification = id;
}

void mdtTtTestNodeSetupData::clear()
{
  pvNodeIdentification.clear();
  pvDeviceIdentification.clear();
  clearUnitSetup();
}

void mdtTtTestNodeSetupData::clearUnitSetup()
{
  pvUnitSetupData.clear();
}

void mdtTtTestNodeSetupData::addUnitSetup(const mdtTtTestNodeUnitSetupData& data)
{
  pvUnitSetupData.append(data);
}

void mdtTtTestNodeSetupData::addUnitSetupList(const QList< mdtTtTestNodeUnitSetupData >& datalist)
{
  pvUnitSetupData.append(datalist);
}

void mdtTtTestNodeSetupData::addRouteData(const mdtTtTestModelItemRouteData& data)
{
  pvRouteDataList.append(data);
  pvUnitSetupData.append(data.setupDataList());
}

void mdtTtTestNodeSetupData::addRouteDataList(const QList< mdtTtTestModelItemRouteData >& dataList)
{
  int i;

  for(i = 0; i < dataList.size(); ++i){
    addRouteData(dataList.at(i));
  }
}

QList< mdtTtTestModelItemRouteData > mdtTtTestNodeSetupData::routeDataListForStepNumber(int stepNumber) const
{
  int i, k;

  for(i = 0; i < pvRouteDataList.size(); ++i){
    
  }
}
