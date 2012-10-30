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
#include "mdtAnalogIo.h"
#include <cmath>
#include <float.h>

mdtAnalogIo::mdtAnalogIo(QObject *parent)
 : mdtAbstractIo(parent)
{
  pvValue = 0.0;
  pvMinimum = 0.0;
  pvMaximum = 0.0;
  pvStep = 1.0;
  pvStepInverse = 1.0;
}

mdtAnalogIo::~mdtAnalogIo()
{
}

void mdtAnalogIo::setUnit(const QString &unit)
{
  pvUnit = unit;
  emit(unitChangedForUi(pvUnit));
}

QString mdtAnalogIo::unit() const
{
  return pvUnit;
}

void mdtAnalogIo::setRange(double min, double max, int steps)
{
  Q_ASSERT(steps > 1);
  Q_ASSERT(max > min);

  pvUpdatingUi = false;
  // Set factors
  pvStep = (max-min)/(double)(steps-1);
  pvStepInverse = 1.0/pvStep;
  // Store min and max and signal
  pvMinimum = min;
  pvMaximum = max;
  emit(rangeChangedForUi(min, max));
  // Set value to the minimum
  setValue(min);
}

double mdtAnalogIo::minimum() const
{
  return pvMinimum;
}

double mdtAnalogIo::maximum() const
{
  return pvMaximum;
}

void mdtAnalogIo::setValue(double value)
{
  if(fabs(value - pvValue) > DBL_EPSILON){
    pvValue = value;
    emit(valueChanged(value));
    pvUpdatingUi = true;
    emit(valueChangedForUi(value));
  }
}

double mdtAnalogIo::value() const
{
  return pvValue;
}

void mdtAnalogIo::setValueInt(int value)
{
  double x = pvStep*(double)value + pvMinimum;

  setValue(x);
}

int mdtAnalogIo::valueInt() const
{
  return (pvStepInverse * (pvValue - pvMinimum));
}

void mdtAnalogIo::setValueFromUi(double value)
{
  // If setValue() is called, valueChangedForUi() is emited
  //  and UI will receive the new value. But, because of this update,
  //  it can happen that these same UI emit their signal valueChanged()
  //  and cause finaly the call of this method (with altered precision).
  // We not want this, so check the flag before update.
  if(pvUpdatingUi){
    pvUpdatingUi = false;
    return;
  }
  if(fabs(value - pvValue) > DBL_EPSILON){
    pvValue = value;
    emit(valueChanged(value));
  }
}

