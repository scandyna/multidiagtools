/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
  ///pvValue = 0.0;
  pvNotifyUi = false;
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
  emit(rangeChangedForUi(min, max));
  // Set value to the minimum
  pvNotifyUi = true;
  setValueFromDouble(min, false);
  pvValue.setDefaultValue(pvValue.valueDouble());
  pvValue.setDefaultValue(pvValue.valueInt());
  pvValue.clear();
  ///pvUpdatingUi = false;

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
  return pvValue.valueDouble();
}

/**
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
*/

void mdtAnalogIo::setValueInt(int value, bool isValid, bool emitValueChanged)
{
  mdtValue v;

  if(isValid){
    v.setValue(value);
  }
  pvNotifyUi = true;
  setValueFromInt(v, emitValueChanged);
}

/**
int mdtAnalogIo::valueInt() const
{
  int m;
  double value = pvValue.valueDouble();

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
*/

int mdtAnalogIo::valueInt() const
{
  return pvValue.valueInt();
}

/**
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
*/

/**
void mdtAnalogIo::setValue(QVariant value, bool emitValueChanged)
{
  mdtValue v;

  if(!value.isValid()){
    setValueFromDouble(v, emitValueChanged);
    return;
  }
  // Check type and update
  if(value.type() == QVariant::Double){
    v.setValue(value.toDouble());
    setValueFromDouble(v, emitValueChanged);
  }else if(value.type() == QVariant::Int){
    v.setValue(value.toInt());
    setValueFromInt(v, emitValueChanged);
  }else{
    setValueFromDouble(v, emitValueChanged);
  }
}
*/

/**
void mdtAnalogIo::setValue(double value, bool isValid, bool emitValueChanged)
{
  pvHasValidData = isValid;
  if(!pvHasValidData){
    value = pvMinimum;
  }
  if(fabs(value - pvValue) >= pvStep){
    pvValue = value;
    if(emitValueChanged){
      emit(valueChanged(addressWrite()));
      emit(valueChanged(pvValue));
    }
    pvUpdatingUi = true;
    emit(valueChangedForUi(value));
  }
}
*/

/**
void mdtAnalogIo::setValue(double value, bool isValid, bool emitValueChanged)
{
  mdtValue v;

  if(isValid){
    v.setValue(value);
  }
  setValueFromDouble(v, emitValueChanged);
}
*/

void mdtAnalogIo::setValue(const mdtValue &value, bool emitValueChanged)
{
  pvNotifyUi = true;
  if(value.hasValueDouble()){
    setValueFromDouble(value, emitValueChanged);
  }else{
    setValueFromInt(value, emitValueChanged);
  }
}

/**
void mdtAnalogIo::setValue(double value)
{
  setValue(value, true);
}
*/

void mdtAnalogIo::setValueFromUi(double value)
{
  mdtValue v;

  // If setValue() is called, valueChangedForUi() is emited
  //  and UI will receive the new value. But, because of this update,
  //  it can happen that these same UI emit their signal valueChanged()
  //  and cause finaly the call of this method (with altered precision).
  // We not want this, so check the flag before update.
  /**
  if(pvUpdatingUi){
    pvUpdatingUi = false;
    return;
  }
  */
  v.setValue(value);
  pvNotifyUi = false;
  setValueFromDouble(v, true);
  ///setValueFromDouble(value, true);
  /**
  if(fabs(value - pvValue) >= pvStep){
    pvValue = value;
    pvHasValidData = true;
    emit(valueChanged(addressWrite()));
    emit(valueChanged(pvValue));
  }
  */
}

void mdtAnalogIo::setValueFromDouble(const mdtValue &value, bool emitValueChanged)
{
  int m;
  double x = value.valueDouble();

  // Check if new value has changed
  if((fabs(x - pvValue.valueDouble()) <= pvStep)&&(value.isValid() == pvValue.isValid())){
    return;
  }
  // Check validity of new value and process if true
  if((value.isValid())&&(value.hasValueDouble())){
    // Make conversion
    if(pvIntValueSigned){
      m = pvStepInverse * x;
    }else{
      m = pvStepInverse * (x - pvMinimum);
    }
    // Apply C1 and negate if value < 0
    if(pvIntValueSigned && (x < 0.0)){
      m = -m;
      m = (~m) & pvIntValueMaskEnc;
    }
    m &= pvIntValueMaskEnc;
    m = m << pvIntValueLsbIndexEnc;
    // Store value
    pvValue.setValue(m);
    pvValue.setValue(x, value.isMinusOl(), value.isPlusOl());
  }else{
    pvValue.clear();
    ///x = pvValue.valueDouble();
  }
  ///qDebug() << "New value from double " << pvValue;
  // Check if new value has changed
  /**
  if(fabs(x - pvValue.valueDouble()) <= pvStep){
    return;
  }
  */
  // Notify
  ///pvUpdatingUi = true;
  if(pvNotifyUi){
    emit(valueChangedForUi(pvValue));
  }
  if(emitValueChanged){
    ///qDebug() << "New value from double: emit valueChanged()";
    ///emit(valueChanged(addressWrite()));
    ///emit(valueChanged(x));
    emit(valueChanged(pvValue));
  }
}

void mdtAnalogIo::setValueFromInt(const mdtValue &value, bool emitValueChanged)
{
  int m = value.valueInt();
  double x;

  // Check if new value has changed
  if((m == pvValue.valueInt())&&(value.isValid() == pvValue.isValid())){
    return;
  }
  // Check validity of new value and process if true
  if((value.isValid())&&(value.hasValueInt())){
    m = value.valueInt();
    // Extract bits in correct range
    m = m >> pvIntValueLsbIndex;
    m &= pvIntValueMask;
    // Apply C1 and negate if sign bit is present
    if(m & pvIntValueSignMask){
      m = ( (~m) & pvIntValueMask );
      m = -m;
    }
    // Make conversion
    if(pvIntValueSigned){
      x = pvStep*(double)m;
    }else{
      x = pvStep*(double)m + pvMinimum;
    }
    // Store
    pvValue.setValue(x);
    pvValue.setValue(m, value.isMinusOl(), value.isPlusOl());
  }else{
    pvValue.clear();
    x = pvValue.valueDouble();
  }
  ///qDebug() << "New value from int: " << pvValue;
  // Check if new value has changed
  /**
  if(fabs(x - pvValue.valueDouble()) <= pvStep){
    return;
  }
  */
  // Notify
  ///pvUpdatingUi = true;
  if(pvNotifyUi){
    emit(valueChangedForUi(pvValue));
  }
  if(emitValueChanged){
    ///qDebug() << "New value from int: emit valueChanged()";
    ///emit(valueChanged(addressWrite()));
    ///emit(valueChanged(x));
    emit(valueChanged(pvValue));
  }
}
