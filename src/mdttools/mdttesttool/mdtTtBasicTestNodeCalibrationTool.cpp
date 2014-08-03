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
#include "mdtTtBasicTestNodeCalibrationTool.h"
#include "mdtTtTestNodeManager.h"
#include "mdtDeviceU3606A.h"
#include "mdtDeviceModbusWago.h"
#include "mdtValue.h"
#include "mdtTtTest.h"
#include <QString>

#include <QDebug>

using namespace std;

mdtTtBasicTestNodeCalibrationTool::mdtTtBasicTestNodeCalibrationTool(QSqlDatabase db, QObject* parent)
 : mdtTtAbstractTestNodeCalibrationTool(db, parent)
{
}

bool mdtTtBasicTestNodeCalibrationTool::init()
{
  if(!mdtTtAbstractTestNodeCalibrationTool::init()){
    return false;
  }
  addInstruments();

  return true;
}

void mdtTtBasicTestNodeCalibrationTool::runCalibration()
{
  QString msg;
  QString whiteSpaces;  // Used to have a raisonnable message box width

  // Let the user proceed to initial setup
  whiteSpaces = "                                         ";
  msg  = tr("- Be shure that U3606A was calibrated recently") + "\n";
  msg  = tr("- Connect U3606A to Sense+ and Sense-") + "\n";
  msg += tr("- Connect calibration plug to X4") + "\n";
  msg += tr("- Connect shortest possible bridge between ISO+ and ISO-") + "\n";
  msg += tr("- Connect shortest possible bridge between Force+ and Force-");
  if(!promptUser(tr("Calibration setup") + whiteSpaces, msg)){
    return;
  }
  // Connect to instruments
  if(!connectToInstruments()){
    displayLastError();
    return;
  }
  // Calibrate K3 to K6
  if(!calibrateSenseRelays()){
    disconnectFromInstruments();
    displayLastError();
    return;
  }
  // Calibrate K1 and K2
  if(!calibrateIsoRelays()){
    disconnectFromInstruments();
    displayLastError();
    return;
  }
  // Calibrate K7 and K8
  if(!calibrateForceRelays()){
    disconnectFromInstruments();
    displayLastError();
    return;
  }
  
  
  // Disconnect from instruments
  disconnectFromInstruments();
}

bool mdtTtBasicTestNodeCalibrationTool::calibrateSenseRelays()
{
  shared_ptr<mdtDeviceU3606A> multimeter;
  shared_ptr<mdtDeviceModbusWago> coupler;
  mdtValue R;

  multimeter = testNodeManager()->device<mdtDeviceU3606A>("U3606A");
  Q_ASSERT(multimeter);
  coupler = testNodeManager()->device<mdtDeviceModbusWago>("W750");
  Q_ASSERT(coupler);

  setTestNodeUnitData("K5", "CalibrationOffset", 1.25);
  qDebug() << "K5 CAL: " << testNodeUnitData("K5", "CalibrationOffset");
  
  // Set all relays OFF
  if(coupler->setDigitalOutputsValue(false, true, true) < 0){
    pvLastError = coupler->lastError();
    return false;
  }
  coupler->wait(50);
  // Check that we have R infinite
  if(multimeter->setupResistanceMeasure(mdtDeviceU3606A::RangeMax, mdtDeviceU3606A::ResolutionMin) < 0){
    pvLastError = multimeter->lastError();
    return false;
  }
  R = multimeter->getMeasureValue();
  qDebug() << "R: " << R;
  if(!isInRange(R, 1e6)){
    pvLastError.updateText(tr("Checking about no short with all realys OFF failed."));
    return false;
  }
  // Set K3 and K5 ON
  coupler->setDigitalOutputValue("K3", true, false, false);
  coupler->setDigitalOutputValue("K5", true, false, false);
  if(!coupler->setDigitalOutputs(true)){
    pvLastError = coupler->lastError();
    return false;
  }
  coupler->wait(50);
  // Check that we have R infinite
  R = multimeter->getMeasureValue();
  qDebug() << "R: " << R;
  if(!isInRange(R, 1e6)){
    pvLastError.updateText(tr("Checking about no short with K3 and K5 set failed."));
    return false;
  }
  // Set all relays OFF
  if(coupler->setDigitalOutputsValue(false, false, false) < 0){
    pvLastError = coupler->lastError();
    return false;
  }
  // Set K4 and K6 ON
  coupler->setDigitalOutputValue("K4", true, false, false);
  coupler->setDigitalOutputValue("K6", true, false, false);
  if(!coupler->setDigitalOutputs(true)){
    pvLastError = coupler->lastError();
    return false;
  }
  coupler->wait(50);
  // Check that we have R infinite
  R = multimeter->getMeasureValue();
  qDebug() << "R: " << R;
  if(!isInRange(R, 1e6)){
    pvLastError.updateText(tr("Checking about no short with K4 and K6 set failed."));
    return false;
  }
  // Set all relays OFF
  if(coupler->setDigitalOutputsValue(false, false, false) < 0){
    pvLastError = coupler->lastError();
    return false;
  }
  // Set K4 and K5 ON
  coupler->setDigitalOutputValue("K4", true, false, false);
  coupler->setDigitalOutputValue("K5", true, false, false);
  if(!coupler->setDigitalOutputs(true)){
    pvLastError = coupler->lastError();
    return false;
  }
  coupler->wait(50);
  // Check that we have R < 1 Ohm
  if(multimeter->setupResistanceMeasure(mdtDeviceU3606A::RangeMin, mdtDeviceU3606A::ResolutionMin) < 0){
    pvLastError = multimeter->lastError();
    return false;
  }
  R = multimeter->getMeasureValue();
  qDebug() << "R: " << R;
  if(!isInRange(R, 0.0, 1.0)){
    pvLastError.updateText(tr("Checking resistance of realys K4 and K5 failed."));
    return false;
  }
  // Calibrate K4 and K5  (R/2)
  setTestNodeUnitCalibrationOffset("K4", R.valueDouble() / 2.0);
  setTestNodeUnitCalibrationOffset("K5", R.valueDouble() / 2.0);
  // Set all relays OFF
  
  // Set K3 and K6 ON
  
  // Check that we have R < 1 Ohm
  
  // Calibrate K3 and K6  (R/2)
  
  return true;
}

