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
#include "mdtTtTestItemWidget.h"

#include <QGridLayout>
#include <QHBoxLayout>

#include <QPushButton>
#include <QLabel>
#include <QIcon>
#include <QFont>
#include <QFontMetrics>

#include <QDebug>

mdtTtTestItemWidget::mdtTtTestItemWidget(QWidget* parent)
 : QWidget(parent),
   ///pvLayout(new QGridLayout)
   pvLayout(new QHBoxLayout)
{
  pbRunAbort = new QPushButton;
  connect(pbRunAbort, SIGNAL(clicked()), this, SLOT(runAbort()));
  ///pvLayout->addWidget(pbRunAbort, 0, 0);
  pvLayout->addWidget(pbRunAbort);
  lbTitle = new QLabel;
  ///pvLayout->addWidget(lbTitle, 0, 1);
  lbTitle->setTextFormat(Qt::PlainText);
  pvLayout->addWidget(lbTitle);
  lbState = new QLabel;
  lbState->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  QFont f("Sans Serif", 12);
  f.setBold(true);
  lbState->setFont(f);
  lbState->setTextFormat(Qt::PlainText);
  ///pvLayout->addWidget(lbState, 0, 2);
  pvLayout->addWidget(lbState);
  lbMessage = new QLabel;
  lbMessage->setTextFormat(Qt::PlainText);
  ///lbMessage->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
  ///pvLayout->addWidget(lbMessage, 0, 3);
  pvLayout->addWidget(lbMessage);
  pvLayout->addStretch(1);

  ///pvLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
  ///pvLayout->setSizeConstraint(QLayout::SetMinimumSize);
  ///pvLayout->setColumnStretch(1, 1);
  ///pvLayout->setColumnStretch(3, 1);
  setLayout(pvLayout);
  setStateInitial();
}

void mdtTtTestItemWidget::setTitle(const QString & text)
{
  lbTitle->setText(text);
//   QFontMetrics fm(lbTitle->font());
//   lbTitle->setMinimumWidth(fm.width(text));
}

void mdtTtTestItemWidget::setMessage(const QString & msg)
{
  lbMessage->setText(msg);
}

void mdtTtTestItemWidget::setMessage(const mdtError & error)
{
  lbMessage->setText(error.text() + "\n" + error.informativeText());
}

void mdtTtTestItemWidget::clearMessage()
{
  lbMessage->clear();
}

void mdtTtTestItemWidget::setRunning()
{
  setStateRunning();
}

void mdtTtTestItemWidget::setFinishedSuccess()
{
  setStateSuccess();
}

void mdtTtTestItemWidget::setFinishedWarn()
{
  setStateWarn();
}

void mdtTtTestItemWidget::setFinishedFail()
{
  setStateFail();
}

void mdtTtTestItemWidget::reset()
{
  setStateInitial();
}

void mdtTtTestItemWidget::setRunAbortEnabled(bool enable)
{
  pbRunAbort->setEnabled(enable);
}

void mdtTtTestItemWidget::runAbort()
{
  if(pvState == State_t::Running){
    emit abortCalled();
    setStateInitial();
  }else{
    setStateRunning();
    emit runCalled();
  }
}

void mdtTtTestItemWidget::setStateInitial()
{
  pvState = State_t::Initial;
  // Update run/abort button
  pbRunAbort->setText(tr("Run"));
  pbRunAbort->setIcon(QIcon::fromTheme("media-playback-start"));
  ///pbRunAbort->setEnabled(true);
  // Update state label
  lbState->setStyleSheet("");
  lbState->setText("Initial ...");
  // Signal new state
  
  emit finished(this);
}

void mdtTtTestItemWidget::setStateRunning()
{
  pvState = State_t::Running;
  // Update run/abort button
  pbRunAbort->setText(tr("Abort"));
  pbRunAbort->setIcon(QIcon::fromTheme("media-playback-stop"));
  ///pbRunAbort->setEnabled(true);
  // Update state label
//   qDebug() << "sizeHint(): " << sizeHint();
//   qDebug() << "lbState->sizeHint(): " << lbState->sizeHint();
//   qDebug() << "lbState->setText(Running ...)";
  lbState->setStyleSheet("");
  lbState->setText("Running ...");
//   qDebug() << "lbState->sizeHint(): " << lbState->sizeHint();
  ///qDebug() << "lbState->updateGeometry()";
  ///lbState->updateGeometry();
  ///qDebug() << "lbState->sizeHint(): " << lbState->sizeHint();
//   qDebug() << "sizeHint(): " << sizeHint();
//   qDebug() << "isVisible(): " << isVisible();
//   qDebug() << "lbState->isVisible(): " << lbState->isVisible();
//   updateGeometry();
//   parentWidget()->updateGeometry();
//   pvLayout->update();
  // Signal new state

  emit started(this);
}

void mdtTtTestItemWidget::setStateFail()
{
  pvState = State_t::Fail;
  // Update run/abort button
  pbRunAbort->setText(tr("Run"));
  pbRunAbort->setIcon(QIcon::fromTheme("media-playback-start"));
  ///pbRunAbort->setEnabled(true);
  // Update state label
  lbState->setStyleSheet("background-color: rgb(255,0,0);\ncolor: rgb(255,255,255);");
  lbState->setText("FAIL");
  // Signal new state

  emit finished(this);
}

void mdtTtTestItemWidget::setStateWarn()
{
  pvState = State_t::Warn;
  // Update run/abort button
  pbRunAbort->setText(tr("Run"));
  pbRunAbort->setIcon(QIcon::fromTheme("media-playback-start"));
  ///pbRunAbort->setEnabled(true);
  // Update state label
  lbState->setStyleSheet("background-color: rgb(255,170,0);\ncolor: rgb(0,0,0);");
  lbState->setText("Warn");
  // Signal new state

  emit finished(this);
}

void mdtTtTestItemWidget::setStateSuccess()
{
  pvState = State_t::Success;
  // Update run/abort button
  pbRunAbort->setText(tr("Run"));
  pbRunAbort->setIcon(QIcon::fromTheme("media-playback-start"));
  ///pbRunAbort->setEnabled(true);
  // Update state label
  lbState->setStyleSheet("background-color: rgb(85,255,0);\ncolor: rgb(0,0,0);");
  lbState->setText("Ok");
  // Signal new state

  emit finished(this);
}
