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
#include "mdtTtTestCableOffsetTool.h"
#include "mdtTtTestNodeManager.h"
#include "mdtDeviceU3606A.h"
#include "mdtDeviceModbusWago.h"
#include "mdtTtTestItemNodeSetupData.h"
#include "mdtTtTestNodeSetupData.h"
#include "mdtTtTestNodeUnitSetupData.h"
#include "mdtTtBase.h"
#include "mdtValue.h"
#include "mdtSqlRecord.h"
#include <QSqlQuery>
#include <QSqlError>

#include <QDebug>

using std::shared_ptr;

mdtTtTestCableOffsetTool::mdtTtTestCableOffsetTool(QSqlDatabase db, QObject* parent)
 : QObject(parent),
   pvDatabase(db),
   pvTestNodeManager(new mdtTtTestNodeManager(0, db)),
   pvTest(new mdtTtTest(0, db))
{
  /*
   * pvParentWidget is used as parent for dialogs.
   * If parent is not a QWidget (or a subclass of QWidget), dynamic_cast will return a null pointer,
   *  ans thats OK, dialogs will also simply be application modal.
   */
  pvParentWidget = dynamic_cast<QWidget*>(parent);
  // Add instruments
  addInstruments();
}

void mdtTtTestCableOffsetTool::setTestLinkTableController(std::shared_ptr< mdtSqlTableViewController > controller)
{
  Q_ASSERT(controller);

  pvTestLinkTableController = controller;
}

bool mdtTtTestCableOffsetTool::runOffsetReset(const QVariant& testModelId)
{
  Q_ASSERT(pvTestLinkTableController);

  QString sql;
  QSqlQuery query(pvDatabase);
  shared_ptr<mdtDeviceU3606A> multimeter;
  QSqlRecord itemRecord;
  QVariant testModelItemId;
  mdtValue R;
  QList<QSqlRecord> logicalTestLinks;
  bool ok;
  int linkCount;
  int i;
  double r;

  // Get instance of multimeter
  multimeter = pvTestNodeManager->device<mdtDeviceU3606A>("U3606A");
  Q_ASSERT(multimeter);
  // Connect to instruments
  if(!connectToInstruments()){
    displayLastError();
    return false;
  }
  // Set SQL to get TestModelItems related to TestModel with given ID
  sql = "SELECT TMI.* FROM TestModelItem_tbl TMI JOIN TestModel_tbl TM ON TM.Id_PK = TMI.TestModel_Id_FK";
  sql += " WHERE TM.Id_PK = " + testModelId.toString();
  sql += " ORDER BY SequenceNumber ASC";
  if(!query.exec(sql)){
    pvLastError.setError(query.lastError().text(), mdtError::Error);
    displayLastError();
    disconnectFromInstruments();
    return false;
  }
  // Setup multimeter to its min range
  if(multimeter->setupResistanceMeasure(mdtDeviceU3606A::RangeMin, mdtDeviceU3606A::ResolutionMin) < 0){
    pvLastError = multimeter->lastError();
    displayLastError();
    disconnectFromInstruments();
    return false;
  }
  // Run all test model items
  while(query.next()){
    itemRecord = query.record();
    testModelItemId = itemRecord.value("Id_PK");
    Q_ASSERT(!testModelItemId.isNull());
    // Setup I/O test node
    if(!setupTestNodes(testModelItemId)){
      displayLastError();
      disconnectFromInstruments();
      return false;
    }
    // Get R
    R = multimeter->getMeasureValue();
    if(!R.isValid()){
      pvLastError = multimeter->lastError();
      displayLastError();
      disconnectFromInstruments();
      return false;
    }
    // Get logical test links
    logicalTestLinks = getLogicalTestLinkData(testModelItemId, ok);
    if(!ok){
      displayLastError();
      disconnectFromInstruments();
      return false;
    }
    // Update
    linkCount = logicalTestLinks.size();
    if(linkCount < 1){
      continue;
    }
    Q_ASSERT(linkCount > 0);
    r = R.valueDouble() / static_cast<double>(linkCount);
    for(i = 0; i < linkCount; ++i){
      if(!pvTestLinkTableController->setData("UnitConnectionStart_Id_FK", logicalTestLinks.at(i).value("TestCableUnitConnectionStart_Id_FK"),
                                             "UnitConnectionEnd_Id_FK", logicalTestLinks.at(i).value("TestCableUnitConnectionEnd_Id_FK"),
                                             "Value", r, false) )
      {
        pvLastError = pvTestLinkTableController->lastError();
        displayLastError();
        disconnectFromInstruments();
        return false;
      }
    }
  }
  // Disconnect
  disconnectFromInstruments();
  // Save
  if(!saveOffsetValues()){
    return false;
  }

  return true;
}

