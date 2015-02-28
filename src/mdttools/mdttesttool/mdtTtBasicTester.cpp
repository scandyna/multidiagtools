/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtTtBasicTester.h"
#include "mdtTtTest.h"
#include "mdtTtTestModelData.h"
#include "mdtTtTestItemNodeSetupData.h"
#include "mdtTtTestNodeUnitSetupData.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include "mdtSqlTableWidget.h"
#include "mdtDevice.h"
#include "mdtDeviceContainer.h"
#include "mdtDevice.h"
#include "mdtDeviceU3606A.h"
#include "mdtDeviceModbusWago.h"
#include <QString>
#include <QVariant>
#include <QMessageBox>
#include <QDate>
#include <QSqlTableModel>
#include <QSqlError>
#include <QTableView>
#include <memory>

#include <QDebug>

using namespace std;

mdtTtBasicTester::mdtTtBasicTester(QSqlDatabase db, QObject *parent)
 : mdtTtAbstractTester(db, parent)
{
}

bool mdtTtBasicTester::setup()
{
  if(!mdtTtAbstractTester::setup()){
    return false;
  }
  if(!addInstruments()){
    return false;
  }
  return true;
}

void mdtTtBasicTester::runTest()
{
  QVariant testItemId;
  ///shared_ptr<mdtDeviceU3606A> multimeter;
  mdtValue measuredValue;

  ///multimeter = testNodeManager()->device<mdtDeviceU3606A>("U3606A");
  ///Q_ASSERT(multimeter);

  if(!connectToInstruments()){
    return;
  }

  // Setup multimeter as Ohmmeter
  ///multimeter->sendCommand("CONF:RES 100, 1e-3");
//   multimeter->sendCommand("CONF:RES");
//   if(multimeter->getMeasureConfiguration() != mdtFrameCodecScpiU3606A::MT_RESISTANCE){
//     pvLastError.setError(tr("Setup U3606A as ohmmeter failed"), mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtTtCableChecker");
//     pvLastError.commit();
//     displayLastError();
//     disconnectFromInstruments();
//     return;
//   }

  
  qDebug() << "Running test ...";
  test()->resetTestItemCursor();
//   while(test()->hasMoreTestItem()){
//     testItemId = test()->nextTestItem();
//     qDebug() << "Curremt item: " << testItemId;
//     ///setTestItemData(testItemId, "Result", "Running ...");
//     test()->setCurrentTestItemData("Result", "Running ...");
//     if(!setupInstruments(testItemId)){
//       disconnectFromInstruments();
//       return;
//     }
//     
//     measuredValue = multimeter->getMeasureValue();
//     qDebug() << "MeasuredValue: " << measuredValue;
//     
//     test()->setMeasuredValue(measuredValue, QVariant(), QVariant());
//     ///test()->setCurrentTestItemData("MeasuredValue", measuredValue.valueDouble());
//     ///test()->setCurrentTestItemData("Result", "Finished");
//   }

  disconnectFromInstruments();
}

bool mdtTtBasicTester::addInstruments()
{
  ///testNodeManager()->addDevice<mdtDeviceU3606A>("U3606A", "", "U3606A Multimeter");
  ///testNodeManager()->addDevice<mdtDeviceModbusWago>("W750", "0", "Wago 750 coupling node");
  testNodeManager()->addDevice<mdtDeviceModbusWago>("W750");

  return true;
}

