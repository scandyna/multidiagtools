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
#include "mdtTtTestItemNodeSetupData.h"

mdtTtTestItemNodeSetupData::mdtTtTestItemNodeSetupData()
{
  ///pvCurrentStep = 0;
  pvStepIterator = pvNodeSetupDataMap.begin();
}

void mdtTtTestItemNodeSetupData::addNodeSetupData(const mdtTtTestNodeSetupData & data)
{
  // Build list of setps contained in node unit setup data list
  
  // For each step, build a test node setup and add unit setup that are part of the step
  
  // Add each node setup to the map
  
}

/**
void mdtTtTestItemNodeSetupData::addNodeUnitSetupData(const mdtTtTestNodeUnitSetupData & data)
{
}
*/

void mdtTtTestItemNodeSetupData::clear()
{
  pvNodeSetupDataMap.clear();
  ///pvCurrentStep = 0;
  pvStepIterator = pvNodeSetupDataMap.begin();
}

mdtTtTestNodeSetupData mdtTtTestItemNodeSetupData::getNextStep()
{

}

QVariant mdtTtTestItemNodeSetupData::currentDeviceIdentification() const
{
  if(pvStepIterator == pvNodeSetupDataMap.end()){
    return QVariant();
  }else{
    return pvStepIterator.value().devicedentification();
  }
}
