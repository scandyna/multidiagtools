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
#include "mdtTtBasicTestNodeCalibrationTool.h"
#include "mdtTtTestNodeManager.h"
#include "mdtDeviceU3606A.h"
#include "mdtDeviceModbusWago.h"
#include "mdtValue.h"
#include "mdtTtTest.h"
#include "mdtTtTestItemNodeSetupData.h"
#include "mdtTtTestNodeSetupData.h"
#include "mdtTtTestNodeUnitSetupData.h"
#include "mdtTtTestModelItemRouteData.h"
#include "mdtClLinkData.h"
#include <QString>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include <QDebug>

using namespace std;

mdtTtBasicTestNodeCalibrationTool::mdtTtBasicTestNodeCalibrationTool(QSqlDatabase db, QObject* parent)
 : mdtTtAbstractTestNodeCalibrationTool(db, parent),
   pvGraph(db),
   pvMultimeter(new mdtDeviceU3606A(this))
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

  // Load link list to graph - Will be used to get links resistances
  if(!pvGraph.loadLinkList()){
    pvLastError = pvGraph.lastError();
    displayLastError();
    return;
  }
  // Let the user proceed to initial setup
  msg  = tr("- Be shure that U3606A was calibrated recently") + "\n";
  msg += tr("- Connect U3606A Sense inputs to couplers Sense+ and Sense-") + "\n";
  msg += tr("- Check that U3606A Force outputs are NOT connected to coupler") + "\n";
  msg += tr("- Connect calibration plug to X4") + "\n";
  msg += tr("- Connect shortest possible bridge between ISO+ and ISO-") + "\n";
  msg += tr("- Connect shortest possible bridge between Force+ and Force-");
  if(!promptUser(tr("Calibration setup (step 1: coupler checks)"), msg)){
    return;
  }
  // Connect to instruments
  if(!connectToInstruments()){
    displayLastError();
    return;
  }
  /**
  // Check K3 to K6
  if(!checkSenseRelays()){
    disconnectFromInstruments();
    displayLastError();
    return;
  }
  // Check K1 and K2
  if(!checkIsoRelays()){
    disconnectFromInstruments();
    displayLastError();
    return;
  }
  // Check K7 and K8
  if(!checkForceRelays()){
    disconnectFromInstruments();
    displayLastError();
    return;
  }
  
  */
  /// \todo check channel relays
  
  // Let user proceed to next setup
  msg  = tr("- Unplug bridge between ISO+ and ISO-") + "\n";
  msg += tr("- Unplug bridge between Force+ and Force-") + "\n";
  msg += tr("- Connect U3606A Force outputs to couplers Sense+ and Sense-") + "\n";
  msg += tr("- Connect U3606A Sense inputs to couplers Sense+ and Sense-");
  if(!promptUser(tr("Calibration setup (step 2: sense relays calibration K3 to K6)"), msg)){
    return;
  }
  // Calibrate K3 to K6
  if(!calibrateSenseRelays()){
    disconnectFromInstruments();
    displayLastError();
    return;
  }
  // Let user proceed to next setup
  msg  = tr("- Connect U3606A Force outputs to couplers ISO+ and ISO-") + "\n";
  msg += tr("- Connect U3606A Sense inputs to couplers ISO+ and ISO-");
  if(!promptUser(tr("Calibration setup (step 3: ISO relays calibration K1 and K2)"), msg)){
    return;
  }
  // Calibrate K1 and K2
  if(!calibrateIsoRelays()){
    disconnectFromInstruments();
    displayLastError();
    return;
  }
  // Let user proceed to next setup
  msg  = tr("- Connect U3606A Force outputs to couplers Force+ and Force-") + "\n";
  msg += tr("- Connect U3606A Sense inputs to couplers Force+ and Force-");
  if(!promptUser(tr("Calibration setup (step 4: PWR realys Calibration K7 and K8)"), msg)){
    return;
  }
  // Calibrate K7 and K8
  if(!calibrateForceRelays()){
    disconnectFromInstruments();
    displayLastError();
    return;
  }
  // Let user proceed to next setup
  msg  = tr("- Connect U3606A Force outputs to couplers Force+ and Force-") + "\n";
  msg += tr("- Connect U3606A Sense inputs to couplers Sense+ and Sense-");
  if(!promptUser(tr("Calibration setup (step 5: channel relays calibration)"), msg)){
    return;
  }
  // Calibrate channel relays
  if(!calibrateChannelRelays()){
    disconnectFromInstruments();
    displayLastError();
    return;
  }
  
  // Disconnect from instruments
  disconnectFromInstruments();
  
  /// \todo Inform user about success
}

