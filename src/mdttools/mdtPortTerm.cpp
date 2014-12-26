/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtPortTerm.h"
#include "mdtApplication.h"
#include "mdtPortInfo.h"
#include "mdtSerialPortManager.h"
#include "mdtUsbtmcPortManager.h"
#include "mdtUsbtmcPortSetupDialog.h"
#include "mdtPortStatusWidget.h"
#include "mdtAbstractPort.h"
#include <QHBoxLayout>
#include <QAction>
#include <QList>
#include <QLabel>
#include <QTimer>
#include <QApplication>

mdtPortTerm::mdtPortTerm(QWidget *parent)
 : QMainWindow(parent)
{
  setupUi(this);
  // Status bar
  pvStatusWidget = new mdtPortStatusWidget;
  statusBar()->addWidget(pvStatusWidget);
  // Serial port members
  pvSerialPortManager = 0;
  pvSerialPortCtlWidget = 0;
  // USBTMC port members
  pvUsbtmcPortManager = 0;
  // Current port manager
  pvCurrentPortManager = 0;
  // Flags
  pvReady = false;

  connect(pbSendCmd, SIGNAL(clicked()), this, SLOT(sendCmd()));
  connect(leCmd, SIGNAL(returnPressed()), this, SLOT(sendCmd()));
  leCmd->setFocus();

  // Actions
  connect(action_Setup, SIGNAL(triggered()), this, SLOT(portSetup()));
  pvLanguageActionGroup = 0;
  // Port selection actions
  pvPortSelectActionGroup = new QActionGroup(this);
  pvPortSelectActionGroup->addAction(action_SerialPort);
  pvPortSelectActionGroup->addAction(action_UsbTmcPort);
  connect(pvPortSelectActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(selectPortType(QAction*)));
  action_UsbTmcPort->setChecked(true);
  attachToUsbtmcPort();
}

mdtPortTerm::~mdtPortTerm()
{
  detachFromPorts();
}

void mdtPortTerm::setAvailableTranslations(const QMap<QString, QString> &avaliableTranslations, const QString &currentTranslationKey)
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

void mdtPortTerm::appendReadenData(mdtPortTransaction *transaction)
{
  Q_ASSERT(transaction != 0);

  teTerm->append(transaction->data());
}

void mdtPortTerm::sendCmd()
{
  QString cmd;

  if(!pvReady){
    return;
  }
  if(pvCurrentPortManager == 0){
    setStateError();
    showStatusMessage(tr("No port manager was set, please choose a port"));
    return;
  }
  cmd = leCmd->text().trimmed();
  if(cmd.isEmpty()){
    leCmd->clear();
    return;
  }
  if(pvCurrentPortManager == pvSerialPortManager){
    if(!sendCommandToSerialPort(cmd)){
      return;
    }
  }else if(pvCurrentPortManager == pvUsbtmcPortManager){
    if(!sendCommandToUsbtmcPort(cmd)){
      return;
    }
  }
  leCmd->clear();
}

void mdtPortTerm::on_pbSendCmdAbort_clicked()
{
  if(pvCurrentPortManager != 0){
    pvCurrentPortManager->abort();
  }
  leCmd->clear();
}

void mdtPortTerm::on_pbClearTerm_clicked()
{
  teTerm->clear();
}

void mdtPortTerm::retranslate()
{
  retranslateUi(this);
}

void mdtPortTerm::attachToSerialPort()
{
  QList<mdtPortInfo*> ports;

  detachFromPorts();
  // Create objects
  pvSerialPortManager = new mdtSerialPortManager;
  pvSerialPortCtlWidget = new mdtSerialPortCtlWidget;
  pvCurrentPortManager = pvSerialPortManager;
  // Status widget
  pvStatusWidget->enableTxRxLeds(pvSerialPortManager->writeThread(), pvSerialPortManager->readThread());
  // Control widget
  pvStatusWidget->addCustomWidget(pvSerialPortCtlWidget);
  pvSerialPortCtlWidget->makeConnections(pvSerialPortManager);
  // Setup port manager and make connections
  pvSerialPortManager->setKeepTransactionsDone(false);
  connect(pvSerialPortManager, SIGNAL(stateChanged(int)), this, SLOT(setStateFromPortManager(int)));
  connect(pvSerialPortManager, SIGNAL(stateChangedForUi(int, const QString&, int, bool)), pvStatusWidget, SLOT(setState(int, const QString&, int, bool)));
  connect(pvSerialPortManager, SIGNAL(newTransactionDone(mdtPortTransaction*)), this, SLOT(appendReadenData(mdtPortTransaction*)));
  pvSerialPortManager->notifyCurrentState();
  // Try to open first port
  ports = pvSerialPortManager->scan();
  if(ports.size() < 1){
    pvStatusWidget->setPermanentText(tr("No free serial port found"));
    return;
  }
  pvSerialPortManager->setPortInfo(*ports.at(0));
  if(pvSerialPortManager->start()){
    pvStatusWidget->setPermanentText(tr("Port : ") + pvCurrentPortManager->portInfo().displayText());
  }
  qDeleteAll(ports);
  // Command term char
  pvCmdTermSequence = "";
  leCmd->setMaxLength(200);
}

