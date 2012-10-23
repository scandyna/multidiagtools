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
#include <qwt_thermo.h>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

mdtAnalogInWidget::mdtAnalogInWidget(QWidget *parent)
 : mdtAbstractIoWidget(parent)
{
  QGridLayout *l = new QGridLayout;

  pvStep = 1.0;
  pvStepInverse = 1.0;
  // Setup GUI
  l->addWidget(lbLabel, 0, 0);
  thValue = new QwtThermo;
  l->addWidget(thValue, 1, 0);
  lbValue = new QLabel;
  lbValue->setAlignment(Qt::AlignHCenter);
  l->addWidget(lbValue, 2, 0);
  l->addWidget(pbDetails, 3, 0);
  setLayout(l);
  // Set a default range for 0-10V / 8 bit style input
  setRange(0.0, 10.0, 256);
  setValue(0.0);
}

mdtAnalogInWidget::~mdtAnalogInWidget()
{
}

void mdtAnalogInWidget::setRange(double min, double max, int steps)
{
  Q_ASSERT(steps > 1);
  Q_ASSERT(max > min);

  // Set factors
  pvStep = (max-min)/(double)(steps-1);
  pvStepInverse = 1.0/pvStep;
  thValue->setRange(min, max);
  // Display min value
  setValue(min);
}

void mdtAnalogInWidget::setUnit(const QString &unit)
{
  pvUnit = unit;
}

double mdtAnalogInWidget::value()
{
  return thValue->value();
}

void mdtAnalogInWidget::setValueInt(int value)
{
  double x = pvStep*(double)value + thValue->minValue();

  setValue(x);
}

int mdtAnalogInWidget::valueInt()
{
  return (pvStepInverse * (thValue->value() - thValue->minValue()));
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
