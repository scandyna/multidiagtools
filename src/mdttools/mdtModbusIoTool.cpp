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
#include "mdtModbusIoTool.h"
#include "mdtDeviceModbusWago.h"
#include "mdtDeviceIosWidget.h"
#include "mdtApplication.h"
#include "mdtDeviceInfo.h"
#include "mdtPortStatusWidget.h"
#include "mdtModbusTcpPortManager.h"
#include "mdtModbusTcpPortSetupDialog.h"
#include "mdtPortInfo.h"
#include <QList>

mdtModbusIoTool::mdtModbusIoTool(QWidget *parent, Qt::WindowFlags flags)
 : QMainWindow(parent, flags)
{
  // Setup GUI
  setupUi(this);
  // Setup status bar
  pvStatusWidget = new mdtPortStatusWidget;
  statusBar()->addWidget(pvStatusWidget);

  // Setup I/Os widget
  pvDeviceIos = new mdtDeviceIos(this);
  pvDeviceIosWidget = new mdtDeviceIosWidget;
  saIos->setWidget(pvDeviceIosWidget);
  // Setup device
  pvDeviceModbusWago = new mdtDeviceModbusWago(this);
  pvDeviceModbusWago->setName("Wago I/O 750");
  connect(pvDeviceModbusWago, SIGNAL(stateChanged(int)), this, SLOT(setStateFromPortManager(int)));
  connect(pvDeviceModbusWago, SIGNAL(statusMessageChanged(const QString&, const QString&, int)), this, SLOT(showStatusMessage(const QString&, const QString&, int)));

  // Set some flags
  pvReady = false;

  /**
  pvDeviceModbusWago->connectToDevice(mdtDeviceInfo());
  pvDeviceModbusWago->detectIos(pvDeviceIos);
  pvDeviceModbusWago->setIos(pvDeviceIos, true);
  pvDeviceIosWidget->setDeviceIos(pvDeviceIos);
  */

    // Actions
  connect(action_Setup, SIGNAL(triggered()), this, SLOT(setup()));
  pvLanguageActionGroup = 0;

  connect(pbConnect, SIGNAL(clicked()), this, SLOT(connectToNode()));
  connect(pbDisconnect, SIGNAL(clicked()), this, SLOT(disconnectFromNode()));
  connect(pbAbortScan, SIGNAL(clicked()), pvDeviceModbusWago->modbusTcpPortManager(), SLOT(abortScan()));
  setWindowTitle(tr("MODBUS I/O tool for Wago 750"));
  setStateFromPortManager(pvDeviceModbusWago->modbusTcpPortManager()->currentState());
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

void mdtModbusIoTool::setStateFromPortManager(int state)
{
  pvStatusWidget->setState(state);
  switch(state){
    case mdtDevice::Disconnected:
      setStateDisconnected();
      break;
    case mdtDevice::Connecting:
      setStateConnecting();
      break;
    case mdtDevice::Ready:
      setStateReady();
      break;
    case mdtDevice::Busy:
      setStateBusy();
      break;
    case mdtDevice::Warning:
      setStateWarning();
      break;
    case mdtDevice::Error:
      setStateError();
      break;
    default:
      setStateError();
      showStatusMessage(tr("Received a unknown state"));
  }
}

void mdtModbusIoTool::setup()
{
  mdtModbusTcpPortSetupDialog d(this);
  d.setPortManager(pvDeviceModbusWago->portManager());
  d.exec();
  // If portManager is running, we can try to detect I/Os
  if(pvDeviceModbusWago->portManager()->isRunning()){
    if(!pvDeviceModbusWago->isWago750()){
      showStatusMessage(tr("Device is not a Wago 750 Fieldbus"), tr("I/O detetcion currently only works with Wago 750 fielbus coupler"));
      pvDeviceModbusWago->portManager()->closePort();
      return;
    }
    showStatusMessage(tr("I/O detection ..."));
    if(!pvDeviceModbusWago->detectIos(pvDeviceIos)){
      showStatusMessage(tr("I/O detection failed"));
      pvDeviceModbusWago->portManager()->closePort();
      return;
    }
    pvDeviceModbusWago->setIos(pvDeviceIos, true);
    pvDeviceIosWidget->setDeviceIos(pvDeviceIos);
    showStatusMessage(tr("I/O detection done"), 1000);
  }
}

void mdtModbusIoTool::connectToNode()
{
  QList<mdtPortInfo*> portInfoList;
  mdtModbusTcpPortManager *m = pvDeviceModbusWago->modbusTcpPortManager();
  Q_ASSERT(m != 0);

  if(m->isRunning()){
    showStatusMessage(tr("Please disconnect before reconnect"), 3000);
    return;
  }
  pbAbortScan->setEnabled(true);
  pbConnect->setEnabled(false);
  // Scan looking in chache file first
  portInfoList = m->scan(m->readScanResult());
  // Try to connect ...
  if(pvDeviceModbusWago->connectToDevice(portInfoList, sbHwNodeId->value(), 8) != mdtAbstractPort::NoError){
    // scan network an try again
    pbConnect->setEnabled(false);
    qDeleteAll(portInfoList);
    portInfoList.clear();
    portInfoList = m->scan(QNetworkInterface::allInterfaces(), 502, 100);
    if(pvDeviceModbusWago->connectToDevice(portInfoList, sbHwNodeId->value(), 8) != mdtAbstractPort::NoError){
      showStatusMessage(tr("Device with HW node ID ") + QString::number(sbHwNodeId->value()) + tr(" not found"));
      pbAbortScan->setEnabled(false);
      pbConnect->setEnabled(true);
      return;
    }
    // Ok found, save scan result
    if(!m->saveScanResult(portInfoList)){
      showStatusMessage(tr("Cannot save scan result in cache file"), 3000);
    }
    // We no lobger need portInfoList
    qDeleteAll(portInfoList);
    portInfoList.clear();
  }
  showStatusMessage(tr("I/O detection ..."));
  if(!pvDeviceModbusWago->detectIos(pvDeviceIos)){
    showStatusMessage(tr("I/O detection failed"));
    pvDeviceModbusWago->portManager()->closePort();
    pbAbortScan->setEnabled(false);
    ///pbConnect->setEnabled(true);
    return;
  }
  pvDeviceModbusWago->setIos(pvDeviceIos, true);
  pvDeviceIosWidget->setDeviceIos(pvDeviceIos);
  showStatusMessage(tr("I/O detection done"), 1000);
  pbAbortScan->setEnabled(false);
  ///pbConnect->setEnabled(true);
}

void mdtModbusIoTool::disconnectFromNode()
{
  mdtModbusTcpPortManager *m = pvDeviceModbusWago->modbusTcpPortManager();
  Q_ASSERT(m != 0);

  m->closePort();
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

void mdtModbusIoTool::setStateDisconnected()
{
  pbDisconnect->setEnabled(false);
  pbConnect->setEnabled(true);
  sbHwNodeId->setEnabled(true);
}

void mdtModbusIoTool::setStateConnecting()
{
  pbDisconnect->setEnabled(false);
  pbConnect->setEnabled(false);
  sbHwNodeId->setEnabled(false);
}

void mdtModbusIoTool::setStateReady()
{
  pbDisconnect->setEnabled(true);
  pbConnect->setEnabled(false);
  sbHwNodeId->setEnabled(false);
}

void mdtModbusIoTool::setStateBusy()
{
  pbDisconnect->setEnabled(true);
  pbConnect->setEnabled(false);
  sbHwNodeId->setEnabled(false);
}

void mdtModbusIoTool::setStateWarning()
{
  pbDisconnect->setEnabled(true);
  pbConnect->setEnabled(false);
  sbHwNodeId->setEnabled(false);
}

void mdtModbusIoTool::setStateError()
{
  pbDisconnect->setEnabled(true);
  pbConnect->setEnabled(false);
  sbHwNodeId->setEnabled(false);
}
