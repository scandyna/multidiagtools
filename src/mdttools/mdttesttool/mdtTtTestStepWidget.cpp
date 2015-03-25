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
   pvLayout(new QGridLayout)
{
  // Setup run/abort button
  pbRunAbort = new QPushButton;
  connect(pbRunAbort, SIGNAL(clicked()), this, SLOT(runAbort()));
  pvLayout->addWidget(pbRunAbort, 0, 0);
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
  pvLayout->addWidget(lbState, 0, 2);
  // Setup message label
  lbMessage = new QLabel;
  lbMessage->setTextFormat(Qt::PlainText);
  pvLayout->addWidget(lbMessage, 0, 3);
  // Other setup
  pvLayout->setColumnStretch(4, 2);
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

void mdtTtTestStepWidget::setMessage(const mdtError & error)
{
  lbMessage->setText(error.text() + "\n" + error.informativeText());
}

void mdtTtTestStepWidget::clearMessage()
{
  lbMessage->clear();
}

void mdtTtTestStepWidget::setRunning()
{
  setStateRunning();
}

void mdtTtTestStepWidget::setFinishedSuccess()
{
  setStateSuccess();
}

void mdtTtTestStepWidget::setFinishedWarn()
{
  setStateWarn();
}

void mdtTtTestStepWidget::setFinishedWarn(const QString & msg)
{
  setMessage(msg);
  setStateWarn();
}

void mdtTtTestStepWidget::setFinishedWarn(const mdtError& msg)
{
  setMessage(msg);
  setStateWarn();
}

void mdtTtTestStepWidget::setFinishedFail()
{
  setStateFail();
}

void mdtTtTestStepWidget::setFinishedFail(const QString & msg)
{
  setMessage(msg);
  setStateFail();
}

void mdtTtTestStepWidget::setFinishedFail(const mdtError& msg)
{
  setMessage(msg);
  setStateFail();
}

void mdtTtTestStepWidget::reset()
{
  setStateInitial();
}

void mdtTtTestStepWidget::setRunAbortEnabled(bool enable)
{
  pbRunAbort->setEnabled(enable);
}

void mdtTtTestStepWidget::runAbort()
{
  if(pvState == State_t::Running){
    emit abortCalled();
    setStateInitial();
  }else{
    setStateRunning();
    emit runCalled();
  }
}

void mdtTtTestStepWidget::setStateInitial()
{
  pvState = State_t::Initial;
  // Update run/abort button
  pbRunAbort->setText(tr("Run"));
  pbRunAbort->setIcon(QIcon::fromTheme("media-playback-start"));
  // Update state label
  lbState->setStyleSheet("");
  lbState->setText("");
  // Signal new state
  
  emit finished(this);
}

void mdtTtTestStepWidget::setStateRunning()
{
  pvState = State_t::Running;
  // Update run/abort button
  pbRunAbort->setText(tr("Abort"));
  pbRunAbort->setIcon(QIcon::fromTheme("media-playback-stop"));
  // Update state label
  lbState->setStyleSheet("");
  lbState->setText("Running ...");
  // Signal new state

  emit started(this);
}

void mdtTtTestStepWidget::setStateFail()
{
  pvState = State_t::Fail;
  // Update run/abort button
  pbRunAbort->setText(tr("Run"));
  pbRunAbort->setIcon(QIcon::fromTheme("media-playback-start"));
  // Update state label
  lbState->setStyleSheet("background-color: rgb(255,0,0);\ncolor: rgb(255,255,255);");
  lbState->setText("FAIL");
  // Signal new state

  emit finished(this);
}

void mdtTtTestStepWidget::setStateWarn()
{
  pvState = State_t::Warn;
  // Update run/abort button
  pbRunAbort->setText(tr("Run"));
  pbRunAbort->setIcon(QIcon::fromTheme("media-playback-start"));
  // Update state label
  lbState->setStyleSheet("background-color: rgb(255,170,0);\ncolor: rgb(0,0,0);");
  lbState->setText("Warn");
  // Signal new state

  emit finished(this);
}

void mdtTtTestStepWidget::setStateSuccess()
{
  pvState = State_t::Success;
  // Update run/abort button
  pbRunAbort->setText(tr("Run"));
  pbRunAbort->setIcon(QIcon::fromTheme("media-playback-start"));
  // Update state label
  lbState->setStyleSheet("background-color: rgb(85,255,0);\ncolor: rgb(0,0,0);");
  lbState->setText("Ok");
  // Signal new state

  emit finished(this);
}
