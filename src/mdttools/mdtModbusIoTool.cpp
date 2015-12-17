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
#include "mdtModbusIoTool.h"
#include "mdtDeviceModbusWago.h"
#include "mdtDeviceIosWidget.h"
#include "mdtApplication.h"
#include "mdtDeviceInfo.h"
#include "mdtPortStatusWidget.h"
#include "mdtPortManager.h"
#include "mdtModbusTcpPortManager.h"
#include "mdtModbusTcpPortSetupDialog.h"
#include "mdtModbusHwNodeId.h"
#include "mdtPortInfo.h"
#include "mdtErrorOut.h"
#include <QList>
#include <QMessageBox>

mdtModbusIoTool::mdtModbusIoTool(QWidget *parent, Qt::WindowFlags flags)
 : QMainWindow(parent, flags)
{
  // Setup GUI
  setupUi(this);
  // Setup status bar
  pvStatusWidget = new mdtPortStatusWidget;
  statusBar()->addWidget(pvStatusWidget);
  // Setup I/Os widget
  pvDeviceIosWidget = new mdtDeviceIosWidget;
  saIos->setWidget(pvDeviceIosWidget);
  // Set some flags
  pvConnectingToNode = false;
  // Actions
  connect(action_Setup, SIGNAL(triggered()), this, SLOT(setup()));
  pvLanguageActionGroup = nullptr;
  // Connect/disconnect actions
  connect(actConnect, SIGNAL(triggered()), this, SLOT(connectToDeviceSlot()));
  connect(actAbortScan, SIGNAL(triggered()), this, SLOT(abortScan()));
  connect(actDisconnect, SIGNAL(triggered()), this, SLOT(disconnectFromDeviceSlot()));

  setWindowTitle(tr("MODBUS I/O tool"));
}

mdtModbusIoTool::~mdtModbusIoTool()
{
}

void mdtModbusIoTool::setAvailableTranslations(const QMap<QString, QString> &avaliableTranslations, const QString &currentTranslationKey)
{
  QMap<QString, QString>::const_iterator it = avaliableTranslations.constBegin();

  // Create a action group
  if(pvLanguageActionGroup == nullptr){
    pvLanguageActionGroup = new QActionGroup(this);
    connect(pvLanguageActionGroup, SIGNAL(triggered(QAction*)), mdtApp, SLOT(changeLanguage(QAction*)));
  }
  // Travel available translation and add actions to menu + group
  while(it != avaliableTranslations.constEnd()){
    QAction *action = new QAction(it.value(), this);
    action->setCheckable(true);
    action->setData(it.key());
    if(it.key() == currentTranslationKey){
      action->setChecked(true);
    }
    menu_Language->addAction(action);
    pvLanguageActionGroup->addAction(action);
    it++;
  }
}

void mdtModbusIoTool::retranslate()
{
  retranslateUi(this);
}

void mdtModbusIoTool::setState(mdtDevice::State_t state)
{
  switch(state){
    case mdtDevice::State_t::Connecting:
      setStateConnecting();
      break;
    case mdtDevice::State_t::Disconnected:
      setStateDisconnected();
      break;
    case mdtDevice::State_t::Ready:
      setStateReady();
      break;
    case mdtDevice::State_t::Error:
      setStateError();
      break;
  }
}