QList< QSqlRecord > mdtTtTestCableOffsetTool::getLogicalTestLinkData(const QVariant& testModelItemId, bool & ok)
{
  mdtTtBase b(0, pvDatabase);
  QList<QSqlRecord> dataList;
  QString sql;

  sql = "SELECT TL.Id_PK, TL.TestCableUnitConnectionStart_Id_FK, TL.TestCableUnitConnectionEnd_Id_FK";
  sql += " FROM TestModelItem_TestLink_tbl TMITL JOIN TestLink_tbl TL ON TL.Id_PK = TMITL.TestLink_Id_FK";
  sql += " WHERE TMITL.TestModelItem_Id_FK = " + testModelItemId.toString();
  dataList = b.getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    pvLastError = b.lastError();
    return dataList;
  }

  return dataList;
}

bool mdtTtTestCableOffsetTool::saveOffsetValues()
{
  Q_ASSERT(pvTestLinkTableController);

  mdtTtBase b(0, pvDatabase);
  int N;
  int i;
  QVariant startCnxId;
  QVariant endCnxId;
  QVariant r;
  bool ok;
  mdtSqlRecord record;

  // Setup record with fields to update
  /**
  if(!record.addField("UnitConnectionStart_Id_FK", "Link_tbl", pvDatabase)){
    pvLastError = record.lastError();
    return false;
  }
  if(!record.addField("UnitConnectionEnd_Id_FK", "Link_tbl", pvDatabase)){
    pvLastError = record.lastError();
    return false;
  }
  */
  if(!record.addField("Value", "Link_tbl", pvDatabase)){
    pvLastError = record.lastError();
    return false;
  }
  // Beginn a transaction
  if(!beginTransaction()){
    return false;
  }
  // Save each value
  N = pvTestLinkTableController->rowCount(true);
  for(i = 0; i < N; ++i){
    // Get cached values (from UnitLink_view)
    startCnxId = pvTestLinkTableController->data(i, "UnitConnectionStart_Id_FK", ok);
    if(!ok){
      pvLastError = pvTestLinkTableController->lastError();
      rollbackTransaction();
      return false;
    }
    endCnxId = pvTestLinkTableController->data(i, "UnitConnectionEnd_Id_FK", ok);
    if(!ok){
      pvLastError = pvTestLinkTableController->lastError();
      rollbackTransaction();
      return false;
    }
    r = pvTestLinkTableController->data(i, "Value", ok);
    if(!ok){
      pvLastError = pvTestLinkTableController->lastError();
      rollbackTransaction();
      return false;
    }
    // Save to Link_tbl
    record.clearValues();
    record.setValue("Value", r);
    if(!b.updateRecord("Link_tbl", record, "UnitConnectionStart_Id_FK", startCnxId, "UnitConnectionEnd_Id_FK", endCnxId)){
      pvLastError = b.lastError();
      rollbackTransaction();
      return false;
    }
  }
  // Commit trasaction
  if(!commitTransaction()){
    return false;
  }

  return true;
}

