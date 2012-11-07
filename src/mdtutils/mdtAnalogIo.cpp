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
#include "mdtError.h"
#include <cmath>
#include <float.h>

#include <QDebug>

mdtAnalogIo::mdtAnalogIo(QObject *parent)
 : mdtAbstractIo(parent)
{
  pvValue = 0.0;
  pvMinimum = 0.0;
  pvMaximum = 0.0;
  pvStep = 1.0;
  pvStepInverse = 1.0;
  pvIntValueLsbIndex = 0;
  pvIntValueSigned = false;
  // Set a full 1 mask
  pvIntValueMask = -1;
  // Set sign bit mask to 0
  pvIntValueSignMask = 0;
  qDebug() << "pvIntValueMask: 0x" << hex << (unsigned int)pvIntValueMask;
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

/**
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
  setValue(min, false);
}
*/

bool mdtAnalogIo::setRange(double min, double max, int intValueBitsCount, int intValueLsbIndex, bool intValueSigned)
{
  Q_ASSERT(max > min);

  int i;

  // Chack that lsb index and bits count are in acceptable range
  if(intValueLsbIndex < 0){
    mdtError e(MDT_PARSE_ERROR, "Specified intValueLsbIndex is < 0, using default conversion parameters", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtAnalogIo");
    e.commit();
    return false;
  }
  if(intValueBitsCount < 0){
    mdtError e(MDT_PARSE_ERROR, "Specified intValueBitsCount is < 0, using default conversion parameters", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtAnalogIo");
    e.commit();
    return false;
  }
  if((unsigned int)(intValueLsbIndex+intValueBitsCount) > (unsigned int)(8*sizeof(int))){
    mdtError e(MDT_PARSE_ERROR, "Specified intValueLsbIndex/intValueBitsCount is to big, using default conversion parameters", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtAnalogIo");
    e.commit();
    return false;
  }
  // Store lsbIndex
  pvIntValueLsbIndex = intValueLsbIndex;
  // Setup mask
  pvIntValueMask = 0;
  for(i=0; i<intValueBitsCount; ++i){
    pvIntValueMask += (1<<i);
  }
  // Set factors + sign mask
  pvIntValueSigned = intValueSigned;
  if(intValueSigned){
    pvStep = max / (pow(2.0, (intValueBitsCount-1)) - 1);
    pvIntValueSignMask = (1 << (intValueBitsCount-1) );
  }else{
    pvStep = (max-min) / (pow(2.0, intValueBitsCount) - 1);
    pvIntValueSignMask = 0;
  }
  qDebug() << "     mask: " << hex << (unsigned int)pvIntValueMask;
  qDebug() << "Sign mask: " << hex << (unsigned int)pvIntValueSignMask;
  pvStepInverse = 1.0/pvStep;

  // Store min and max and signal
  pvMinimum = min;
  pvMaximum = max;
  pvUpdatingUi = false;
  emit(rangeChangedForUi(min, max));
  // Set value to the minimum
  setValue(min, false);

  return true;
}

double mdtAnalogIo::minimum() const
{
  return pvMinimum;
}

double mdtAnalogIo::maximum() const
{
  return pvMaximum;
}

double mdtAnalogIo::value() const
{
  return pvValue;
}

/**
bool mdtAnalogIo::setValueBitsRange(int lsbIndex, int bitsCount)
{
  int i;

  // Chack that lsb index is valid
  if(lsbIndex < 0){
    mdtError e(MDT_PARSE_ERROR, "Specified lsbIndex is < 0, using default conversion parameters", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtAnalogIo");
    e.commit();
    return false;
  }
  if(bitsCount < 0){
    mdtError e(MDT_PARSE_ERROR, "Specified bitsCount is < 0, using default conversion parameters", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtAnalogIo");
    e.commit();
    return false;
  }
  if((unsigned int)(lsbIndex+bitsCount) > (unsigned int)(8*sizeof(int))){
    mdtError e(MDT_PARSE_ERROR, "Specified lsbIndex/bitsCount is to big, using default conversion parameters", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtAnalogIo");
    e.commit();
    return false;
  }
  // Store lsbIndex
  pvIntValueLsbIndex = lsbIndex;
  // Setup mask
  pvIntValueMask = 0;
  for(i=0; i<bitsCount; ++i){
    pvIntValueMask += (1<<i);
    qDebug() << "mask: " << hex << (unsigned int)pvIntValueMask;
  }
  
  return true;
}
*/

void mdtAnalogIo::setValueInt(int value, bool isValid)
{
  double x;

  if(!isValid){
    setValue(pvMinimum, false);
    return;
  }
  // Extract bits in correct range
  qDebug() << "value: " << value << " (hex: " << hex << value << ")";
  value = value >> pvIntValueLsbIndex;
  value &= pvIntValueMask;
  qDebug() << "value: " << value << " (hex: " << hex << value << ")";
  // Apply C1 and negate if sign bit is present
  if(value & pvIntValueSignMask){
    value = ( (~value) & pvIntValueMask );
    value = -value;
    qDebug() << "value: " << value << " (hex: " << hex << value << ")";
  }
  // Make conversion
  if(pvIntValueSigned){
    x = pvStep*(double)value;
  }else{
    x = pvStep*(double)value + pvMinimum;
  }
  ///qDebug() << "value: " << value << " (hex: " << hex << value << ")";

  setValue(x, true);
}

int mdtAnalogIo::valueInt() const
{
  return (pvStepInverse * (pvValue - pvMinimum));
}

void mdtAnalogIo::setValue(double value, bool isValid)
{
  pvHasValidData = isValid;
  if(!pvHasValidData){
    value = pvMinimum;
  }
  if(fabs(value - pvValue) > DBL_EPSILON){
    pvValue = value;
    emit(valueChanged(value));
    emit(valueChanged(pvAddress, value));
    pvUpdatingUi = true;
    emit(valueChangedForUi(value));
  }
}

void mdtAnalogIo::setValue(double value)
{
  setValue(value, true);
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
    pvHasValidData = true;
    emit(valueChanged(value));
    emit(valueChanged(pvAddress, value));
  }
}

