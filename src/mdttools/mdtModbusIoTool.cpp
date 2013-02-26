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

  // Set some flags
  pvReady = false;

  pvDeviceModbusWago->connectToDevice(mdtDeviceInfo());
  pvDeviceModbusWago->detectIos(pvDeviceIos);
  pvDeviceModbusWago->setIos(pvDeviceIos, true);
  pvDeviceIosWidget->setDeviceIos(pvDeviceIos);
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

void mdtModbusIoTool::setStateDisconnected()
{

}

void mdtModbusIoTool::setStateConnecting()
{

}

void mdtModbusIoTool::setStateReady()
{

}

void mdtModbusIoTool::setStateBusy()
{

}

void mdtModbusIoTool::setStateWarning()
{

}

void mdtModbusIoTool::setStateError()
{

}

void mdtModbusIoTool::showStatusMessage(const QString & message, int timeout)
{
  pvStatusWidget->showMessage(message, QString(), timeout);
}

void mdtModbusIoTool::showStatusMessage(const QString & message, const QString & details, int timeout)
{
  pvStatusWidget->showMessage(message, details, timeout);
}
