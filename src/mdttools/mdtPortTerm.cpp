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
#include <QHBoxLayout>
#include <QAction>
#include <QByteArray>
#include <QList>

#include <QDebug>

mdtPortTerm::mdtPortTerm(QWidget *parent)
 : QMainWindow(parent)
{
  setupUi(this);
  // Serial port members
  pvSerialPortManager = 0;
  pvSerialPortCtlWidget = 0;
  // USBTMC port members
  pvUsbtmcPortManager = 0;
  // Current port manager
  pvCurrentPortManager = 0;
  
  connect(pbSendCmd, SIGNAL(clicked()), this, SLOT(sendCmd()));
  connect(leCmd, SIGNAL(returnPressed()), this, SLOT(sendCmd()));
  leCmd->setFocus();
  
  
  // Actions
  connect(action_Setup, SIGNAL(triggered()), this, SLOT(serialPortSetup()));
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

void mdtPortTerm::appendReadenData()
{
  if(pvCurrentPortManager == 0){
    qDebug() << "TERM: err, pvCurrentPortManager == 0";
    return;
  }
  while(!pvCurrentPortManager->readenFrames().isEmpty()){
    teTerm->append(pvCurrentPortManager->readenFrames().takeFirst());
  }
}

void mdtPortTerm::sendCmd()
{
  QString cmd;
  bool cmdIsQuery = false;

  if(pvCurrentPortManager == 0){
    qDebug() << "TERM: err, pvCurrentPortManager == 0";
    return;
  }
  ///if(teCmd->toPlainText().size() < 1){
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
  pvCurrentPortManager->writeData(cmd.toAscii());
  if(pvCurrentPortManager == pvUsbtmcPortManager){
    if(cmdIsQuery){
      pvUsbtmcPortManager->sendReadRequest();
    }
  }
  ///teCmd->clear();
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
  qDebug() << "Port Term: bottomHLayout items: " << bottomHLayout->count();

  connect(pvSerialPortManager, SIGNAL(newReadenFrame()), this, SLOT(appendReadenData()));

  // Try to open first port
  ports = pvSerialPortManager->scan();
  if(ports.size() > 0){
    pvSerialPortManager->setPortName(ports.at(0)->portName());
    if(!pvSerialPortManager->openPort()){
      qDebug() << "mdtPortTerm::attachToSerialPort(): cannot open port";
      qDeleteAll(ports);
      return;
    }
  }
  if(!pvSerialPortManager->start()){
    qDebug() << "mdtPortTerm::attachToSerialPort(): cannot start port";
  }
  qDeleteAll(ports);
}

void mdtPortTerm::detachFromSerialPort()
{
  if(pvSerialPortManager != 0){
    disconnect(pvSerialPortManager, SIGNAL(newReadenFrame()), this, SLOT(appendReadenData()));
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

void mdtPortTerm::serialPortSetup()
{
  // Show the correct setup dialog
  if(pvSerialPortManager != 0){
    mdtSerialPortSetupDialog d(this);
    d.setPortManager(pvSerialPortManager);
    d.exec();
  }
}

void mdtPortTerm::attachToUsbtmcPort()
{
  detachFromPorts();
  // Create objects
  pvUsbtmcPortManager = new mdtUsbtmcPortManager;
  /// \todo Provisoire
  pvUsbtmcPortManager->setPortName(":0x0957:0x0588");
  if(!pvUsbtmcPortManager->openPort()){
    qDebug() << "Port term: cannot open USBTMC port";
    detachFromUsbtmcPort();
    return;
  }
  if(!pvUsbtmcPortManager->start()){
    qDebug() << "Port term: cannot start USBTMC port";
    detachFromUsbtmcPort();
    return;
  }

  connect(pvUsbtmcPortManager, SIGNAL(newReadenFrame()), this, SLOT(appendReadenData()));
  pvCurrentPortManager = pvUsbtmcPortManager;
  
}

void mdtPortTerm::detachFromUsbtmcPort()
{
  if(pvUsbtmcPortManager != 0){
    disconnect(pvUsbtmcPortManager, SIGNAL(newReadenFrame()), this, SLOT(appendReadenData()));
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
}