void mdtModbusIoTool::setModbusDevice(const std::shared_ptr<mdtDeviceModbus> & device)
{
  Q_ASSERT(device);

  /*
   * If we allready have a device,
   *  disconnect from physical decvice,
   *  and diconnect signals/slots first
   */
  if(pvDeviceModbus){
    pvDeviceModbus->disconnectFromDevice();
    disconnect(pvDeviceModbus.get(), SIGNAL(stateChanged(mdtDevice::State_t)), this, SLOT(setState(mdtDevice::State_t)));
    disconnect(pvDeviceModbus.get(), SIGNAL(statusMessageChanged(const QString&, const QString&, int)), this, SLOT(showStatusMessage(const QString&, const QString&, int)));
    disconnect(pvDeviceModbus->portManager(), SIGNAL(stateChangedForUi(int, const QString&, int, bool)), pvStatusWidget, SLOT(setState(int, const QString&, int, bool)));
  }
  // Store given device
  pvDeviceModbus = device;
  Q_ASSERT(pvDeviceModbus->portManager() != 0);
  // Make signal/slots connections
  connect(pvDeviceModbus.get(), SIGNAL(stateChanged(mdtDevice::State_t)), this, SLOT(setState(mdtDevice::State_t)));
  connect(pvDeviceModbus.get(), SIGNAL(statusMessageChanged(const QString&, const QString&, int)), this, SLOT(showStatusMessage(const QString&, const QString&, int)));
  connect(pvDeviceModbus->portManager(), SIGNAL(stateChangedForUi(int, const QString&, int, bool)), pvStatusWidget, SLOT(setState(int, const QString&, int, bool)));

  pvDeviceModbus->portManager()->notifyCurrentState();
  // Start periodic inputs query (will only start once device is ready, see mdtDevice doc for details)
  /// \todo Clarify and implement
//   pvDeviceModbus->start(100);

}

