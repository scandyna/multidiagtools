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

  // Setup device
  ///pvDeviceModbusWago = new mdtDeviceModbusWago(this);
//   pvDeviceModbus.reset(new mdtDeviceModbusWago);
  ///pvDeviceModbusWago->setName("Wago I/O 750");
  // Setup I/Os widget
  pvDeviceIosWidget = new mdtDeviceIosWidget;
  saIos->setWidget(pvDeviceIosWidget);
  // Make connections
//   connect(pvDeviceModbus.get(), SIGNAL(stateChanged(int)), this, SLOT(setState(int)));
//   connect(pvDeviceModbus.get(), SIGNAL(statusMessageChanged(const QString&, const QString&, int)), this, SLOT(showStatusMessage(const QString&, const QString&, int)));
//   Q_ASSERT(pvDeviceModbus->portManager() != 0);
//   connect(pvDeviceModbus->portManager(), SIGNAL(stateChangedForUi(int, const QString&, int, bool)), pvStatusWidget, SLOT(setState(int, const QString&, int, bool)));
  /**
  connect(pvDeviceModbusWago, SIGNAL(stateChanged(int)), this, SLOT(setState(int)));
  connect(pvDeviceModbusWago, SIGNAL(statusMessageChanged(const QString&, const QString&, int)), this, SLOT(showStatusMessage(const QString&, const QString&, int)));
  Q_ASSERT(pvDeviceModbusWago->portManager() != 0);
  connect(pvDeviceModbusWago->portManager(), SIGNAL(stateChangedForUi(int, const QString&, int, bool)), pvStatusWidget, SLOT(setState(int, const QString&, int, bool)));
  */

  /// \note Test
//   connect(this, SIGNAL(errorEvent()), pvDeviceModbus->portManager(), SIGNAL(pmUnhandledErrorEvent()));
  
  // Set some flags
  pvReady = false;
  pvConnectingToNode = false;
    // Actions
  connect(action_Setup, SIGNAL(triggered()), this, SLOT(setup()));
  pvLanguageActionGroup = 0;
  // Buttons
  ///connect(pbConnect, SIGNAL(clicked()), this, SLOT(connectToNode()));
  connect(actConnect, SIGNAL(triggered()), this, SLOT(connectToNode()));
  ///connect(pbDisconnect, SIGNAL(clicked()), this, SLOT(disconnectFromNode()));
  connect(actDisconnect, SIGNAL(triggered()), this, SLOT(disconnectFromNode()));
  
  ///actConnect->setVisible(false);
  
//   connect(pbAbortScan, SIGNAL(clicked()), pvDeviceModbus->modbusTcpPortManager(), SLOT(abortScan()));
  setWindowTitle(tr("MODBUS I/O tool for Wago 750"));
//   pvDeviceModbus->portManager()->notifyCurrentState();
  // Start periodic inputs query (will only start once device is ready, see mdtDevice doc for details)
//   pvDeviceModbus->start(100);
}

mdtModbusIoTool::~mdtModbusIoTool()
{
}

