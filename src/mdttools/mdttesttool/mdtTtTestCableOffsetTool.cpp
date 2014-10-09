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

void mdtTtTestCableOffsetTool::runOffsetReset()
{
  Q_ASSERT(pvTestLinkTableController);

  // Connect to instruments
  if(!connectToInstruments()){
    displayLastError();
    return;
  }
  
  
  
  // Disconnect
  disconnectFromInstruments();
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
