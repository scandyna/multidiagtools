/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#include "mdtUsbtmcPortSetupDialog.h"
#include <QHBoxLayout>
#include <QAction>
#include <QList>
#include <QLabel>

#include <QDebug>

mdtPortTerm::mdtPortTerm(QWidget *parent)
 : QMainWindow(parent)
{
  setupUi(this);
  // Status bar
  lbStatusMessage = new QLabel;
  statusBar()->addPermanentWidget(lbStatusMessage);
  // Serial port members
  pvSerialPortManager = 0;
  pvSerialPortCtlWidget = 0;
  // USBTMC port members
  pvUsbtmcPortManager = 0;
  // Current port manager
  pvCurrentPortManager = 0;
  // Flags
  pvRunning = false;

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
  action_SerialPort->setChecked(true);
  attachToSerialPort();
  ///attachToUsbtmcPort();
}

mdtPortTerm::~mdtPortTerm()
{
  detachFromPorts();
}

void mdtPortTerm::setAvailableTranslations(QMap<QString, QString> &avaliableTranslations, const QString &currentTranslationKey)
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

void mdtPortTerm::appendReadenData(QByteArray data)
{
  teTerm->append(data);
}

void mdtPortTerm::appendReadenData(mdtPortTransaction transaction)
{
  teTerm->append(transaction.data());
}

void mdtPortTerm::sendCmd()
{
  QString cmd;
  bool cmdIsQuery = false;

  if(!pvRunning){
    return;
  }
  if(pvCurrentPortManager == 0){
    qDebug() << "TERM: err, pvCurrentPortManager == 0";
    return;
  }
  if(leCmd->text().size() < 1){
    return;
  }
  /// NOTE: essais
  ///cmd = teCmd->toPlainText();
  cmd = leCmd->text();
  //cmd.remove('\n');
  ///cmd.append((char)0x0D);
  if(cmd.trimmed().right(1) == "?"){
    cmdIsQuery = true;
  }
  cmd.append('\n');
  //cmd.append((char)0x04);
  /// \todo Error handling !
  // Wait until write is possible and write
  if(!pvCurrentPortManager->waitOnWriteReady(1000)){
    qDebug() << "TERM: cannot write for the moment";
    return;
  }
  if(pvCurrentPortManager->writeData(cmd.toAscii()) < 0){
    qDebug() << "TERM: write error";
    return;
  }
  // If we have a query for USBTMC port, send the read request
  if((pvCurrentPortManager == pvUsbtmcPortManager)&&(cmdIsQuery)){
    if(!pvCurrentPortManager->waitOnWriteReady(1000)){
      qDebug() << "TERM: cannot write for the moment";
      return;
    }
    if(pvUsbtmcPortManager->sendReadRequest(false) < 0){
      qDebug() << "TERM: cannot send read request";
      return;
    }
  }
  leCmd->clear();
}

void mdtPortTerm::on_pbSendCmdAbort_clicked()
{
  /**
  if(pvSerialPortManager != 0){
    pvSerialPortManager->port().flushOut();
    pvSerialPortManager->port().flushIn();
  }
  */
  qDebug() << "mdtPortTerm::on_pbSendCmdAbort_clicked() ...";
  if(pvCurrentPortManager != 0){
    qDebug() << "mdtPortTerm::on_pbSendCmdAbort_clicked() calling abort";
    pvCurrentPortManager->abort();
  }
  ///teCmd->clear();
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
  // Ctl widget
  bottomHLayout->insertWidget(0, pvSerialPortCtlWidget);
  pvSerialPortCtlWidget->makeConnections(pvSerialPortManager);

  connect(pvSerialPortManager, SIGNAL(newReadenFrame(QByteArray)), this, SLOT(appendReadenData(QByteArray)));

  // Try to open first port
  ports = pvSerialPortManager->scan();
  if(ports.size() > 0){
    pvSerialPortManager->setPortInfo(*ports.at(0));
    if(!pvSerialPortManager->openPort()){
      setStateError(tr("Cannot open port ") + ports.at(0)->portName());
      ///qDebug() << "mdtPortTerm::attachToSerialPort(): cannot open port";
      qDeleteAll(ports);
      return;
    }
  }
  ///pvSerialPortManager->setNotifyNewReadenFrame(true);
  if(pvSerialPortManager->start()){
    setStateRunning(tr("Port open: ") + pvCurrentPortManager->portInfo().displayText());
  }
  qDeleteAll(ports);
}