bool mdtModbusIoTool::connectToDevice()
{
  Q_ASSERT(pvDeviceModbus);

  mdtModbusHwNodeId hwNodeId;
  mdtModbusTcpPortManager *m = pvDeviceModbus->modbusTcpPortManager();
  Q_ASSERT(m != 0);

  if(!m->isClosed()){
    pvLastError.setError(tr("Please disconnect before reconnect"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtModbusIoTool");
    pvLastError.commit();
    return false;
  }
  // Update GUI state
  setStateConnectingToNode();
  // Try to find and connect to device
  hwNodeId.setId(sbHwNodeId->value(), 8, 0);
  if(!pvDeviceModbus->connectToDevice(hwNodeId, "Wago 750 I/O", 100, 502)){
    pvLastError = pvDeviceModbus->lastError();
    setStateConnectingToNodeFinished();
    return false;
  }
  showStatusMessage(tr("I/O detection ..."));
  if(!pvDeviceModbus->detectIos()){
    pvLastError = pvDeviceModbus->lastError();
    pvDeviceModbus->disconnectFromDevice();
    setStateConnectingToNodeFinished();
    return false;
  }
  pvDeviceModbus->ios()->setIosDefaultLabelShort();
  updateIosWidget();
  showStatusMessage(tr("I/O detection done"), 1000);
  pvStatusDeviceInformations = tr("Connected to device with HW node ID ") + QString::number(sbHwNodeId->value());
  setStateConnectingToNodeFinished();
  // Start periodic inputs querying
  pvDeviceModbus->start(100);

  return true;
}

void mdtModbusIoTool::disconnectFromDevice()
{
  pvStatusWidget->clearMessage();
  if(pvDeviceModbus){
    pvDeviceModbus->stop();
    pvDeviceModbus->disconnectFromDevice();
  }
  pvDeviceIosWidget->clearIoWidgets();
}

void mdtModbusIoTool::updateIosWidget()
{
  Q_ASSERT(pvDeviceModbus);

  pvDeviceIosWidget->clearIoWidgets();
  pvDeviceIosWidget->setDeviceIos(pvDeviceModbus->ios());
  pvDeviceModbus->getDigitalOutputs(0);
}

void mdtModbusIoTool::displayLastError()
{
  QMessageBox msgBox(this);

  msgBox.setText(pvLastError.text());
  msgBox.setInformativeText(pvLastError.informativeText());
  msgBox.setDetailedText(pvLastError.systemText());
  ///msgBox.setIcon(pvLastError.levelIcon());
  msgBox.exec();
}

void mdtModbusIoTool::setup()
{
  Q_ASSERT(pvDeviceModbus);

  // If we are connecting to node, we not accept setup (can conflict)
//   if(pvDeviceModbus->currentState() != mdtDevice::State_t::Disconnected){
//     
//   }
  if(pvConnectingToNode){
    QMessageBox msgBox;
    msgBox.setText(tr("Cannot run setup while connecting to node, please abort and retry"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();
    return;
  }
  mdtModbusTcpPortSetupDialog d(this);
  d.setPortManager(pvDeviceModbus->portManager());
  d.exec();
  // If portManager is running, we can try to detect I/Os
  if(pvDeviceModbus->portManager()->isReady()){
    /**
    if(!pvDeviceModbusWago->isWago750()){
      showStatusMessage(tr("Device is not a Wago 750 Fieldbus"), tr("I/O detetcion currently only works with Wago 750 fielbus coupler"));
      pvDeviceModbusWago->portManager()->stop();
      return;
    }
    */
    showStatusMessage(tr("I/O detection ..."));
    if(!pvDeviceModbus->detectIos()){
      showStatusMessage(tr("I/O detection failed"));
      pvDeviceModbus->portManager()->stop();
      return;
    }
    pvDeviceIosWidget->setDeviceIos(pvDeviceModbus->ios());
    pvDeviceModbus->getDigitalOutputs(0);
    showStatusMessage(tr("I/O detection done"), 1000);
  }
}

void mdtModbusIoTool::connectToDeviceSlot()
{
  if(!connectToDevice()){
    displayLastError();
  }
}

void mdtModbusIoTool::disconnectFromDeviceSlot()
{
  disconnectFromDevice();
}

void mdtModbusIoTool::showStatusMessage(const QString & message, int timeout)
{
  pvStatusWidget->showMessage(message, QString(), timeout);
}

void mdtModbusIoTool::showStatusMessage(const QString & message, const QString & details, int timeout)
{
  pvStatusWidget->showMessage(message, details, timeout);
}

void mdtModbusIoTool::abortScan()
{
  if(pvDeviceModbus){
    Q_ASSERT(pvDeviceModbus->portManager() != nullptr);
    pvDeviceModbus->modbusTcpPortManager()->abortScan();
  }
}

void mdtModbusIoTool::setStateConnectingToNode()
{
  qDebug() << " o Set state connecting to node ..";
  pvConnectingToNode = true;
  actDisconnect->setEnabled(false);
  actConnect->setEnabled(false);
  actAbortScan->setEnabled(true);
  sbHwNodeId->setEnabled(false);
}

void mdtModbusIoTool::setStateConnectingToNodeFinished()
{
  Q_ASSERT(pvDeviceModbus);

  qDebug() << " o Set state connecting to node finished ..";
  
  pvConnectingToNode = false;
  pvDeviceModbus->portManager()->notifyCurrentState();
}

void mdtModbusIoTool::setStateDisconnected()
{
  qDebug() << " o Set state disconnected ..";

  if(!pvConnectingToNode){
    actDisconnect->setEnabled(false);
    actConnect->setEnabled(true);
    actAbortScan->setEnabled(false);
    sbHwNodeId->setEnabled(true);
    pvStatusWidget->setPermanentText("");
  }
}

void mdtModbusIoTool::setStateConnecting()
{
  qDebug() << " o Set state connecting ..";
  
  if(!pvConnectingToNode){
    actDisconnect->setEnabled(false);
    actConnect->setEnabled(false);
    actAbortScan->setEnabled(true);
    sbHwNodeId->setEnabled(false);
    pvStatusWidget->setPermanentText("");
  }
}

void mdtModbusIoTool::setStateReady()
{
  qDebug() << " o Set state ready ..";
  
  if(!pvConnectingToNode){
    actDisconnect->setEnabled(true);
    actConnect->setEnabled(false);
    actAbortScan->setEnabled(false);
    sbHwNodeId->setEnabled(false);
    pvStatusWidget->setPermanentText(pvStatusDeviceInformations);
  }
}

void mdtModbusIoTool::setStateBusy()
{
  qDebug() << " o Set state busy ..";
  
  if(!pvConnectingToNode){
    actDisconnect->setEnabled(true);
    actConnect->setEnabled(false);
    actAbortScan->setEnabled(false);
    sbHwNodeId->setEnabled(false);
  }
}

void mdtModbusIoTool::setStateError()
{
  qDebug() << " o Set state error ..";
  
  if(!pvConnectingToNode){
    actDisconnect->setEnabled(true);
    actConnect->setEnabled(false);
    actAbortScan->setEnabled(false);
    sbHwNodeId->setEnabled(false);
  }
}
