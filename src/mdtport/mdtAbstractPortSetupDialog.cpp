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
#include "mdtAbstractPortSetupDialog.h"
#include "mdtPortStatusWidget.h"
#include "mdtError.h"
#include <QVBoxLayout>
#include <QPushButton>

//#include <QDebug>

mdtAbstractPortSetupDialog::mdtAbstractPortSetupDialog(QWidget *parent)
 : QDialog(parent)
{
  setupUi(this);
  pvPortManager = 0;
  wHeader = 0;
  // Setup status widget
  pvStatusWidget = new mdtPortStatusWidget;
  bottomLayout->insertWidget(0, pvStatusWidget);
  // Remove options tabs
  while(twOptions->count() > 0){
    twOptions->removeTab(0);
  }
}

mdtAbstractPortSetupDialog::~mdtAbstractPortSetupDialog()
{
}

void mdtAbstractPortSetupDialog::setPortManager(mdtPortManager *portManager)
{
  Q_ASSERT(portManager != 0);

  pvPortManager = portManager;
  connect(pvPortManager, SIGNAL(stateChanged(int)), this, SLOT(setStateFromPortManager(int)));
  setStateFromPortManager((int)pvPortManager->currentState());
  portManagerSet();
}

mdtPortManager *mdtAbstractPortSetupDialog::portManager()
{
  Q_ASSERT(pvPortManager != 0);

  return pvPortManager;
}

void mdtAbstractPortSetupDialog::portManagerSet()
{
}

void mdtAbstractPortSetupDialog::setHeaderWidget(QWidget *w)
{
  Q_ASSERT(layout() != 0);

  QVBoxLayout *l = dynamic_cast<QVBoxLayout*>(layout());
  Q_ASSERT(l != 0);

  if(wHeader != 0){
    l->removeWidget(wHeader);
    delete wHeader;
  }
  wHeader = w;
  l->insertWidget(0, wHeader);
}

void mdtAbstractPortSetupDialog::addOptionsTabWidget(QWidget *w, const QString &label)
{
  twOptions->addTab(w, label);
}

void mdtAbstractPortSetupDialog::showStatusMessage(const QString &message, int timeout)
{
  pvStatusWidget->showMessage(message, QString(), timeout);
}

void mdtAbstractPortSetupDialog::showStatusMessage(const QString &message, const QString &details, int timeout)
{
  pvStatusWidget->showMessage(message, details, timeout);
}

void mdtAbstractPortSetupDialog::setOkButtonEnabled(bool enabled)
{
  QPushButton *button = buttonBox->button(QDialogButtonBox::Ok);
  if(button == 0){
    mdtError e(MDT_GUI_ERROR, "Ok button not found in buttonBox", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtAbstractPortSetupDialog");
    e.commit();
    return;
  }
  button->setEnabled(enabled);
}

void mdtAbstractPortSetupDialog::setApplyButtonEnabled(bool enabled)
{
  QPushButton *button = buttonBox->button(QDialogButtonBox::Apply);
  if(button == 0){
    mdtError e(MDT_GUI_ERROR, "Apply button not found in buttonBox", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtAbstractPortSetupDialog");
    e.commit();
    return;
  }
  button->setEnabled(enabled);
}

void mdtAbstractPortSetupDialog::setCancelButtonEnabled(bool enabled)
{
  QPushButton *button = buttonBox->button(QDialogButtonBox::Cancel);
  if(button == 0){
    mdtError e(MDT_GUI_ERROR, "Cancel button not found in buttonBox", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtAbstractPortSetupDialog");
    e.commit();
    return;
  }
  button->setEnabled(enabled);
}

void mdtAbstractPortSetupDialog::setStateFromPortManager(int state)
{
  pvStatusWidget->setState(state);
  switch(state){
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
    case mdtPortManager::Warning:
      setStateWarning();
      break;
    case mdtPortManager::Error:
      setStateError();
      break;
    default:
      setStateError();
      showStatusMessage(tr("Received a unknown state"));
  }
}

void mdtAbstractPortSetupDialog::on_buttonBox_clicked(QAbstractButton *button)
{
  Q_ASSERT(button != 0);

  QDialogButtonBox::StandardButton type;

  type = buttonBox->standardButton(button);
  switch(type){
    case QDialogButtonBox::Ok:
      if(applySetup()){
        close();
      }
      break;
    case QDialogButtonBox::Apply:
      applySetup();
      break;
    case QDialogButtonBox::Cancel:
      close();
      break;
    default:
      mdtError e(MDT_GUI_ERROR, "Recived signal from buttonBox with unknown button type", mdtError::Warning);
      MDT_ERROR_SET_SRC(e, "mdtAbstractPortSetupDialog");
      e.commit();
  }
}

