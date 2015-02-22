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
  pvDeviceModbusWago = new mdtDeviceModbusWago(this);
  ///pvDeviceModbusWago->setName("Wago I/O 750");
  // Setup I/Os widget
  pvDeviceIosWidget = new mdtDeviceIosWidget;
  saIos->setWidget(pvDeviceIosWidget);
  // Make connections
  connect(pvDeviceModbusWago, SIGNAL(stateChanged(int)), this, SLOT(setState(int)));
  connect(pvDeviceModbusWago, SIGNAL(statusMessageChanged(const QString&, const QString&, int)), this, SLOT(showStatusMessage(const QString&, const QString&, int)));
  Q_ASSERT(pvDeviceModbusWago->portManager() != 0);
  connect(pvDeviceModbusWago->portManager(), SIGNAL(stateChangedForUi(int, const QString&, int, bool)), pvStatusWidget, SLOT(setState(int, const QString&, int, bool)));

  /// \note Test
  connect(this, SIGNAL(errorEvent()), pvDeviceModbusWago->portManager(), SIGNAL(pmUnhandledErrorEvent()));
  
  // Set some flags
  pvReady = false;
  pvConnectingToNode = false;
    // Actions
  connect(action_Setup, SIGNAL(triggered()), this, SLOT(setup()));
  pvLanguageActionGroup = 0;
  // Buttons
  connect(pbConnect, SIGNAL(clicked()), this, SLOT(connectToNode()));
  connect(pbDisconnect, SIGNAL(clicked()), this, SLOT(disconnectFromNode()));
  connect(pbAbortScan, SIGNAL(clicked()), pvDeviceModbusWago->modbusTcpPortManager(), SLOT(abortScan()));
  setWindowTitle(tr("MODBUS I/O tool for Wago 750"));
  pvDeviceModbusWago->portManager()->notifyCurrentState();
  // Start periodic inputs query (will only start once device is ready, see mdtDevice doc for details)
  pvDeviceModbusWago->start(100);
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

void mdtModbusIoTool::setState(int state)
{
  ///qDebug() << "mdtModbusIoTool - new state: " << pvDeviceModbusWago->currentState() << " ...";
  switch((mdtPortManager::state_t)state){
    case mdtPortManager::PortClosed:
      setStatePortClosed();
      break;
    case mdtPortManager::Disconnected:
      setStateDisconnected();
      break;
    case mdtPortManager::Connecting:
      setStateConnecting();
      break;
    case mdtPortManager::Ready:
      setStateReady();
      break;
    case mdtPortManager::Busy:
      setStateBusy();
      break;
    case mdtPortManager::PortError:
      setStateError();
      break;
  }
  ///qDebug() << "mdtModbusIoTool - new state: " << pvDeviceModbusWago->currentState();
}

