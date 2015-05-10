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
#include "mdtTtTestStepWidget.h"

#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QIcon>
#include <QFont>

//#include <QDebug>

mdtTtTestStepWidget::mdtTtTestStepWidget(QWidget* parent)
 : QWidget(parent),
   pvLayout(new QGridLayout),
   pvCustomWidget(nullptr)
{
  // Setup run/abort button
  pbRunAbort = new QPushButton;
//   connect(pbRunAbort, SIGNAL(clicked()), this, SLOT(runAbort()));
  connect(pbRunAbort, SIGNAL(clicked()), this, SIGNAL(runAbortButtonClicked()));
  pvLayout->addWidget(pbRunAbort, 0, 0);
  pvRunAbortButtonEnabled = true;
  pvAbortSupported = false;
  // Setup title label
  lbTitle = new QLabel;
  lbTitle->setTextFormat(Qt::PlainText);
  pvLayout->addWidget(lbTitle, 0, 1);
  // Setup state label
  lbState = new QLabel;
  lbState->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  QFont f("Sans Serif", 12);
  f.setBold(true);
  lbState->setFont(f);
  lbState->setTextFormat(Qt::PlainText);
  pvLayout->addWidget(lbState, 0, 3);
  // Setup message label
  lbMessage = new QLabel;
  lbMessage->setTextFormat(Qt::PlainText);
  pvLayout->addWidget(lbMessage, 0, 4);
  // Other setup
  pvLayout->setColumnStretch(5, 2);
  setLayout(pvLayout);
  setStateInitial();
}

void mdtTtTestStepWidget::setTitle(const QString & text)
{
  lbTitle->setText(text);
}

void mdtTtTestStepWidget::setMessage(const QString & msg)
{
  lbMessage->setText(msg);
}

void mdtTtTestStepWidget::clearMessage()
{
  lbMessage->clear();
}

void mdtTtTestStepWidget::setCustomWidget(QWidget* w)
{
  if(pvCustomWidget != nullptr){
    pvLayout->removeWidget(pvCustomWidget);
    delete pvCustomWidget;
  }
  pvCustomWidget = w;
  pvLayout->addWidget(pvCustomWidget, 0, 2);
}

void mdtTtTestStepWidget::setAbortSupported(bool support)
{
  pvAbortSupported = support;
  updateRunAbortButtonEnabledState();
}

void mdtTtTestStepWidget::setRunAbortButtonEnabled(bool enable)
{
  pvRunAbortButtonEnabled = enable;
  updateRunAbortButtonEnabledState();
}

void mdtTtTestStepWidget::setState(mdtTtTestStep::State_t state)
{
  pvState = state;
  switch(state){
    case mdtTtTestStep::State_t::Initial:
      setStateInitial();
      break;
    case mdtTtTestStep::State_t::Running:
      setStateRunning();
      break;
    case mdtTtTestStep::State_t::Success:
      setStateSuccess();
      break;
    case mdtTtTestStep::State_t::Warn:
      setStateWarn();
      break;
    case mdtTtTestStep::State_t::Fail:
      setStateFail();
      break;
  };
}

void mdtTtTestStepWidget::setStateInitial()
{
  // Update run/abort button
  pbRunAbort->setText(tr("Run"));
  pbRunAbort->setIcon(QIcon::fromTheme("media-playback-start"));
  updateRunAbortButtonEnabledState();
  // Update state label
  lbState->setStyleSheet("");
  lbState->setText("");
  // Signal new state
  
  emit finished(this);
}

void mdtTtTestStepWidget::setStateRunning()
{
  // Update run/abort button
  if(pvAbortSupported){
    pbRunAbort->setText(tr("Abort"));
    pbRunAbort->setIcon(QIcon::fromTheme("media-playback-stop"));
  }else{
    pbRunAbort->setText(tr("Run"));
    pbRunAbort->setIcon(QIcon::fromTheme("media-playback-start"));
  }
  updateRunAbortButtonEnabledState();
  // Update state label
  lbState->setStyleSheet("");
  lbState->setText("Running ...");
  // Signal new state
  emit started(this);
}

void mdtTtTestStepWidget::setStateFail()
{
  // Update run/abort button
  pbRunAbort->setText(tr("Run"));
  pbRunAbort->setIcon(QIcon::fromTheme("media-playback-start"));
  updateRunAbortButtonEnabledState();
  // Update state label
  lbState->setStyleSheet("background-color: rgb(255,0,0);\ncolor: rgb(255,255,255);");
  lbState->setText("FAIL");
  // Signal new state
  emit finished(this);
}

void mdtTtTestStepWidget::setStateWarn()
{
  // Update run/abort button
  pbRunAbort->setText(tr("Run"));
  pbRunAbort->setIcon(QIcon::fromTheme("media-playback-start"));
  updateRunAbortButtonEnabledState();
  // Update state label
  lbState->setStyleSheet("background-color: rgb(255,170,0);\ncolor: rgb(0,0,0);");
  lbState->setText("Warn");
  // Signal new state
  emit finished(this);
}

void mdtTtTestStepWidget::setStateSuccess()
{
  // Update run/abort button
  pbRunAbort->setText(tr("Run"));
  pbRunAbort->setIcon(QIcon::fromTheme("media-playback-start"));
  updateRunAbortButtonEnabledState();
  // Update state label
  lbState->setStyleSheet("background-color: rgb(85,255,0);\ncolor: rgb(0,0,0);");
  lbState->setText("Ok");
  // Signal new state
  emit finished(this);
}

void mdtTtTestStepWidget::updateRunAbortButtonEnabledState()
{
  // If pvRunAbortButtonEnabled is false, button is allways disabled
  if(!pvRunAbortButtonEnabled){
    pbRunAbort->setEnabled(false);
    return;
  }
  /*
   * Check state:
   *  the only state for witch button can be disable is running,
   *  when abort is not supported.
   */
  if( (pvState == mdtTtTestStep::State_t::Running)&&(!pvAbortSupported) ){
    pbRunAbort->setEnabled(false);
  }else{
    pbRunAbort->setEnabled(true);
  }
}