bool mdtTtBasicTestNodeCalibrationTool::calibrateIsoRelays()
{
  /// \todo Implement !
  
  // Set all relays OFF
  
  // Set K1 ON
  
  // Check that we have R infinite
  
  
  
  return true;
}

bool mdtTtBasicTestNodeCalibrationTool::calibrateForceRelays()
{
  /// \todo Implement !
  return true;
}


void mdtTtBasicTestNodeCalibrationTool::addInstruments()
{
  testNodeManager()->addDevice<mdtDeviceU3606A>("U3606A", "", "U3606A Multimeter");
  testNodeManager()->addDevice<mdtDeviceModbusWago>("W750", "0", "Wago 750 coupling node");
}

bool mdtTtBasicTestNodeCalibrationTool::connectToInstruments()
{
  shared_ptr<mdtDeviceU3606A> multimeter;
  shared_ptr<mdtDeviceModbusWago> coupler;
  QVariant couplerNodeId;

  multimeter = testNodeManager()->device<mdtDeviceU3606A>("U3606A");
  Q_ASSERT(multimeter);
  coupler = testNodeManager()->device<mdtDeviceModbusWago>("W750");
  Q_ASSERT(coupler);

  // Get current coupler ID
  couplerNodeId = currentTestNodeData("VehicleType_Id_FK_PK");
  if(couplerNodeId.isNull()){
    pvLastError.setError(tr("Test node with ID ") + couplerNodeId.toString() + tr(" not found."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }
  // Connect to multimeter
  if(multimeter->connectToDevice(mdtDeviceInfo()) != mdtAbstractPort::NoError){
    pvLastError.setError(tr("Cannot connect to ") + multimeter->name(), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }
  // Connect to coupler
  if(coupler->connectToDevice(mdtDeviceInfo()) != mdtAbstractPort::NoError){
    pvLastError.setError(tr("Cannot connect to ") + coupler->name(), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }
  if(!coupler->detectIos()){
    pvLastError = coupler->lastError();
    return false;
  }
  // Set short labels to coupler units
  if(!testNodeManager()->setDeviceIosLabelShort(couplerNodeId, "W750")){
    pvLastError = testNodeManager()->lastError();
    disconnectFromInstruments();
    return false;
  }
  // Reset multimeter to factory state
  if(multimeter->sendCommand("*RST") < 0){
    pvLastError.setError(tr("*RST failed on device ") + multimeter->name(), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }

  return true;
}

void mdtTtBasicTestNodeCalibrationTool::disconnectFromInstruments()
{
  Q_ASSERT(testNodeManager()->container());

  testNodeManager()->container()->disconnectFromDevices();
}