void mdtTtBasicTestNodeCalibrationTool::saveCalibration()
{

  if(!saveTestNodeUnitCalibrationOffsets()){
    displayLastError();
    return;
  }
}

double mdtTtBasicTestNodeCalibrationTool::measureBridge()
{
  mdtValueDouble R;

  if(!pvMultimeter->setupResistanceMeasure(mdtDeviceU3606A::Range_t::Min, mdtDeviceU3606A::Resolution_t::Min)){
    pvLastError = pvMultimeter->lastError();
    return -1.0;
  }
  R = pvMultimeter->getMeasureValue();
  qDebug() << "R of bridge: " << R;
  if(!isInRange(R, 0.0, 1.0)){
    pvLastError.updateText(tr("Checking resistance of a bridge failed."));
    return -1.0;
  }

  return R.value();
}

bool mdtTtBasicTestNodeCalibrationTool::checkSenseRelays()
{
  shared_ptr<mdtDeviceModbusWago> coupler;
  mdtValueDouble R;

  coupler = testNodeManager()->device<mdtDeviceModbusWago>("W750");
  Q_ASSERT(coupler);

  // Set all relays OFF
  if(coupler->setDigitalOutputsValue(false, true, true) < 0){
    pvLastError = coupler->lastError();
    return false;
  }
  coupler->wait(50);
  // Check that we have R infinite
  if(!pvMultimeter->setupResistanceMeasure(mdtDeviceU3606A::Range_t::Max, mdtDeviceU3606A::Resolution_t::Min)){
    pvLastError = pvMultimeter->lastError();
    return false;
  }
  R = pvMultimeter->getMeasureValue();
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
  R = pvMultimeter->getMeasureValue();
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
  R = pvMultimeter->getMeasureValue();
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
  if(!pvMultimeter->setupResistanceMeasure(mdtDeviceU3606A::Range_t::Min, mdtDeviceU3606A::Resolution_t::Min)){
    pvLastError = pvMultimeter->lastError();
    return false;
  }
  R = pvMultimeter->getMeasureValue();
  qDebug() << "R: " << R;
  if(!isInRange(R, 0.0, 1.0)){
    pvLastError.updateText(tr("Checking resistance of realys K4 and K5 failed."));
    return false;
  }
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
  R = pvMultimeter->getMeasureValue();
  qDebug() << "R: " << R;
  if(!isInRange(R, 0.0, 1.0)){
    pvLastError.updateText(tr("Checking resistance of realys K4 and K5 failed."));
    return false;
  }

  return true;
}

bool mdtTtBasicTestNodeCalibrationTool::calibrateSenseRelays()
{
  shared_ptr<mdtDeviceModbusWago> coupler;
  QStringList relays;
  mdtValueDouble R;
  double Rlinks;
  double r;
  bool ok;

  coupler = testNodeManager()->device<mdtDeviceModbusWago>("W750");
  Q_ASSERT(coupler);

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
  // Frit line
  if(!fritLine()){
    pvLastError.updateText(tr("Fritting line of realys K4 and K5 failed."));
    return false;
  }
  // Setup pvMultimeter for low resistance measurement
  if(!pvMultimeter->setupLowResistanceMeasure(mdtDeviceU3606A::Range_t::Auto, mdtDeviceU3606A::Resolution_t::Min)){
    pvLastError = pvMultimeter->lastError();
    return false;
  }
  // Check that we have R < 1 Ohm
  R = pvMultimeter->getMeasureValue();
  if(!isInRange(R, 0.0, 1.0)){
    pvLastError.updateText(tr("Calibrating resistance of realys K4 and K5 failed."));
    return false;
  }
  // Get links resistance from SENSE+ to SENSE-
  relays << "K4" << "K5";
  Rlinks = getLinkResistanceForRoute("XSENS+", "", "XSENS+", "XSENS-", "", "XSENS-", relays, ok);
  if(!ok){
    return false;
  }
  // Calibrate K4 and K5: (R-Rlinks)/2
  r = (R.value() - Rlinks) / 2.0;
  qDebug() << "R K4-K5: " << R.value() << " , Rlinks: " << Rlinks << " , r: " << r;
  if(!isInRange(r, 0.0, 1.0)){
    pvLastError.updateText(tr("Calibrating resistance of realys K4 and K5 failed."));
    return false;
  }
  setTestNodeUnitCalibrationOffset("K4", r);
  setTestNodeUnitCalibrationOffset("K5", r);
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
  R = pvMultimeter->getMeasureValue();
  if(!isInRange(R, 0.0, 1.0)){
    pvLastError.updateText(tr("Calibrating resistance of realys K3 and K6 failed."));
    return false;
  }
  // Get links resistance from SENSE+ to SENSE-
  relays.clear();
  relays << "K3" << "K6";
  Rlinks = getLinkResistanceForRoute("XSENS+", "", "XSENS+", "XSENS-", "", "XSENS-", relays, ok);
  if(!ok){
    return false;
  }
  r = (R.value() - Rlinks) / 2.0;
  qDebug() << "R K3-K6: " << R.value() << " , Rlinks: " << Rlinks << " , r: " << r;
  if(!isInRange(r, 0.0, 1.0)){
    pvLastError.updateText(tr("Calibrating resistance of realys K3 and K6 failed."));
    return false;
  }
  // Calibrate K3 and K6: (R-Rlinks)/2
  setTestNodeUnitCalibrationOffset("K3", r);
  setTestNodeUnitCalibrationOffset("K6", r);

  return true;
}

