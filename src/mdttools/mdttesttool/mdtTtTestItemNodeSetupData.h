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
#ifndef MDT_TT_TEST_ITEM_NODE_SETUP_DATA_H
#define MDT_TT_TEST_ITEM_NODE_SETUP_DATA_H

#include "mdtTtTestNodeSetupData.h"
#include "mdtTtTestNodeUnitSetupData.h"
#include <QList>
#include <QMultiMap>
#include <QVariant>

/*! \brief Store setup data related to a test item
 *
 * Setup data are stored in TestNodeUnitSetup_tbl,
 *  and each entry is related to a test item.
 *
 * Many setup data can be related to one test item.
 *
 * Each setup data is related to a node unit,
 *  and also to a node.
 *
 * To apply a setup for one test item,
 *  it can be necessary to configure
 *  nodes in a given order.
 *  For this, TestNodeUnitSetup_tbl contains a field (StepNumber).
 *
 * Example of a setup:
 *
 *  <table border="1" cellpadding="5">
 *   <tr><th>Test item</th><th>Node</th><th>Node unit</th><th>Value</th><th>Step</th></tr>
 *   <tr><td>1</td><td>Multimeter</td><td>Source</td><td>OFF</td><td>0</td></tr>
 *   <tr><td>1</td><td>I/O module</td><td>K1</td><td>ON</td><td>1</td></tr>
 *   <tr><td>1</td><td>I/O module</td><td>K2</td><td>ON</td><td>1</td></tr>
 *   <tr><td>1</td><td>I/O module</td><td>K5</td><td>ON</td><td>1</td></tr>
 *   <tr><td>1</td><td>I/O module</td><td>K6</td><td>ON</td><td>1</td></tr>
 *   <tr><td>1</td><td>Multimeter</td><td>Source</td><td>12V</td><td>1</td></tr>
 *   <tr><td>1</td><td>Multimeter</td><td>Source</td><td>ON</td><td>2</td></tr>
 *  </table>
 *
 * In this example, we need to disable source first. Next, we setup the nodes.
 *  Finally, we enable source output.
 *
 * To add setup data, use addNodeSetupData() or addNodeUnitSetupData().
 *
 * To get setup data, use hasMoreStep() and getNextStep().
 */
class mdtTtTestItemNodeSetupData
{
 public:

  /*! \brief Constructor
   */
  mdtTtTestItemNodeSetupData();

  /*! \brief Add test node setup data
   *
   * Will add node setup data, regarding deviceIdentification
   *  and stepNumber of each unit setup stored inside.
   */
  void addNodeSetupData(const mdtTtTestNodeSetupData & data);

  /*! \brief Add test node unit setup data
   */
  ///void addNodeUnitSetupData(const mdtTtTestNodeUnitSetupData & data);

  /*! \brief Clear data
   */
  void clear();

  /*! \brief Check if more step is available
   */
  inline bool hasMoreStep() const
  {
    ///return (pvCurrentStep < pvNodeSetupDataMap.size());
    return (pvStepIterator != pvNodeSetupDataMap.end());
  }

  /*! \brief Get next step
   */
  mdtTtTestNodeSetupData getNextStep();

  /*! \brief Get device identification for current step
   */
  QVariant currentDeviceIdentification() const;

 private:

  ///int pvCurrentStep;
  ///QList<mdtTtTestNodeSetupData> pvNodeSetupDataList;
  QMultiMap<int, mdtTtTestNodeSetupData> pvNodeSetupDataMap;
  QMultiMap<int, mdtTtTestNodeSetupData>::const_iterator pvStepIterator;
  mdtTtTestNodeSetupData pvCurrentTestNodeSetupData;
};

#endif // #ifndef MDT_TT_TEST_ITEM_NODE_SETUP_DATA_H