void mdtPortTerm::detachFromSerialPort()
{
  if(pvSerialPortManager != 0){
    pvSerialPortManager->stop();
    disconnect(pvSerialPortManager, SIGNAL(newTransactionDone(mdtPortTransaction*)), this, SLOT(appendReadenData(mdtPortTransaction*)));
    disconnect(pvSerialPortManager, SIGNAL(stateChanged(int)), this, SLOT(setStateFromPortManager(int)));
    disconnect(pvSerialPortManager, SIGNAL(stateChangedForUi(int, const QString&, int, bool)), pvStatusWidget, SLOT(setState(int, const QString&, int, bool)));
    // Ctl widget
    pvStatusWidget->removeCustomWidget();
    delete pvSerialPortCtlWidget;
    pvSerialPortCtlWidget = 0;
    // Status widget
    pvStatusWidget->disableTxRxLeds();
    // Free ..
    delete pvSerialPortManager;
    pvSerialPortManager = 0;
    pvStatusWidget->setPermanentText("");
  }
  pvCurrentPortManager = 0;
}

void mdtPortTerm::portSetup()
{
  Q_ASSERT(pvCurrentPortManager != 0);

  // Show the correct setup dialog
  if(pvSerialPortManager != 0){
    mdtSerialPortSetupDialog d(this);
    d.setPortManager(pvSerialPortManager);
    d.exec();
    if(pvSerialPortManager->isReady()){
      pvStatusWidget->setPermanentText(tr("Port : ") + pvCurrentPortManager->portInfo().displayText());
    }else{
      pvStatusWidget->setPermanentText("");
    }
  }else if(pvUsbtmcPortManager != 0){
    disconnect(pvUsbtmcPortManager, SIGNAL(newTransactionDone(mdtPortTransaction*)), this, SLOT(appendReadenData(mdtPortTransaction*)));
    mdtUsbtmcPortSetupDialog d(this);
    ///d.setPortManager(pvUsbtmcPortManager);
    d.exec();
    connect(pvUsbtmcPortManager, SIGNAL(newTransactionDone(mdtPortTransaction*)), this, SLOT(appendReadenData(mdtPortTransaction*)));
    if(pvUsbtmcPortManager->isReady()){
      pvStatusWidget->setPermanentText(tr("Device : ") + pvCurrentPortManager->portInfo().displayText());
    }else{
      pvStatusWidget->setPermanentText("");
    }
  }else{
    showStatusMessage(tr("Cannot call setup dialog, no port manager defined"), 3000);
    return;
  }
}

void mdtPortTerm::attachToUsbtmcPort()
{
  QList<mdtPortInfo*> ports;

  detachFromPorts();
  // Create objects
  pvUsbtmcPortManager = new mdtUsbtmcPortManager;
  pvCurrentPortManager = pvUsbtmcPortManager;
  // Setup port manager and make connections
  pvUsbtmcPortManager->setKeepTransactionsDone(false);
  connect(pvUsbtmcPortManager, SIGNAL(stateChanged(int)), this, SLOT(setStateFromPortManager(int)));
  connect(pvUsbtmcPortManager, SIGNAL(stateChangedForUi(int, const QString&, int, bool)), pvStatusWidget, SLOT(setState(int, const QString&, int, bool)));
  connect(pvUsbtmcPortManager, SIGNAL(newTransactionDone(mdtPortTransaction*)), this, SLOT(appendReadenData(mdtPortTransaction*)));
  pvUsbtmcPortManager->notifyCurrentState();
  // Try to open first port
  ports = pvUsbtmcPortManager->scan();
  if(ports.size() < 1){
    showStatusMessage(tr("No USBTMC device found"));
    pvStatusWidget->setPermanentText(tr("No USBTMC device found"));
    return;
  }
  pvUsbtmcPortManager->setPortInfo(*ports.at(0));
  if(pvUsbtmcPortManager->start()){
    pvStatusWidget->setPermanentText(tr("Device : ") + pvCurrentPortManager->portInfo().displayText());
  }
  qDeleteAll(ports);
  // Command term char
  pvCmdTermSequence = "\n";
  leCmd->setMaxLength(20);
}

