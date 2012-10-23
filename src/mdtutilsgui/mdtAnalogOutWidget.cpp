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
#include "mdtAnalogOutWidget.h"
#include <qwt_slider.h>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <math.h>
#include <float.h>

mdtAnalogOutWidget::mdtAnalogOutWidget(QWidget *parent)
 : mdtAbstractIoWidget(parent)
{
  QGridLayout *l = new QGridLayout;

  pvStep = 1.0;
  pvStepInverse = 1.0;
  pvCurrentValue = 0.0;
  // Setup GUI
  l->addWidget(lbLabel, 0, 0);
  slValue = new QwtSlider(this, Qt::Vertical, QwtSlider::LeftScale);
  l->addWidget(slValue, 1, 0);
  sbValue = new QDoubleSpinBox;
  l->addWidget(sbValue, 2, 0);
  l->addWidget(pbDetails, 3, 0);
  setLayout(l);
  // Synchronize spinbox and slider
  connect(slValue, SIGNAL(valueChanged(double)), sbValue, SLOT(setValue(double)));
  connect(sbValue, SIGNAL(valueChanged(double)), slValue, SLOT(setValue(double)));
  // value chage signal
  connect(slValue, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged(double)));
  // Set a default range for 0-10V / 8 bit style input
  setRange(0.0, 10.0, 256);
  setValue(0.0);
}

mdtAnalogOutWidget::~mdtAnalogOutWidget()
{
}

void mdtAnalogOutWidget::setRange(double min, double max, int steps)
{
  Q_ASSERT(steps > 1);
  Q_ASSERT(max > min);

  // Set factors
  pvStep = (max-min)/(double)(steps-1);
  pvStepInverse = 1.0/pvStep;
  slValue->setRange(min, max);
  sbValue->setRange(min, max);
  // Display min value
  setValue(min);
}

void mdtAnalogOutWidget::setUnit(const QString &unit)
{
  pvUnit = unit;
}

void mdtAnalogOutWidget::setValue(double value)
{
  if(abs(value - pvCurrentValue) > DBL_EPSILON){
    pvCurrentValue = value;
    sbValue->setValue(value);
    emit(valueChanged(value));
  }
}

double mdtAnalogOutWidget::value()
{
  return pvCurrentValue;
}

void mdtAnalogOutWidget::setValueInt(int value)
{
  double x = pvStep*(double)value + slValue->minValue();

  setValue(x);
}

int mdtAnalogOutWidget::valueInt()
{
  return (pvStepInverse * (pvCurrentValue - slValue->minValue()));
}
