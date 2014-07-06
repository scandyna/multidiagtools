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
#include <QVector>

//#include <QDebug>

mdtTtTestItemNodeSetupData::mdtTtTestItemNodeSetupData()
{
  ///pvCurrentStep = 0;
  pvStepIterator = pvNodeSetupDataMap.begin();
}

void mdtTtTestItemNodeSetupData::addNodeSetupData(const mdtTtTestNodeSetupData & data)
{
  QList<mdtTtTestNodeUnitSetupData> unitSetupDataList;
  int step;
  QVector<int> steps;
  int i, k;

  // Build list of setps contained in node unit setup data list
  unitSetupDataList = data.unitSetupList();
  for(i = 0; i < unitSetupDataList.size(); ++i){
    step = unitSetupDataList.at(i).value("StepNumber").toInt();
    if(!steps.contains(step)){
      steps.append(step);
    }
  }
  // For each step, build a test node setup and add unit setup that are part of the step
  for(i = 0; i < steps.size(); ++i){
    // Setup a new node setup
    mdtTtTestNodeSetupData nodeSetupData;
    nodeSetupData.setNodeIdentification(data.nodeIdentification());
    nodeSetupData.setDeviceIdentification(data.deviceIdentification());
    // Add unit setup for current step
    for(k = 0; k < unitSetupDataList.size(); ++k){
      if(unitSetupDataList.at(k).value("StepNumber").toInt() == steps.at(i)){
        nodeSetupData.addUnitSetup(unitSetupDataList.at(k));
      }
    }
    // Add node setup to map
    pvNodeSetupDataMap.insert(steps.at(i), nodeSetupData);
  }
  pvStepIterator = pvNodeSetupDataMap.begin();
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
  pvCurrentTestNodeSetupData.clear();
}

mdtTtTestNodeSetupData mdtTtTestItemNodeSetupData::getNextStep()
{
  Q_ASSERT(pvStepIterator != pvNodeSetupDataMap.end());

  pvCurrentTestNodeSetupData = pvStepIterator.value();
  ++pvStepIterator;

  return pvCurrentTestNodeSetupData;
}

QVariant mdtTtTestItemNodeSetupData::currentDeviceIdentification() const
{
  return pvCurrentTestNodeSetupData.deviceIdentification();
  /**
  if(pvStepIterator == pvNodeSetupDataMap.end()){
    return QVariant();
  }else{
    return pvStepIterator.value().devicedentification();
  }
  */
}
