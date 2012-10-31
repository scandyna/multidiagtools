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
#include "mdtAnalogInWidget.h"
#include "mdtAnalogIo.h"
#include <qwt_thermo.h>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

mdtAnalogInWidget::mdtAnalogInWidget(QWidget *parent)
 : mdtAbstractIoWidget(parent)
{
  QGridLayout *l = new QGridLayout;

  // Setup GUI
  l->addWidget(lbLabel, 0, 0);
  thValue = new QwtThermo;
  l->addWidget(thValue, 1, 0);
  lbValue = new QLabel;
  lbValue->setAlignment(Qt::AlignHCenter);
  l->addWidget(lbValue, 2, 0);
  l->addWidget(pbDetails, 3, 0);
  setLayout(l);
}

mdtAnalogInWidget::~mdtAnalogInWidget()
{
}

void mdtAnalogInWidget::setIo(mdtAnalogIo *io)
{
  Q_ASSERT(io != 0);

  // Base Signals/slots connections
  mdtAbstractIoWidget::setIo(io);
  // Signals/slots from io to widget
  connect(io, SIGNAL(unitChangedForUi(const QString&)), this, SLOT(setUnit(const QString&)));
  connect(io, SIGNAL(rangeChangedForUi(double, double)), this, SLOT(setRange(double, double)));
  connect(io, SIGNAL(valueChangedForUi(double)), this, SLOT(setValue(double)));
  // Set initial data
  setUnit(io->unit());
  setRange(io->minimum(), io->maximum());
  setValue(io->value());
}

void mdtAnalogInWidget::setUnit(const QString &unit)
{
  pvUnit = unit;
}

void mdtAnalogInWidget::setRange(double min, double max)
{
  thValue->setRange(min, max);
}

void mdtAnalogInWidget::setValue(double value)
{
  thValue->setValue(value);
  if(pvUnit.isEmpty()){
    lbValue->setText(QString::number(value, 'g', 4));
  }else{
    lbValue->setText(QString::number(value, 'g', 4) + " " + pvUnit);
  }
}
