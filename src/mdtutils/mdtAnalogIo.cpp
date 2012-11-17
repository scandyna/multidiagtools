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

//#include <QDebug>

mdtAnalogIo::mdtAnalogIo(QObject *parent)
 : mdtAbstractIo(parent)
{
  pvValue = 0.0;
  pvMinimum = 0.0;
  pvMaximum = 0.0;
  pvStep = 1.0;
  pvStepInverse = 1.0;
  pvIntValueLsbIndex = 0;
  pvIntValueLsbIndexEnc = 0;
  pvIntValueSigned = false;
  // Set a full 1 mask
  pvIntValueMask = -1;
  pvIntValueMaskEnc = -1;
  // Set sign bit mask to 0
  pvIntValueSignMask = 0;
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

bool mdtAnalogIo::setRange(double min, double max, int intValueBitsCount, int intValueLsbIndex, bool intValueSigned)
{
  Q_ASSERT(max > min);

  int i;

  // Check that lsb index and bits count are in acceptable range
  if(intValueLsbIndex < 0){
    mdtError e(MDT_PARSE_ERROR, "Specified intValueLsbIndex is < 0, using current conversion parameters", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtAnalogIo");
    e.commit();
    return false;
  }
  if(intValueBitsCount < 0){
    mdtError e(MDT_PARSE_ERROR, "Specified intValueBitsCount is < 0, using current conversion parameters", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtAnalogIo");
    e.commit();
    return false;
  }
  if((unsigned int)(intValueLsbIndex+intValueBitsCount) > (unsigned int)(8*sizeof(int))){
    mdtError e(MDT_PARSE_ERROR, "Specified intValueLsbIndex + intValueBitsCount is to big, using current conversion parameters", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtAnalogIo");
    e.commit();
    return false;
  }
  // Store lsbIndex
  pvIntValueLsbIndex = intValueLsbIndex;
  pvIntValueLsbIndexEnc = intValueLsbIndex;
  // Setup mask
  pvIntValueMask = 0;
  pvIntValueMaskEnc = 0;
  for(i=0; i<intValueBitsCount; ++i){
    pvIntValueMask += (1<<i);
    pvIntValueMaskEnc += (1<<i);
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

bool mdtAnalogIo::setEncodeBitSettings(int intValueBitsCount, int intValueLsbIndex)
{
  int i;

  // Check that lsb index and bits count are in acceptable range
  if(intValueLsbIndex < 0){
    mdtError e(MDT_PARSE_ERROR, "Specified intValueLsbIndex is < 0, using current conversion parameters", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtAnalogIo");
    e.commit();
    return false;
  }
  if(intValueBitsCount < 0){
    mdtError e(MDT_PARSE_ERROR, "Specified intValueBitsCount is < 0, using current conversion parameters", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtAnalogIo");
    e.commit();
    return false;
  }
  if((unsigned int)(intValueLsbIndex+intValueBitsCount) > (unsigned int)(8*sizeof(int))){
    mdtError e(MDT_PARSE_ERROR, "Specified intValueLsbIndex + intValueBitsCount is to big, using current conversion parameters", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtAnalogIo");
    e.commit();
    return false;
  }
  // Store lsbIndex
  pvIntValueLsbIndexEnc = intValueLsbIndex;
  // Setup mask
  pvIntValueMaskEnc = 0;
  for(i=0; i<intValueBitsCount; ++i){
    pvIntValueMaskEnc += (1<<i);
  }
  // Set factor
  if(pvIntValueSigned){
    pvStepInverse = (pow(2.0, (intValueBitsCount-1)) - 1) / pvMaximum;
  }else{
    pvStepInverse = (pow(2.0, intValueBitsCount) - 1) / (pvMaximum - pvMinimum);
  }

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

void mdtAnalogIo::setValueInt(int value, bool isValid, bool emitValueChanged)
{
  double x;

  if(!isValid){
    setValue(pvMinimum, false, emitValueChanged);
    return;
  }
  // Extract bits in correct range
  value = value >> pvIntValueLsbIndex;
  value &= pvIntValueMask;
  // Apply C1 and negate if sign bit is present
  if(value & pvIntValueSignMask){
    value = ( (~value) & pvIntValueMask );
    value = -value;
  }
  // Make conversion
  if(pvIntValueSigned){
    x = pvStep*(double)value;
  }else{
    x = pvStep*(double)value + pvMinimum;
  }

  setValue(x, true, emitValueChanged);
}

int mdtAnalogIo::valueInt() const
{
  int m;

  // Make conversion
  if(pvIntValueSigned){
    m = pvStepInverse * pvValue;
  }else{
    m = pvStepInverse * (pvValue - pvMinimum);
  }
  // Apply C1 and negate if value < 0
  if(pvIntValueSigned && (pvValue < 0.0)){
    m = -m;
    m = (~m) & pvIntValueMaskEnc;
  }
  m &= pvIntValueMaskEnc;
  m = m << pvIntValueLsbIndexEnc;

  return m;
}

void mdtAnalogIo::setValue(QVariant value, bool emitValueChanged)
{
  if(!value.isValid()){
    setValue(0.0, false, emitValueChanged);
    return;
  }
  // Check type and update
  if(value.type() == QVariant::Double){
    setValue(value.toDouble(), true, emitValueChanged);
  }else if(value.type() == QVariant::Int){
    setValueInt(value.toInt(), true, emitValueChanged);
  }else{
    setValue(0.0, false, emitValueChanged);
  }
}

void mdtAnalogIo::setValue(double value, bool isValid, bool emitValueChanged)
{
  pvHasValidData = isValid;
  if(!pvHasValidData){
    value = pvMinimum;
  }
  if(fabs(value - pvValue) >= pvStep){
    pvValue = value;
    if(emitValueChanged){
      emit(valueChanged(pvAddress, valueInt()));
      emit(valueChanged(pvValue));
    }
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
  if(fabs(value - pvValue) >= pvStep){
    pvValue = value;
    pvHasValidData = true;
    emit(valueChanged(pvAddress, valueInt()));
    emit(valueChanged(pvValue));
  }
}

