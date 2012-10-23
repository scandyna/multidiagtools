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
#include "mdtDigitalOutWidget.h"
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

#include <QDebug>

mdtDigitalOutWidget::mdtDigitalOutWidget(QWidget *parent)
 : mdtAbstractIoWidget(parent)
{
  QGridLayout *l = new QGridLayout;

  // Setup GUI
  l->addWidget(lbLabel, 0, 0);
  pbState = new QPushButton;
  pbState->setCheckable(true);
  l->addWidget(pbState, 1, 0);
  l->addWidget(pbDetails, 2, 0);
  setLayout(l);

  connect(pbState, SIGNAL(toggled(bool)), this, SLOT(updateState(bool)));

  setOn(false);
  pbState->setText(tr("OFF"));
}

mdtDigitalOutWidget::~mdtDigitalOutWidget()
{
}

void mdtDigitalOutWidget::setOn(bool on)
{
  if(on != pbState->isChecked()){
    pbState->setChecked(on);
    updateState(on);
  }else{
    pbState->setChecked(on);
  }
}

bool mdtDigitalOutWidget::isOn()
{
  return pbState->isChecked();
}

void mdtDigitalOutWidget::updateState(bool state)
{
  // Update text
  if(state){
    pbState->setText(tr("ON"));
  }else{
    pbState->setText(tr("OFF"));
  }
  emit(stateChanged(state));
}