void mdtModbusIoTool::setAvailableTranslations(const QMap<QString, QString> &avaliableTranslations, const QString &currentTranslationKey)
{
  QMap<QString, QString>::const_iterator it = avaliableTranslations.constBegin();

  // Create a action group
  if(pvLanguageActionGroup == 0){
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
  ///qDebug() << "mdtModbusIoTool - new state: " << pvDeviceModbusWago->currentState() << " ...";
//   switch((mdtPortManager::state_t)state){
//     case mdtPortManager::PortClosed:
//       setStatePortClosed();
//       break;
//     case mdtPortManager::Disconnected:
//       setStateDisconnected();
//       break;
//     case mdtPortManager::Connecting:
//       setStateConnecting();
//       break;
//     case mdtPortManager::Ready:
//       setStateReady();
//       break;
//     case mdtPortManager::Busy:
//       setStateBusy();
//       break;
//     case mdtPortManager::PortError:
//       setStateError();
//       break;
//   }
  ///qDebug() << "mdtModbusIoTool - new state: " << pvDeviceModbusWago->currentState();
}

void mdtModbusIoTool::setModbusDevice(const std::shared_ptr<mdtDeviceModbus> & device)
{
  Q_ASSERT(device);

  /*
   * If we allready have a device,
   *  disconnect from physical decvice,
   *  diconnect signals/slots first
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
  /// \todo Abort, etc.. buttons
  pvDeviceModbus->portManager()->notifyCurrentState();
  // Start periodic inputs query (will only start once device is ready, see mdtDevice doc for details)
  /// \todo Clarify and implement
//   pvDeviceModbus->start(100);

}

void mdtModbusIoTool::updateIosWidget()
{
  Q_ASSERT(pvDeviceModbus);

  pvDeviceIosWidget->clearIoWidgets();
  pvDeviceIosWidget->setDeviceIos(pvDeviceModbus->ios());
  pvDeviceModbus->getDigitalOutputs(0);
}

void mdtModbusIoTool::setup()
{
  Q_ASSERT(pvDeviceModbus);

  // If we are connecting to node, we not accept setup (can conflict)
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

void mdtModbusIoTool::connectToNode()
{
  Q_ASSERT(pvDeviceModbus);

  mdtModbusHwNodeId hwNodeId;
  mdtModbusTcpPortManager *m = pvDeviceModbus->modbusTcpPortManager();
  Q_ASSERT(m != 0);

  if(!m->isClosed()){
    showStatusMessage(tr("Please disconnect before reconnect"), 3000);
    return;
  }
  // Update GUI state
  setStateConnectingToNode();
  // Try to find and connect to device
  hwNodeId.setId(sbHwNodeId->value(), 8, 0);
  if(!pvDeviceModbus->connectToDevice(hwNodeId, "Wago 750 I/O", 100, 502)){
    pvStatusWidget->setPermanentText(tr("Device with HW node ID ") + QString::number(sbHwNodeId->value()) + tr(" not found"));
    setStateConnectingToNodeFinished();
    return;
  }
  showStatusMessage(tr("I/O detection ..."));
  if(!pvDeviceModbus->detectIos()){
    QString details = tr("This probably caused by presence of a unsupported module.");
    details += tr("Take a look at log file for more details.");
    details += "\n" + tr("Path to log file: ");
    details += mdtErrorOut::logFile();
    showStatusMessage(tr("I/O detection failed"), details);
    pvDeviceModbus->disconnectFromDevice();
    setStateConnectingToNodeFinished();
    emit errorEvent();
    return;
  }
  pvDeviceModbus->ios()->setIosDefaultLabelShort();
  pvDeviceIosWidget->setDeviceIos(pvDeviceModbus->ios());
  pvDeviceModbus->getDigitalOutputs(0);
  showStatusMessage(tr("I/O detection done"), 1000);
  ///pbAbortScan->setEnabled(false);
  pvStatusDeviceInformations = tr("Connected to device with HW node ID ") + QString::number(sbHwNodeId->value());
  setStateConnectingToNodeFinished();
  ///pvStatusWidget->setPermanentText(tr("Connected to device with HW node ID ") + QString::number(sbHwNodeId->value()));
}

void mdtModbusIoTool::disconnectFromNode()
{
  pvStatusWidget->clearMessage();
  if(pvDeviceModbus){
    pvDeviceModbus->disconnectFromDevice();
  }
  pvDeviceIosWidget->clearIoWidgets();
}

void mdtModbusIoTool::showStatusMessage(const QString & message, int timeout)
{
  pvStatusWidget->showMessage(message, QString(), timeout);
}

void mdtModbusIoTool::showStatusMessage(const QString & message, const QString & details, int timeout)
{
  pvStatusWidget->showMessage(message, details, timeout);
}

void mdtModbusIoTool::setStateConnectingToNode()
{
  qDebug() << " o Set state connecting to node ..";
  pvConnectingToNode = true;
  ///pbDisconnect->setEnabled(false);
  actDisconnect->setEnabled(false);
  pbAbortScan->setEnabled(true);
  ///pbConnect->setEnabled(false);
  actConnect->setEnabled(false);
  sbHwNodeId->setEnabled(false);
}

void mdtModbusIoTool::setStateConnectingToNodeFinished()
{
  Q_ASSERT(pvDeviceModbus);

  qDebug() << " o Set state connecting to node finished ..";
  
  pvConnectingToNode = false;
  pvDeviceModbus->portManager()->notifyCurrentState();
}

void mdtModbusIoTool::setStatePortClosed()
{
  qDebug() << " o Set state port closed ..";
  
  if(!pvConnectingToNode){
    ///pbDisconnect->setEnabled(false);
    actDisconnect->setEnabled(false);
    pbAbortScan->setEnabled(false);
    ///pbConnect->setEnabled(true);
    actConnect->setEnabled(true);
    sbHwNodeId->setEnabled(true);
    pvStatusWidget->setPermanentText("");
  }
}

void mdtModbusIoTool::setStateDisconnected()
{
  qDebug() << " o Set state disconnected ..";
  
  if(!pvConnectingToNode){
    ///pbDisconnect->setEnabled(false);
    actDisconnect->setEnabled(false);
    pbAbortScan->setEnabled(false);
    ///pbConnect->setEnabled(true);
    actConnect->setEnabled(true);
    sbHwNodeId->setEnabled(true);
    pvStatusWidget->setPermanentText("");
  }
}

void mdtModbusIoTool::setStateConnecting()
{
  qDebug() << " o Set state connecting ..";
  
  if(!pvConnectingToNode){
    ///pbDisconnect->setEnabled(false);
    actDisconnect->setEnabled(false);
    pbAbortScan->setEnabled(true);
    ///pbConnect->setEnabled(false);
    actConnect->setEnabled(false);
    sbHwNodeId->setEnabled(false);
    pvStatusWidget->setPermanentText("");
  }
}

void mdtModbusIoTool::setStateReady()
{
  qDebug() << " o Set state ready ..";
  
  if(!pvConnectingToNode){
    ///pbDisconnect->setEnabled(true);
    actDisconnect->setEnabled(true);
    pbAbortScan->setEnabled(false);
    ///pbConnect->setEnabled(false);
    actConnect->setEnabled(false);
    sbHwNodeId->setEnabled(false);
    pvStatusWidget->setPermanentText(pvStatusDeviceInformations);
  }
}

void mdtModbusIoTool::setStateBusy()
{
  qDebug() << " o Set state busy ..";
  
  if(!pvConnectingToNode){
    ///pbDisconnect->setEnabled(true);
    actDisconnect->setEnabled(true);
    pbAbortScan->setEnabled(false);
    ///pbConnect->setEnabled(false);
    actConnect->setEnabled(false);
    sbHwNodeId->setEnabled(false);
  }
}

void mdtModbusIoTool::setStateError()
{
  qDebug() << " o Set state error ..";
  
  if(!pvConnectingToNode){
    ///pbDisconnect->setEnabled(true);
    actDisconnect->setEnabled(true);
    pbAbortScan->setEnabled(false);
    ///pbConnect->setEnabled(false);
    actConnect->setEnabled(false);
    sbHwNodeId->setEnabled(false);
  }
}