void mdtPortTerm::detachFromSerialPort()
{
  if(pvSerialPortManager != 0){
    disconnect(pvSerialPortManager, SIGNAL(newReadenFrame(QByteArray)), this, SLOT(appendReadenData(QByteArray)));
    delete pvSerialPortManager;
    pvSerialPortManager = 0;
    // Ctl widget
    bottomHLayout->removeWidget(pvSerialPortCtlWidget);
    delete pvSerialPortCtlWidget;
    pvSerialPortCtlWidget = 0;
    /// \todo disconnect ?

  }
  pvCurrentPortManager = 0;
  /// NOTE: à compléter
}

void mdtPortTerm::portSetup()
{
  Q_ASSERT(pvCurrentPortManager != 0);

  // Show the correct setup dialog
  if(pvSerialPortManager != 0){
    mdtSerialPortSetupDialog d(this);
    d.setPortManager(pvSerialPortManager);
    d.exec();
  }else if(pvUsbtmcPortManager != 0){
    mdtUsbtmcPortSetupDialog d(this);
    d.setPortManager(pvUsbtmcPortManager);
    d.exec();
  }
  // If port is running, enable terminal
  if(pvCurrentPortManager->isRunning()){
    ///setStateRunning(pvPortDisplayText);
    setStateRunning(tr("Port open: ") + pvCurrentPortManager->portInfo().displayText());
  }else{
    setStateStopped();
  }
}

void mdtPortTerm::attachToUsbtmcPort()
{
  QList<mdtPortInfo*> ports;

  detachFromPorts();
  // Create objects
  pvUsbtmcPortManager = new mdtUsbtmcPortManager;
  pvCurrentPortManager = pvUsbtmcPortManager;

  // Try to open first port
  ports = pvUsbtmcPortManager->scan();
  if(ports.size() < 1){
    setStateError(tr("No USBTMC device found"));
    return;
  }
  ///pvUsbtmcPortManager->setPortName(ports.at(0)->portName());
  pvUsbtmcPortManager->setPortInfo(*ports.at(0));
  if(!pvUsbtmcPortManager->openPort()){
    setStateError(tr("Cannot open port ") + ports.at(0)->portName());
    qDeleteAll(ports);
    return;
  }
  ///pvUsbtmcPortManager->setNotifyNewReadenFrame(true);
  if(pvUsbtmcPortManager->start()){
    ///pvPortDisplayText = tr("Attached to device: ") + ports.at(0)->displayText();
    ///setStateRunning(pvPortDisplayText);
    setStateRunning(tr("Port open: ") + pvCurrentPortManager->portInfo().displayText());
  }
  qDeleteAll(ports);

  connect(pvUsbtmcPortManager, SIGNAL(newReadenFrame(mdtPortTransaction)), this, SLOT(appendReadenData(mdtPortTransaction)));
}

void mdtPortTerm::detachFromUsbtmcPort()
{
  if(pvUsbtmcPortManager != 0){
    disconnect(pvUsbtmcPortManager, SIGNAL(newReadenFrame(mdtPortTransaction)), this, SLOT(appendReadenData(mdtPortTransaction)));
    delete pvUsbtmcPortManager;
    pvUsbtmcPortManager = 0;
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
  setStateStopped();
}

void mdtPortTerm::setStateRunning(const QString &msg)
{
  pvRunning = true;
  pbSendCmd->setEnabled(true);
  statusBar()->showMessage(msg);
}

void mdtPortTerm::setStateStopped(const QString &msg)
{
  pvRunning = false;
  pbSendCmd->setEnabled(false);
  statusBar()->showMessage(msg);
}

void mdtPortTerm::setStateError(const QString &msg)
{
  pvRunning = false;
  pbSendCmd->setEnabled(false);
  statusBar()->showMessage(msg);
}