bool mdtTtTestCableOffsetTool::setupTestNodes(const QVariant& testModelItemId)
{
  mdtTtTestItemNodeSetupData itemSetupData;
  mdtTtTestNodeSetupData nodeSetupData;
  bool ok;

  itemSetupData = pvTest->getSetupData(testModelItemId, ok);
  if(!ok){
    pvLastError = pvTest->lastError();
    displayLastError();
    return false;
  }
  while(itemSetupData.hasMoreStep()){
    nodeSetupData = itemSetupData.getNextStep();
    ///qDebug() << " Has setup for node " << nodeSetupData.nodeIdentification();
    if(nodeSetupData.nodeIdentification() == "0"){
      if(!setupIoCoupler(nodeSetupData)){
        return false;
      }
    }else{
      pvLastError.setError(tr("Have setup for a unknown node: ") + nodeSetupData.nodeIdentification(), mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestCableOffsetTool");
      pvLastError.commit();
      return false;
    }
  }

  return true;

}

bool mdtTtTestCableOffsetTool::setupIoCoupler(const mdtTtTestNodeSetupData& nodeSetupData/**, const QString& nodeIdentification*/)
{
  QList<mdtTtTestNodeUnitSetupData> unitSetupDataList;
  mdtTtTestNodeUnitSetupData unitSetupData;
  shared_ptr<mdtDeviceModbusWago> coupler;
  int i;

  coupler = pvTestNodeManager->device<mdtDeviceModbusWago>("W750");
  Q_ASSERT(coupler);
  // Set all outputs to 0
  /// \todo Set analog outputs to 0
  coupler->setDigitalOutputsValue(false, false, false);
  // Set values of each I/O
  unitSetupDataList = nodeSetupData.unitSetupList();
  for(i = 0; i < unitSetupDataList.size(); ++i){
    unitSetupData = unitSetupDataList.at(i);
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
      case mdtTtTestNodeUnitSetupData::AnalogInput:
      case mdtTtTestNodeUnitSetupData::DigitalInput:
      case mdtTtTestNodeUnitSetupData::Unknown:
        // Nothing to do
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

  return true;
}

void mdtTtTestCableOffsetTool::displayLastError()
{
  QMessageBox msgBox(pvParentWidget);

  msgBox.setText(pvLastError.text());
  msgBox.setInformativeText(pvLastError.informativeText());
  msgBox.setDetailedText(pvLastError.systemText());
  msgBox.setIcon(pvLastError.levelIcon());
  msgBox.exec();
}

void mdtTtTestCableOffsetTool::displayMessage(const QString& text, const QString& informativeText, QMessageBox::Icon icon)
{
  QMessageBox msgBox(pvParentWidget);

  msgBox.setText(text);
  msgBox.setInformativeText(informativeText);
  msgBox.setIcon(icon);
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.exec();
}

bool mdtTtTestCableOffsetTool::promptUser(const QString& text, const QString& informativeText, QMessageBox::Icon icon, QMessageBox::StandardButtons buttons)
{
  return askUser(text, informativeText, icon, buttons);
}

bool mdtTtTestCableOffsetTool::askUser(const QString& text, const QString& informativeText, QMessageBox::Icon icon, QMessageBox::StandardButtons buttons)
{
  QMessageBox msgBox(pvParentWidget);

  msgBox.setText(text);
  msgBox.setInformativeText(informativeText);
  msgBox.setIcon(icon);
  msgBox.setStandardButtons(buttons);
  switch(msgBox.exec()){
    case QMessageBox::Ok:
    case QMessageBox::Open:
    case QMessageBox::Save:
    case QMessageBox::Apply:
    case QMessageBox::SaveAll:
    case QMessageBox::Yes:
    case QMessageBox::YesToAll:
      return true;
    default:
      return false;
  }
}

bool mdtTtTestCableOffsetTool::isInRange(const mdtValue & value, double min, double max)
{
  double x;

  // Get double value
  if(value.isMinusOl()){
    x = -std::numeric_limits<double>::max();
  }else if(value.isPlusOl()){
    x = std::numeric_limits<double>::max();
  }else{
    x = value.valueDouble();
  }
  // Check range
  if(!mdtTtTest::isInRange(x, min, max)){
    QString msg;
    pvLastError.setError(tr("Range checking failed for given value."), mdtError::Error);
    msg  = tr("Value") + " " + QString::number(x) + " " + tr("is out of range.") + " ";
    msg += tr("Allowed range is: ") + "[" + QString::number(min) + ";" + QString::number(max) + "]";
    pvLastError.setInformativeText(msg);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestCableOffsetTool");
    pvLastError.commit();
    return false;
  }

  return true;
}

void mdtTtTestCableOffsetTool::addInstruments()
{
  pvTestNodeManager->addDevice<mdtDeviceU3606A>("U3606A", "", "U3606A Multimeter");
  pvTestNodeManager->addDevice<mdtDeviceModbusWago>("W750", "0", "Wago 750 coupling node");
}

bool mdtTtTestCableOffsetTool::connectToInstruments()
{
  shared_ptr<mdtDeviceU3606A> multimeter;
  shared_ptr<mdtDeviceModbusWago> coupler;
  QVariant couplerNodeId;

  multimeter = pvTestNodeManager->device<mdtDeviceU3606A>("U3606A");
  Q_ASSERT(multimeter);
  coupler = pvTestNodeManager->device<mdtDeviceModbusWago>("W750");
  Q_ASSERT(coupler);

  // Get current coupler ID
  /// \todo Provisoire !
  couplerNodeId = "0";
  /**
  couplerNodeId = currentTestNodeData("VehicleType_Id_FK_PK");
  if(couplerNodeId.isNull()){
    pvLastError.setError(tr("Test node with ID ") + couplerNodeId.toString() + tr(" not found."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestCableOffsetTool");
    pvLastError.commit();
    return false;
  }
  */
  // Connect to multimeter
  if(multimeter->connectToDevice(mdtDeviceInfo()) != mdtAbstractPort::NoError){
    pvLastError.setError(tr("Cannot connect to ") + multimeter->name(), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestCableOffsetTool");
    pvLastError.commit();
    return false;
  }
  // Connect to coupler
  if(coupler->connectToDevice(mdtDeviceInfo()) != mdtAbstractPort::NoError){
    pvLastError.setError(tr("Cannot connect to ") + coupler->name(), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestCableOffsetTool");
    pvLastError.commit();
    return false;
  }
  if(!coupler->detectIos()){
    pvLastError = coupler->lastError();
    return false;
  }
  // Set short labels to coupler units
  if(!pvTestNodeManager->setDeviceIosLabelShort(couplerNodeId, "W750")){
    pvLastError = pvTestNodeManager->lastError();
    disconnectFromInstruments();
    return false;
  }
  // Reset multimeter to factory state
  if(multimeter->sendCommand("*RST") < 0){
    pvLastError.setError(tr("*RST failed on device ") + multimeter->name(), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestCableOffsetTool");
    pvLastError.commit();
    return false;
  }

  return true;
}

void mdtTtTestCableOffsetTool::disconnectFromInstruments()
{
  Q_ASSERT(pvTestNodeManager->container());

  shared_ptr<mdtDeviceU3606A> multimeter;
  shared_ptr<mdtDeviceModbusWago> coupler;

  multimeter = pvTestNodeManager->device<mdtDeviceU3606A>("U3606A");
  Q_ASSERT(multimeter);
  coupler = pvTestNodeManager->device<mdtDeviceModbusWago>("W750");
  Q_ASSERT(coupler);

  // Set all relays OFF
  if(coupler->setDigitalOutputsValue(false, true, true) < 0){
    pvLastError = coupler->lastError();
  }
  // Reset multimeter to factory state
  if(multimeter->sendCommand("*RST") < 0){
    pvLastError.setError(tr("*RST failed on device ") + multimeter->name(), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestCableOffsetTool");
    pvLastError.commit();
  }

  // Disconnect
  pvTestNodeManager->container()->disconnectFromDevices();
}

bool mdtTtTestCableOffsetTool::beginTransaction() 
{
  if(!pvDatabase.transaction()){
    QSqlError sqlError = pvDatabase.lastError();
    pvLastError.setError(tr("Cannot beginn transaction (database: '") + pvDatabase.databaseName() + tr("')."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestCableOffsetTool");
    pvLastError.commit();
    return false;
  }
  return true;
}

bool mdtTtTestCableOffsetTool::rollbackTransaction() 
{
  if(!pvDatabase.rollback()){
    QSqlError sqlError = pvDatabase.lastError();
    pvLastError.setError(tr("Cannot beginn rollback (database: '") + pvDatabase.databaseName() + tr("')."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestCableOffsetTool");
    pvLastError.commit();
    return false;
  }
  return true;
}

bool mdtTtTestCableOffsetTool::commitTransaction() 
{
  if(!pvDatabase.commit()){
    QSqlError sqlError = pvDatabase.lastError();
    pvLastError.setError(tr("Cannot beginn commit (database: '") + pvDatabase.databaseName() + tr("')."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestCableOffsetTool");
    pvLastError.commit();
    return false;
  }
  return true;
}
