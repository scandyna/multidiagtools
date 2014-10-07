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
#include <QSqlQuery>

#include <QDebug>

using namespace std;

mdtTtBasicTestNodeCalibrationTool::mdtTtBasicTestNodeCalibrationTool(QSqlDatabase db, QObject* parent)
 : mdtTtAbstractTestNodeCalibrationTool(db, parent)
{
}

bool mdtTtBasicTestNodeCalibrationTool::setup(QWidget *testNodeFormWidget)
{
  if(!mdtTtAbstractTestNodeCalibrationTool::setup(testNodeFormWidget)){
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
  // Send relays states to coupler
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
  if(coupler->setDigitalOutputsValue(false, false, false) < 0){
    pvLastError = coupler->lastError();
    return false;
  }
  // Set K3 and K6 ON
  coupler->setDigitalOutputValue("K3", true, false, false);
  coupler->setDigitalOutputValue("K6", true, false, false);
  // Send relays states to coupler
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
  // Calibrate K3 and K6  (R/2)
  setTestNodeUnitCalibrationOffset("K3", R.valueDouble() / 2.0);
  setTestNodeUnitCalibrationOffset("K6", R.valueDouble() / 2.0);

  return true;
}

bool mdtTtBasicTestNodeCalibrationTool::calibrateIsoRelays()
{
  shared_ptr<mdtDeviceU3606A> multimeter;
  shared_ptr<mdtDeviceModbusWago> coupler;
  mdtValue R;
  QVariant val;
  double r4, r6;
  bool ok;

  multimeter = testNodeManager()->device<mdtDeviceU3606A>("U3606A");
  Q_ASSERT(multimeter);
  coupler = testNodeManager()->device<mdtDeviceModbusWago>("W750");
  Q_ASSERT(coupler);

  // Get R of K4 and K6
  val = testNodeUnitData("K4", "CalibrationOffset");
  if(val.isNull()){
    pvLastError.setError(tr("Could not get calibration offset of relay K4."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }
  r4 = val.toDouble(&ok);
  Q_ASSERT(ok);
  val = testNodeUnitData("K6", "CalibrationOffset");
  if(val.isNull()){
    pvLastError.setError(tr("Could not get calibration offset of relay K6."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }
  r6 = val.toDouble(&ok);
  Q_ASSERT(ok);
  // Setup multimeter to its max range
  if(multimeter->setupResistanceMeasure(mdtDeviceU3606A::RangeMax, mdtDeviceU3606A::ResolutionMin) < 0){
    pvLastError = multimeter->lastError();
    return false;
  }
  // Set all relays OFF
  if(coupler->setDigitalOutputsValue(false, false, false) < 0){
    pvLastError = coupler->lastError();
    return false;
  }
  // Set K4, K6 and K1 ON
  coupler->setDigitalOutputValue("K4", true, false, false);
  coupler->setDigitalOutputValue("K6", true, false, false);
  coupler->setDigitalOutputValue("K1", true, false, false);
  // Send relays states to coupler
  if(!coupler->setDigitalOutputs(true)){
    pvLastError = coupler->lastError();
    return false;
  }
  coupler->wait(50);
  // Check that we have R infinite
  R = multimeter->getMeasureValue();
  qDebug() << "R: " << R;
  if(!isInRange(R, 1e6)){
    pvLastError.updateText(tr("Checking about no short with K4, K6 and K1 set failed."));
    return false;
  }
  // Set all relays OFF
  if(coupler->setDigitalOutputsValue(false, false, false) < 0){
    pvLastError = coupler->lastError();
    return false;
  }
  // Set K4, K6 and K2 ON
  coupler->setDigitalOutputValue("K4", true, false, false);
  coupler->setDigitalOutputValue("K6", true, false, false);
  coupler->setDigitalOutputValue("K2", true, false, false);
  // Send relays states to coupler
  if(!coupler->setDigitalOutputs(true)){
    pvLastError = coupler->lastError();
    return false;
  }
  coupler->wait(50);
  // Check that we have R infinite
  R = multimeter->getMeasureValue();
  qDebug() << "R: " << R;
  if(!isInRange(R, 1e6)){
    pvLastError.updateText(tr("Checking about no short with K4, K6 and K2 set failed."));
    return false;
  }
  // Set K4, K6, K1 and K2 ON
  coupler->setDigitalOutputValue("K4", true, false, false);
  coupler->setDigitalOutputValue("K6", true, false, false);
  coupler->setDigitalOutputValue("K1", true, false, false);
  coupler->setDigitalOutputValue("K2", true, false, false);
  // Send relays states to coupler
  if(!coupler->setDigitalOutputs(true)){
    pvLastError = coupler->lastError();
    return false;
  }
  coupler->wait(50);
  // Setup multimeter to its min range
  if(multimeter->setupResistanceMeasure(mdtDeviceU3606A::RangeMin, mdtDeviceU3606A::ResolutionMin) < 0){
    pvLastError = multimeter->lastError();
    return false;
  }
  // Check that we have R < 1 Ohm
  R = multimeter->getMeasureValue();
  qDebug() << "R: " << R;
  if(!isInRange(R, 0.0, 1.0)){
    pvLastError.updateText(tr("Checking resistance of realys K1 and K2 failed.\nNote: did you plug the bridge between ISO+ and ISO- ?"));
    return false;
  }
  // Calibrate K1 and K2 ( (R - r4 - r6)/2 )
  setTestNodeUnitCalibrationOffset("K1", (R.valueDouble() - r4 - r6) / 2.0);
  setTestNodeUnitCalibrationOffset("K2", (R.valueDouble() - r4 - r6) / 2.0);

  return true;
}

bool mdtTtBasicTestNodeCalibrationTool::calibrateForceRelays()
{
  shared_ptr<mdtDeviceU3606A> multimeter;
  shared_ptr<mdtDeviceModbusWago> coupler;
  mdtValue R;
  QVariant val;
  double r4, r6;
  bool ok;

  multimeter = testNodeManager()->device<mdtDeviceU3606A>("U3606A");
  Q_ASSERT(multimeter);
  coupler = testNodeManager()->device<mdtDeviceModbusWago>("W750");
  Q_ASSERT(coupler);

  // Get R of K4 and K6
  // Get R of K4 and K6
  val = testNodeUnitData("K4", "CalibrationOffset");
  if(val.isNull()){
    pvLastError.setError(tr("Could not get calibration offset of relay K4."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }
  r4 = val.toDouble(&ok);
  Q_ASSERT(ok);
  val = testNodeUnitData("K6", "CalibrationOffset");
  if(val.isNull()){
    pvLastError.setError(tr("Could not get calibration offset of relay K6."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }
  r6 = val.toDouble(&ok);
  Q_ASSERT(ok);
  // Setup multimeter to its max range
  if(multimeter->setupResistanceMeasure(mdtDeviceU3606A::RangeMax, mdtDeviceU3606A::ResolutionMin) < 0){
    pvLastError = multimeter->lastError();
    return false;
  }
  // Set all relays OFF
  if(coupler->setDigitalOutputsValue(false, false, false) < 0){
    pvLastError = coupler->lastError();
    return false;
  }
  // Set K4, K6 and K7 ON
  coupler->setDigitalOutputValue("K4", true, false, false);
  coupler->setDigitalOutputValue("K6", true, false, false);
  coupler->setDigitalOutputValue("K7", true, false, false);
  // Send relays states to coupler
  if(!coupler->setDigitalOutputs(true)){
    pvLastError = coupler->lastError();
    return false;
  }
  coupler->wait(50);
  // Check that we have R infinite
  R = multimeter->getMeasureValue();
  if(!isInRange(R, 1e6)){
    pvLastError.updateText(tr("Checking about no short with K4, K6 and K2 set failed."));
    return false;
  }
  // Set all relays OFF
  if(coupler->setDigitalOutputsValue(false, false, false) < 0){
    pvLastError = coupler->lastError();
    return false;
  }
  // Set K4, K6 and K8 ON
  coupler->setDigitalOutputValue("K4", true, false, false);
  coupler->setDigitalOutputValue("K6", true, false, false);
  coupler->setDigitalOutputValue("K8", true, false, false);
  // Send relays states to coupler
  if(!coupler->setDigitalOutputs(true)){
    pvLastError = coupler->lastError();
    return false;
  }
  coupler->wait(50);
  // Check that we have R infinite
  R = multimeter->getMeasureValue();
  if(!isInRange(R, 1e6)){
    pvLastError.updateText(tr("Checking about no short with K4, K6 and K2 set failed."));
    return false;
  }
  // Set K4, K6, K7 and K8 ON
  coupler->setDigitalOutputValue("K4", true, false, false);
  coupler->setDigitalOutputValue("K6", true, false, false);
  coupler->setDigitalOutputValue("K7", true, false, false);
  coupler->setDigitalOutputValue("K8", true, false, false);
  // Send relays states to coupler
  if(!coupler->setDigitalOutputs(true)){
    pvLastError = coupler->lastError();
    return false;
  }
  coupler->wait(50);
  // Setup multimeter to its min range
  if(multimeter->setupResistanceMeasure(mdtDeviceU3606A::RangeMin, mdtDeviceU3606A::ResolutionMin) < 0){
    pvLastError = multimeter->lastError();
    return false;
  }
  // Check that we have R < 1 Ohm
  R = multimeter->getMeasureValue();
  if(!isInRange(R, 0.0, 1.0)){
    pvLastError.updateText(tr("Checking resistance of realys K7 and K8 failed.\nNote: did you plug the bridge between PWR+ and PWR- ?"));
    return false;
  }
  // Calibrate K7 and K8 ( (R - r4 - r6)/2 )
  setTestNodeUnitCalibrationOffset("K7", (R.valueDouble() - r4 - r6) / 2.0);
  setTestNodeUnitCalibrationOffset("K8", (R.valueDouble() - r4 - r6) / 2.0);

  return true;
}

bool mdtTtBasicTestNodeCalibrationTool::calibrateChannelRelays()
{

  // Select TestModel with KEY xy
  
  // Create QSqlQuery for data in TestModelItem_tbl for selecte TestModel
  
  // For each entry, get setup
  
  // ...
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
