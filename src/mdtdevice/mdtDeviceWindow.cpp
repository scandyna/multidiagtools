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
#include "mdtDeviceWindow.h"
#include "mdtDeviceIosWidget.h"
#include "mdtApplication.h"
#include <QPushButton>
#include <QActionGroup>

mdtDeviceWindow::mdtDeviceWindow(QWidget *parent)
 : QMainWindow(parent)
{
  setupUi(this);

  pvStatusWidget = new mdtDeviceStatusWidget;
  statusBar()->addWidget(pvStatusWidget);
  pvDevice = 0;
}

mdtDeviceWindow::~mdtDeviceWindow()
{
}

void mdtDeviceWindow::setDevice(mdtDevice *device)
{
  Q_ASSERT(device != 0);

  pvDevice = device;
  // Status widget
  pvStatusWidget->setDevice(pvDevice);
}

void mdtDeviceWindow::setIosWidget(QWidget *widget)
{
  // Add I/Os widget to the scroll area
  saIos->setWidget(widget);
}

mdtDeviceStatusWidget *mdtDeviceWindow::statusWidget()
{
  Q_ASSERT(pvStatusWidget != 0);

  return pvStatusWidget;
}

void mdtDeviceWindow::enableTranslations()
{
  Q_ASSERT(mdtApp != 0);

  connect(mdtApp, SIGNAL(languageChanged()), this, SLOT(retranslate()));
  setAvailableTranslations(mdtApp->availableTranslations(), mdtApp->currentTranslationKey());
}

void mdtDeviceWindow::setAvailableTranslations(QMap<QString, QString> &avaliableTranslations, const QString &currentTranslationKey)
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

void mdtDeviceWindow::retranslate()
{
  retranslateUi(this);
}