void mdtPortTerm::detachFromUsbtmcPort()
{
  if(pvUsbtmcPortManager != 0){
    pvUsbtmcPortManager->stop();
    disconnect(pvUsbtmcPortManager, SIGNAL(newTransactionDone(mdtPortTransaction*)), this, SLOT(appendReadenData(mdtPortTransaction*)));
    disconnect(pvUsbtmcPortManager, SIGNAL(stateChanged(int)), this, SLOT(setStateFromPortManager(int)));
    disconnect(pvUsbtmcPortManager, SIGNAL(stateChangedForUi(int, const QString&, int, bool)), pvStatusWidget, SLOT(setState(int, const QString&, int, bool)));
    delete pvUsbtmcPortManager;
    pvUsbtmcPortManager = 0;
    pvStatusWidget->setPermanentText("");
  }
  pvCurrentPortManager = 0;
}

void mdtPortTerm::selectPortType(QAction*)
{
  if(action_SerialPort->isChecked()){
    if(pvSerialPortManager == 0){
      attachToSerialPort();
    }
  }else if(action_UsbTmcPort->isChecked()){
    if(pvUsbtmcPortManager == 0){
      attachToUsbtmcPort();
    }
  }
}

void mdtPortTerm::detachFromPorts()
{
  detachFromSerialPort();
  detachFromUsbtmcPort();
}

void mdtPortTerm::setStateFromPortManager(int state)
{
  switch((mdtPortManager::state_t)state){
    case mdtPortManager::PortClosed:
      setStatePortClosed();
      break;
    case mdtPortManager::Starting:
    case mdtPortManager::Running:
    case mdtPortManager::Stopped:
    case mdtPortManager::Stopping:
    case mdtPortManager::Connected:
      break;
    case mdtPortManager::Disconnected:
      setStateDisconnected();
      break;
    case mdtPortManager::Connecting:
      setStateConnecting();
      break;
    case mdtPortManager::PortReady:
      if(pvCurrentPortManager == pvSerialPortManager){
        setStateReady();
      }
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
}

void mdtPortTerm::setStatePortClosed()
{
  pvReady = false;
  pbSendCmd->setEnabled(false);
  pbSendCmdAbort->setEnabled(false);
}

void mdtPortTerm::setStateDisconnected()
{
  pvReady = false;
  pbSendCmd->setEnabled(false);
  pbSendCmdAbort->setEnabled(false);
}

void mdtPortTerm::setStateConnecting()
{
  pvReady = false;
  pbSendCmd->setEnabled(false);
  pbSendCmdAbort->setEnabled(false);
}

void mdtPortTerm::setStateReady()
{
  pvReady = true;
  pbSendCmd->setEnabled(true);
  pbSendCmdAbort->setEnabled(true);
}

void mdtPortTerm::setStateBusy()
{
  pvReady = false;
  pbSendCmd->setEnabled(false);
  pbSendCmdAbort->setEnabled(false);
}

void mdtPortTerm::setStateError()
{
  pvReady = false;
  pbSendCmd->setEnabled(false);
  pbSendCmdAbort->setEnabled(false);
}

void mdtPortTerm::showStatusMessage(const QString &message, int timeout)
{
  Q_ASSERT(pvStatusWidget != 0);

  pvStatusWidget->showMessage(message, "", timeout);
}

bool mdtPortTerm::sendCommandToSerialPort(const QString &command)
{
  Q_ASSERT(pvSerialPortManager != 0);

  if(pvSerialPortManager->sendData(command.toAscii() + pvCmdTermSequence) < 0){
    showStatusMessage(tr("Error occured during write process"), 1000);
    return false;
  }

  return true;
}

bool mdtPortTerm::sendCommandToUsbtmcPort(const QString &command)
{
  Q_ASSERT(pvUsbtmcPortManager != 0);

  // Send command
  if(pvUsbtmcPortManager->sendCommand(command.toAscii() + pvCmdTermSequence) < 0){
    showStatusMessage(tr("Error occured during write process"), 1000);
    return false;
  }
  // If we have a query, send the read request
  if(command.right(1) == "?"){
    if(pvUsbtmcPortManager->sendReadRequest(false) < 0){
      showStatusMessage(tr("Error occured while sending read request"), 1000);
      return false;
    }
  }

  return true;
}