bool mdtTtBasicTester::connectToInstruments()
{
  shared_ptr<mdtDeviceU3606A> multimeter;
  shared_ptr<mdtDeviceModbusWago> coupler;

  ///multimeter = testNodeManager()->device<mdtDeviceU3606A>("U3606A");
  ///Q_ASSERT(multimeter);
  coupler = testNodeManager()->device<mdtDeviceModbusWago>("W750");
  Q_ASSERT(coupler);

  // Connect to multimeter
//   if(multimeter->connectToDevice(mdtDeviceInfo()) != mdtAbstractPort::NoError){
//     pvLastError.setError(tr("Cannot connect to ") + multimeter->name(), mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTester");
//     pvLastError.commit();
//     displayLastError();
//     return false;
//   }
  // Connect to coupler
  /**
  if(coupler->connectToDevice(mdtDeviceInfo()) != mdtAbstractPort::NoError){
    pvLastError.setError(tr("Cannot connect to ") + coupler->name(), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTester");
    pvLastError.commit();
    displayLastError();
    return false;
  }
  */
  if(!coupler->detectIos()){
    pvLastError = coupler->lastError();
    displayLastError();
    return false;
  }

  return true;
}

bool mdtTtBasicTester::disconnectFromInstruments()
{
  Q_ASSERT(testNodeManager()->container());

  testNodeManager()->container()->disconnectFromDevices();
}

bool mdtTtBasicTester::setupInstruments(const QVariant & testItemId)
{
  mdtTtTestItemNodeSetupData setupData;
  mdtTtTestNodeSetupData nodeSetupData;
  bool ok;

  setupData = test()->getSetupData(testItemId, ok);
  if(!ok){
    pvLastError = test()->lastError();
    displayLastError();
    return false;
  }
  while(setupData.hasMoreStep()){
    nodeSetupData = setupData.getNextStep();
    ///qDebug() << " Has setup for node " << nodeSetupData.nodeIdentification();
    if(!setupTestNode(nodeSetupData)){
      return false;
    }
  }

  return true;
}

bool mdtTtBasicTester::setupTestNode(const mdtTtTestNodeSetupData & setupData)
{
  ///qDebug() << "Setup node " << setupData.nodeIdentification();

  if(setupData.nodeIdentification() == "0"){
    return setupIoCoupler0(setupData);
  }
  
  return true;
}

bool mdtTtBasicTester::setupIoCoupler0(const mdtTtTestNodeSetupData & setupData)
{
  Q_ASSERT(setupData.nodeIdentification() == "0");

  QList<mdtTtTestNodeUnitSetupData> setupDataList;
  mdtTtTestNodeUnitSetupData unitSetupData;
  shared_ptr<mdtDeviceModbusWago> coupler;
  int i;

  coupler = testNodeManager()->device<mdtDeviceModbusWago>("W750");
  Q_ASSERT(coupler);
  // Set all outputs to 0
  Q_ASSERT(coupler->ios() != 0);
  coupler->ios()->setAnalogOutputsValue(0);
  coupler->ios()->setDigitalOutputsValue(false);
  // Set values of each I/O
  setupDataList = setupData.unitSetupList();
  for(i = 0; i < setupDataList.size(); ++i){
    unitSetupData = setupDataList.at(i);
    qDebug() << " Unit: " << unitSetupData.schemaPosition() << ", I/O pos: " << unitSetupData.ioPosition() << " , type: " << unitSetupData.ioType();
    switch(unitSetupData.ioType()){
      case mdtTtTestNodeUnitSetupData::AnalogOutput:
        if(coupler->setAnalogOutputValueAt(unitSetupData.ioPosition(), unitSetupData.value("Value").toDouble(), false, false) < 0){
          pvLastError = coupler->lastError();
          displayLastError();
          return false;
        }
        break;
      case mdtTtTestNodeUnitSetupData::DigitalOutput:
        if(coupler->setDigitalOutputValueAt(unitSetupData.ioPosition(), unitSetupData.value("State").toBool(), false, false) < 0){
          pvLastError = coupler->lastError();
          displayLastError();
          return false;
        }
        break;
    }
  }
  // Send to coupler
  if(coupler->setAnalogOutputs(true) < 0){
    pvLastError = coupler->lastError();
    displayLastError();
    return false;
  }
  if(coupler->setDigitalOutputs(true) < 0){
    pvLastError = coupler->lastError();
    displayLastError();
    return false;
  }
  // Let relays some time to switch
  coupler->wait(50);
  ///coupler->wait(300);

  return true;
}