void mdtModbusIoTool::setup()
{
  // If we are connecting to node, we not accept setup (can conflict)
  if(pvConnectingToNode){
    QMessageBox msgBox;
    msgBox.setText(tr("Cannot run setup while connecting to node, please abort and retry"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();
    return;
  }
  mdtModbusTcpPortSetupDialog d(this);
  d.setPortManager(pvDeviceModbusWago->portManager());
  d.exec();
  // If portManager is running, we can try to detect I/Os
  if(pvDeviceModbusWago->portManager()->isReady()){
    if(!pvDeviceModbusWago->isWago750()){
      showStatusMessage(tr("Device is not a Wago 750 Fieldbus"), tr("I/O detetcion currently only works with Wago 750 fielbus coupler"));
      pvDeviceModbusWago->portManager()->stop();
      return;
    }
    showStatusMessage(tr("I/O detection ..."));
    if(!pvDeviceModbusWago->detectIos()){
      showStatusMessage(tr("I/O detection failed"));
      pvDeviceModbusWago->portManager()->stop();
      return;
    }
    pvDeviceIosWidget->setDeviceIos(pvDeviceModbusWago->ios());
    pvDeviceModbusWago->getDigitalOutputs(0);
    showStatusMessage(tr("I/O detection done"), 1000);
  }
}

void mdtModbusIoTool::connectToNode()
{
  QList<mdtPortInfo*> portInfoList;
  QList<int> expectedHwNodeAddresses;
  mdtModbusTcpPortManager *m = pvDeviceModbusWago->modbusTcpPortManager();
  Q_ASSERT(m != 0);

  if(!m->isClosed()){
    showStatusMessage(tr("Please disconnect before reconnect"), 3000);
    return;
  }
  ///expectedHwNodeAddresses.append(sbHwNodeId->value());
  // Update GUI state
  setStateConnectingToNode();
  
  // Try to find and connect to device
  ///pvDeviceModbusWago->setHardwareNodeId(sbHwNodeId->value(), 8, 0);
  if(pvDeviceModbusWago->connectToDevice(expectedHwNodeAddresses) != mdtAbstractPort::NoError){
    pvStatusWidget->setPermanentText(tr("Device with HW node ID ") + QString::number(sbHwNodeId->value()) + tr(" not found"));
    setStateConnectingToNodeFinished();
    return;
  }
  showStatusMessage(tr("I/O detection ..."));
  if(!pvDeviceModbusWago->detectIos()){
    QString details = tr("This probably caused by presence of a unsupported module.");
    details += tr("Take a look at log file for more details.");
    details += "\n" + tr("Path to log file: ");
    details += mdtErrorOut::logFile();
    showStatusMessage(tr("I/O detection failed"), details);
    ///pvDeviceModbusWago->portManager()->stop();
    setStateConnectingToNodeFinished();
    emit errorEvent();
    return;
  }
  pvDeviceIosWidget->setDeviceIos(pvDeviceModbusWago->ios());
  pvDeviceModbusWago->getDigitalOutputs(0);
  showStatusMessage(tr("I/O detection done"), 1000);
  ///pbAbortScan->setEnabled(false);
  pvStatusDeviceInformations = tr("Connected to device with HW node ID ") + QString::number(sbHwNodeId->value());
  setStateConnectingToNodeFinished();
  ///pvStatusWidget->setPermanentText(tr("Connected to device with HW node ID ") + QString::number(sbHwNodeId->value()));
}

void mdtModbusIoTool::disconnectFromNode()
{
  pvStatusWidget->clearMessage();
  pvDeviceModbusWago->disconnectFromDevice();
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
  pvConnectingToNode = true;
  pbDisconnect->setEnabled(false);
  pbAbortScan->setEnabled(true);
  pbConnect->setEnabled(false);
  sbHwNodeId->setEnabled(false);
}

void mdtModbusIoTool::setStateConnectingToNodeFinished()
{
  pvConnectingToNode = false;
  pvDeviceModbusWago->portManager()->notifyCurrentState();
}

void mdtModbusIoTool::setStatePortClosed()
{
  if(!pvConnectingToNode){
    pbDisconnect->setEnabled(false);
    pbAbortScan->setEnabled(false);
    pbConnect->setEnabled(true);
    sbHwNodeId->setEnabled(true);
    pvStatusWidget->setPermanentText("");
  }
}

void mdtModbusIoTool::setStateDisconnected()
{
  if(!pvConnectingToNode){
    pbDisconnect->setEnabled(false);
    pbAbortScan->setEnabled(false);
    pbConnect->setEnabled(true);
    sbHwNodeId->setEnabled(true);
    pvStatusWidget->setPermanentText("");
  }
}

void mdtModbusIoTool::setStateConnecting()
{
  if(!pvConnectingToNode){
    pbDisconnect->setEnabled(false);
    pbAbortScan->setEnabled(true);
    pbConnect->setEnabled(false);
    sbHwNodeId->setEnabled(false);
    pvStatusWidget->setPermanentText("");
  }
}

void mdtModbusIoTool::setStateReady()
{
  if(!pvConnectingToNode){
    pbDisconnect->setEnabled(true);
    pbAbortScan->setEnabled(false);
    pbConnect->setEnabled(false);
    sbHwNodeId->setEnabled(false);
    pvStatusWidget->setPermanentText(pvStatusDeviceInformations);
  }
}

void mdtModbusIoTool::setStateBusy()
{
  if(!pvConnectingToNode){
    pbDisconnect->setEnabled(true);
    pbAbortScan->setEnabled(false);
    pbConnect->setEnabled(false);
    sbHwNodeId->setEnabled(false);
  }
}

void mdtModbusIoTool::setStateError()
{
  if(!pvConnectingToNode){
    pbDisconnect->setEnabled(true);
    pbAbortScan->setEnabled(false);
    pbConnect->setEnabled(false);
    sbHwNodeId->setEnabled(false);
  }
}