bool mdtTtBasicTestNodeCalibrationTool::checkIsoRelays()
{
  shared_ptr<mdtDeviceModbusWago> coupler;
  mdtValueDouble R;

  coupler = testNodeManager()->device<mdtDeviceModbusWago>("W750");
  Q_ASSERT(coupler);

  // Setup pvMultimeter to its max range
  if(!pvMultimeter->setupResistanceMeasure(mdtDeviceU3606A::Range_t::Max, mdtDeviceU3606A::Resolution_t::Min)){
    pvLastError = pvMultimeter->lastError();
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
  R = pvMultimeter->getMeasureValue();
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
  R = pvMultimeter->getMeasureValue();
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
  // Setup pvMultimeter to its min range
  if(!pvMultimeter->setupResistanceMeasure(mdtDeviceU3606A::Range_t::Min, mdtDeviceU3606A::Resolution_t::Min)){
    pvLastError = pvMultimeter->lastError();
    return false;
  }
  // Check that we have R < 1 Ohm
  R = pvMultimeter->getMeasureValue();
  qDebug() << "R: " << R;
  if(!isInRange(R, 0.0, 1.0)){
    pvLastError.updateText(tr("Checking resistance of realys K1 and K2 failed.\nNote: did you plug the bridge between ISO+ and ISO- ?"));
    return false;
  }

  return true;
}

bool mdtTtBasicTestNodeCalibrationTool::calibrateIsoRelays()
{
  shared_ptr<mdtDeviceModbusWago> coupler;
  QStringList relays;
  mdtValueDouble R;
  QVariant val;
  double r3, r4, r5, r6;
  double Rlinks;
  double r;
  bool ok;

  coupler = testNodeManager()->device<mdtDeviceModbusWago>("W750");
  Q_ASSERT(coupler);

  // Get R of K3, K4, K5 and K6
  val = testNodeUnitData("K3", "CalibrationOffset");
  if(val.isNull()){
    pvLastError.setError(tr("Could not get calibration offset of relay K3."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }
  r3 = val.toDouble(&ok);
  Q_ASSERT(ok);
  val = testNodeUnitData("K4", "CalibrationOffset");
  if(val.isNull()){
    pvLastError.setError(tr("Could not get calibration offset of relay K4."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }
  r4 = val.toDouble(&ok);
  Q_ASSERT(ok);
  val = testNodeUnitData("K5", "CalibrationOffset");
  if(val.isNull()){
    pvLastError.setError(tr("Could not get calibration offset of relay K5."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }
  r5 = val.toDouble(&ok);
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
  // Set K3, K4, K5, K6, K1 and K2 ON
  coupler->setDigitalOutputValue("K3", true, false, false);
  coupler->setDigitalOutputValue("K4", true, false, false);
  coupler->setDigitalOutputValue("K5", true, false, false);
  coupler->setDigitalOutputValue("K6", true, false, false);
  coupler->setDigitalOutputValue("K1", true, false, false);
  coupler->setDigitalOutputValue("K2", true, false, false);
  // Send relays states to coupler
  if(!coupler->setDigitalOutputs(true)){
    pvLastError = coupler->lastError();
    return false;
  }
  coupler->wait(50);
  // Setup pvMultimeter to its min range
  if(!pvMultimeter->setupLowResistanceMeasure(mdtDeviceU3606A::Range_t::Auto, mdtDeviceU3606A::Resolution_t::Min)){
    pvLastError = pvMultimeter->lastError();
    return false;
  }
  // Check that we have R < 1 Ohm
  R = pvMultimeter->getMeasureValue();
  qDebug() << "R: " << R;
  if(!isInRange(R, 0.0, 1.0)){
    pvLastError.updateText(tr("Calibrating resistance of realys K1 and K2 failed."));
    return false;
  }
  // Get links resistance from XISO+ to XISO-
  relays.clear();
  relays << "K1" << "K2" << "K3" << "K4" << "K5" << "K6";
  Rlinks = getLinkResistanceForRoute("XISO+", "", "XISO+", "XISO-", "", "XISO-", relays, ok);
  if(!ok){
    return false;
  }
  // Calculate resistance of K1 and K2
  r = (R.value() - Rlinks - r3- r4 - r5 - r6) / 2.0;
  qDebug() << "R K1-K2: " << R.value() << " , Rlinks: " << Rlinks << " , r: " << r;
  if(!isInRange(r, 0.0, 1.0)){
    pvLastError.updateText(tr("Calibrating resistance of realys K1 and K2 failed."));
    return false;
  }
  // Set resistances
  setTestNodeUnitCalibrationOffset("K1", r);
  setTestNodeUnitCalibrationOffset("K2", r);

  /**
  // Calibrate K1 and K2 ( (R - r4 - r6)/2 )
  Rk = (R.valueDouble() - r3- r4 - r5 - r6) / 2.0;
  if(!isInRange(Rk, 0.0, 1.0)){
    pvLastError.updateText(tr("Calibrating resistance of realys K1 and K2 failed."));
    return false;
  }
  setTestNodeUnitCalibrationOffset("K1", Rk.valueDouble());
  setTestNodeUnitCalibrationOffset("K2", Rk.valueDouble());
  */

  return true;
}

bool mdtTtBasicTestNodeCalibrationTool::checkForceRelays()
{
  shared_ptr<mdtDeviceModbusWago> coupler;
  mdtValueDouble R;

  coupler = testNodeManager()->device<mdtDeviceModbusWago>("W750");
  Q_ASSERT(coupler);

  // Setup pvMultimeter to its max range
  if(!pvMultimeter->setupResistanceMeasure(mdtDeviceU3606A::Range_t::Max, mdtDeviceU3606A::Resolution_t::Min)){
    pvLastError = pvMultimeter->lastError();
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
  R = pvMultimeter->getMeasureValue();
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
  R = pvMultimeter->getMeasureValue();
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
  // Setup pvMultimeter to its min range
  if(!pvMultimeter->setupResistanceMeasure(mdtDeviceU3606A::Range_t::Min, mdtDeviceU3606A::Resolution_t::Min)){
    pvLastError = pvMultimeter->lastError();
    return false;
  }
  // Check that we have R < 1 Ohm
  R = pvMultimeter->getMeasureValue();
  if(!isInRange(R, 0.0, 1.0)){
    pvLastError.updateText(tr("Checking resistance of realys K7 and K8 failed.\nNote: did you plug the bridge between PWR+ and PWR- ?"));
    return false;
  }

  return true;
}

bool mdtTtBasicTestNodeCalibrationTool::calibrateForceRelays()
{
  shared_ptr<mdtDeviceModbusWago> coupler;
  mdtValueDouble R;
  mdtValueDouble Rk;
  QVariant val;
  double r4, r6;
  bool ok;

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
  // Setup pvMultimeter to its min range
  if(!pvMultimeter->setupLowResistanceMeasure(mdtDeviceU3606A::Range_t::Auto, mdtDeviceU3606A::Resolution_t::Min)){
    pvLastError = pvMultimeter->lastError();
    return false;
  }
  // Check that we have R < 1 Ohm
  R = pvMultimeter->getMeasureValue();
  if(!isInRange(R, 0.0, 1.0)){
    pvLastError.updateText(tr("Calibrating resistance of realys K7 and K8 failed.\nNote: did you plug the bridge between PWR+ and PWR- ?"));
    return false;
  }
  // Calibrate K7 and K8 ( (R - r4 - r6)/2 )
  Rk = (R.value() - r4 - r6) / 2.0;
  if(!isInRange(Rk, 0.0, 1.0)){
    pvLastError.updateText(tr("Calibrating resistance of realys K7 and K8 failed.\nNote: did you plug the bridge between PWR+ and PWR- ?"));
    return false;
  }
  setTestNodeUnitCalibrationOffset("K7", Rk.value());
  setTestNodeUnitCalibrationOffset("K8", Rk.value());

  return true;
}

bool mdtTtBasicTestNodeCalibrationTool::calibrateChannelRelays()
{
  shared_ptr<mdtDeviceModbusWago> coupler;
  QSqlQuery query(database());
  QString sql;
  mdtTtTestItemNodeSetupData itemSetupData;
  mdtTtTestNodeSetupData nodeSetupData;
  mdtTtTestNodeUnitSetupData unitSetupData;
  QVariant testModelItemId;
  QSqlRecord itemRecord;
  mdtValueDouble R;
  double offset;
  QVariant val;
  bool ok;
  int i, k;
  QVariant relay1Id, relay2Id;

  coupler = testNodeManager()->device<mdtDeviceModbusWago>("W750");
  Q_ASSERT(coupler);

  // Set SQL to get TestModelItems related to TestModel with key = W750CAL
  sql = "SELECT TMI.* FROM TestModelItem_tbl TMI JOIN TestModel_tbl TM ON TM.Id_PK = TMI.TestModel_Id_FK";
  sql += " WHERE TM.Key='W750CAL'";
  sql += " ORDER BY SequenceNumber ASC";
  if(!query.exec(sql)){
    pvLastError.setError(query.lastError().text(), mdtError::Error);
    return false;
  }
  // Check that we found test model
  if(!query.next()){
    pvLastError.setError(tr("Test model with key W750CAL not found, or it is empty."), mdtError::Error);
    return false;
  }
  query.previous();
  // Setup pvMultimeter to its min range
  if(!pvMultimeter->setupResistanceMeasure(mdtDeviceU3606A::Range_t::Min, mdtDeviceU3606A::Resolution_t::Min)){
    pvLastError = pvMultimeter->lastError();
    return false;
  }
  // Run all test model items
  while(query.next()){
    itemRecord = query.record();
    testModelItemId = itemRecord.value("Id_PK");
    Q_ASSERT(!testModelItemId.isNull());
    qDebug() << "Running TMI " << itemRecord << " ...";
    // Get setup data for coupler
    nodeSetupData = test()->getTestNodeSetupData(testModelItemId, "0", ok);
    if(!ok){
      pvLastError = test()->lastError();
      return false;
    }
    // Setup coupler's routes
    if(!setupIoCouplerRoutes(nodeSetupData.routeDataList())){
      return false;
    }
    // Check that we have R < 1 Ohm
    /// \todo Adapter ... prendre valeurs du test model item, message d'erreur plus parlant, etc...
    R = pvMultimeter->getMeasureValue();
    if(!isInRange(R, 0.0, 1.0)){
      pvLastError.updateText(tr("Checking resistance of some channel realys failed."));
      ///return false;
    }
    /// \todo Adapter en cas de passage aux mesures 4 fils
    /*
     * Calculate resistance of channel relays.
     * For each test model item, we have exactly 2 routes in coupler
     * For each route in I/O coupler, we have exactly 1 coupling relay and 1 channel relay.
     * If this is not the case, something is wrong in test model (item).
     */
    // Check that we have exactly 2 routes in I/O coupler
    if(nodeSetupData.routeDataList().size() != 2){
      QString msg = tr("By channel relays calibration, a test item returned a wrong number of routes (expected: 2). ");
      msg += tr("Check test 'W750CAL' and try again.");
      pvLastError.setError(msg, mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTestNodeCalibrationTool");
      pvLastError.commit();
      return false;
    }
    Q_ASSERT(nodeSetupData.routeDataList().size() == 2);
    // Get first and second route
    auto route1 = nodeSetupData.routeDataList().at(0);
    auto route2 = nodeSetupData.routeDataList().at(1);
    // Check that we have exactly 1 channel relay in first route
    if(route1.channelRelaysIdList().size() != 1){
      QString msg = tr("By channel relays calibration, a test item returned a wrong number of channel relays (expected: 1). ");
      msg += tr("Check test 'W750CAL' and try again.");
      pvLastError.setError(msg, mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTestNodeCalibrationTool");
      pvLastError.commit();
      return false;
    }
    // Check that we have exactly 1 channel relay in second route
    if(route2.channelRelaysIdList().size() != 1){
      QString msg = tr("By channel relays calibration, a test item returned a wrong number of channel relays (expected: 1). ");
      msg += tr("Check test 'W750CAL' and try again.");
      pvLastError.setError(msg, mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTestNodeCalibrationTool");
      pvLastError.commit();
      return false;
    }
    Q_ASSERT(route1.channelRelaysIdList().size() == 1);
    Q_ASSERT(route2.channelRelaysIdList().size() == 1);
    // Get ID of relays to calibrate
    relay1Id = route1.channelRelaysIdList().at(0);
    relay2Id = route2.channelRelaysIdList().at(0);
    // Calculate offset
    /// \todo Passer route1.busCouplingRelaysResistance() en mdtValueDouble
    offset = R.value() - route1.busCouplingRelaysResistance().valueDouble() - route2.busCouplingRelaysResistance().valueDouble();
    /// \todo Check that offset is correct (>= 0, ...)
    // Set result
    setTestNodeUnitCalibrationOffset(relay1Id.toInt(), offset / 2.0);
    setTestNodeUnitCalibrationOffset(relay2Id.toInt(), offset / 2.0);
  }

  pvLastError.setError("Channel relays testing not complete", mdtError::Info);
  return false;
}

bool mdtTtBasicTestNodeCalibrationTool::setupIoCouplerRoutes(const QList< mdtTtTestModelItemRouteData >& routeDataList)
{
  shared_ptr<mdtDeviceModbusWago> coupler;
  int i, k;

  // Get coupler instance
  coupler = testNodeManager()->device<mdtDeviceModbusWago>("W750");
  Q_ASSERT(coupler);
  // Set all relays OFF
  if(coupler->setDigitalOutputsValue(false, false, false) < 0){
    pvLastError = coupler->lastError();
    return false;
  }
  // Cache all relays sates
  for(i = 0; i < routeDataList.size(); ++i){
    auto setupDataList = routeDataList.at(i).setupDataList();
    for(k = 0; k < setupDataList.size(); ++k){
      auto unitSetupData = setupDataList.at(k);
      qDebug() << "---> Setting relay " << unitSetupData.value("SchemaPosition").toString() << " ON ...";
      if(unitSetupData.ioType() == mdtTtTestNodeUnitSetupData::DigitalOutput){
        coupler->setDigitalOutputValue(unitSetupData.ioPosition(), true, false, false);
      }
    }
  }
  // Send relays states to coupler
  if(!coupler->setDigitalOutputs(true)){
    pvLastError = coupler->lastError();
    return false;
  }
  coupler->wait(50);

  return true;
}

void mdtTtBasicTestNodeCalibrationTool::addInstruments()
{
  ///testNodeManager()->addDevice<mdtDeviceModbusWago>("W750", "0", "Wago 750 coupling node");
  testNodeManager()->addDevice<mdtDeviceModbusWago>("W750");
}

bool mdtTtBasicTestNodeCalibrationTool::connectToInstruments()
{
  shared_ptr<mdtDeviceModbusWago> coupler;
  QVariant couplerAlias;

  coupler = testNodeManager()->device<mdtDeviceModbusWago>("W750");
  Q_ASSERT(coupler);

  // Get current coupler ID
  couplerAlias = currentTestNodeData("Alias");
  if(couplerAlias.isNull()){
    pvLastError.setError(tr("Test node with ID ") + couplerAlias.toString() + tr(" not found."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }
  // Connect to pvMultimeter
  /**
  if(!pvMultimeter->connectToDevice()){
    pvLastError.setError(tr("Cannot connect to ") + pvMultimeter->name(), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }
  */
  // Connect to coupler
  /**
  if(coupler->connectToDevice(mdtDeviceInfo()) != mdtAbstractPort::NoError){
    pvLastError.setError(tr("Cannot connect to ") + coupler->name(), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }
  */
  if(!coupler->detectIos()){
    pvLastError = coupler->lastError();
    return false;
  }
  // Set short labels to coupler units
  if(!testNodeManager()->setDeviceIosLabelShort(couplerAlias.toString())){
    pvLastError = testNodeManager()->lastError();
    disconnectFromInstruments();
    return false;
  }
  // Reset pvMultimeter to factory state
  /**
  if(pvMultimeter->sendCommand("*RST") < 0){
    pvLastError.setError(tr("*RST failed on device ") + pvMultimeter->name(), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }
  */

  return true;
}

void mdtTtBasicTestNodeCalibrationTool::disconnectFromInstruments()
{
  Q_ASSERT(testNodeManager()->container());

  shared_ptr<mdtDeviceModbusWago> coupler;

  coupler = testNodeManager()->device<mdtDeviceModbusWago>("W750");
  Q_ASSERT(coupler);

  // Set all relays OFF
  if(coupler->setDigitalOutputsValue(false, true, true) < 0){
    pvLastError = coupler->lastError();
  }
  // Reset pvMultimeter to factory state
  /// \todo Why ??
//   if(pvMultimeter->sendCommand("*RST") < 0){
//     pvLastError.setError(tr("*RST failed on device ") + pvMultimeter->name(), mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTestNodeCalibrationTool");
//     pvLastError.commit();
//   }
  // Disconnect multimeter
  pvMultimeter->disconnectFromDevice();

  // Disconnect
  testNodeManager()->container()->disconnectFromDevices();
}

double mdtTtBasicTestNodeCalibrationTool::getLinkResistanceForRoute(const QString & schemaPositionA, const QString & connectorA, const QString & contactA,
                                                                    const QString & schemaPositionB, const QString & connectorB, const QString & contactB,
                                                                    const QStringList & relaysToEnable, bool & ok)
{
  QVariant testNodeId;
  QList<mdtClLinkData> linkDataList;

  testNodeId = currentTestNodeData("VehicleType_Id_FK_PK");
  if(testNodeId.isNull()){
    ok = false;
    return 0.0;
  }
  linkDataList = test()->getLinkDataListForRoute(testNodeId, schemaPositionA, connectorA, contactA, schemaPositionB, connectorB, contactB, relaysToEnable, pvGraph, ok);
  if(!ok){
    pvLastError = test()->lastError();
    return 0.0;
  }

  return test()->linkPathResistance(linkDataList);
}

bool mdtTtBasicTestNodeCalibrationTool::fritLine()
{
  Q_ASSERT(testNodeManager()->container());

  if(!pvMultimeter->setupVoltageDcMeasure(mdtDeviceU3606A::Range_t::Range10, mdtDeviceU3606A::Resolution_t::Min)){
    pvLastError = pvMultimeter->lastError();
    return false;
  }
  if(!pvMultimeter->setOutputState(false)){
    pvLastError = pvMultimeter->lastError();
    return false;
  }
  if(!pvMultimeter->setSourceRange(mdtDeviceU3606A::SourceRange_t::S1_30V1A)){
    pvLastError = pvMultimeter->lastError();
    return false;
  }
  if(!pvMultimeter->setSourceCurrent(1.0)){
    pvLastError = pvMultimeter->lastError();
    return false;
  }
  if(!pvMultimeter->setSourceVoltageLimit(30.0)){
    pvLastError = pvMultimeter->lastError();
    return false;
  }
  if(!pvMultimeter->setOutputState(true)){
    pvLastError = pvMultimeter->lastError();
    return false;
  }
  pvMultimeter->wait(50);
  /// tetss..
  qDebug() << "U (=R): " << pvMultimeter->getMeasureValue();
  pvMultimeter->wait(1000);
  qDebug() << "U (=R): " << pvMultimeter->getMeasureValue();
  pvMultimeter->wait(1000);
  qDebug() << "U (=R): " << pvMultimeter->getMeasureValue();
  pvMultimeter->wait(1000);
  qDebug() << "U (=R): " << pvMultimeter->getMeasureValue();
  pvMultimeter->wait(1000);
  qDebug() << "U (=R): " << pvMultimeter->getMeasureValue();
  pvMultimeter->wait(1000);
  qDebug() << "U (=R): " << pvMultimeter->getMeasureValue();
  pvMultimeter->wait(1000);
  
  if(!pvMultimeter->setOutputState(false)){
    pvLastError = pvMultimeter->lastError();
    return false;
  }
  pvMultimeter->wait(50);

  return true;
}
