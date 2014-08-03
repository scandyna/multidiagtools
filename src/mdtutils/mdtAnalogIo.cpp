/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include <limits>

//#include <QDebug>

using namespace std;

mdtAnalogIo::mdtAnalogIo(QObject *parent)
 : mdtAbstractIo(parent),
   pvConverter(nullptr)
{
  pvMinimum = 0.0;
  pvMaximum = 0.0;
  /**
  pvStepQ = 1.0;
  pvStepAD = 1.0;
  pvIntValueLsbIndex = 0;
  pvIntValueLsbIndexEnc = 0;
  pvIntValueSigned = false;
  pvScaleFromMinToMax = false;
  // Set a full 1 mask
  pvIntValueMask = -1;
  pvIntValueMaskEnc = -1;
  // Set sign bit mask to 0
  pvIntValueSignMask = 0;
  */
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

void mdtAnalogIo::setRange(double min, double max, int intValueBitsCount, int intValueLsbIndex, bool intValueSigned, bool scaleFromMinToMax, double conversionFactor)
{
  Q_ASSERT(max > min);
  Q_ASSERT((qAbs(conversionFactor) - DBL_EPSILON) > 0.0);

  if(!pvConverter){
    pvConverter = unique_ptr<mdtBinaryDoubleConverter<int> >(new mdtBinaryDoubleConverter<int>);
  }
  pvConverter->setRange(min, max, intValueBitsCount, intValueLsbIndex, intValueSigned, scaleFromMinToMax, conversionFactor);
  

  /**
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
  for(i = 0; i < intValueBitsCount; ++i){
    pvIntValueMask += (1<<i);
    pvIntValueMaskEnc += (1<<i);
  }
  // Set factors + sign mask
  pvIntValueSigned = intValueSigned;
  pvScaleFromMinToMax = scaleFromMinToMax;
  if(pvScaleFromMinToMax){
    pvStepQ = conversionFactor * (max-min) / (pow(2.0, intValueBitsCount));
  }else{
    pvStepQ = conversionFactor;
  }
  pvIntValueSignMask = (1 << (intValueBitsCount-1) );
  pvStepAD = 1.0/pvStepQ;
  */

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

  ///return true;
}

double mdtAnalogIo::minimum() const
{
  return pvMinimum;
}

double mdtAnalogIo::maximum() const
{
  return pvMaximum;
}

void mdtAnalogIo::setValueInt(int value, bool isValid, bool emitValueChanged)
{
  mdtValue v;

  if(isValid){
    v.setValue(value);
  }
  pvNotifyUi = true;
  setValueFromInt(v, emitValueChanged);
}

void mdtAnalogIo::setValue(const mdtValue &value, bool emitValueChanged)
{
  pvNotifyUi = true;
  if(value.hasValueDouble()){
    setValueFromDouble(value, emitValueChanged);
  }else{
    setValueFromInt(value, emitValueChanged);
  }
}

void mdtAnalogIo::setValueFromUi(double value)
{
  mdtValue v;

  // If setValue() is called, valueChangedForUi() is emited
  //  and UI will receive the new value. But, because of this update,
  //  it can happen that these same UI emit their signal valueChanged()
  //  and cause finaly the call of this method (with altered precision).
  // We not want this, so check the flag before update.
  v.setValue(value);
  pvNotifyUi = false;
  setValueFromDouble(v, true);
}

void mdtAnalogIo::setValueFromDouble(const mdtValue &value, bool emitValueChanged)
{
  ///int m;
  double x = value.valueDouble();

  // Check if new value has changed
  if((qAbs<double>(x - pvValue.valueDouble()) <= std::numeric_limits<double>::min()) && (value.isValid() == pvValue.isValid())){
    return;
  }
  // Check validity of new value and process if true
  if((value.isValid())&&(value.hasValueDouble())){
    if(pvConverter){
      pvValue.setValue(pvConverter->doubleToBinary(x));
    }
    pvValue.setValue(x, value.isMinusOl(), value.isPlusOl());
  }else{
    pvValue.clear();
  }
  /**
  // Check if new value has changed
  if((fabs(x - pvValue.valueDouble()) <= pvStepQ)&&(value.isValid() == pvValue.isValid())){
    return;
  }
  // Check validity of new value and process if true
  if((value.isValid())&&(value.hasValueDouble())){
    // We must clip value, else we can have strongly wrong errors (underflow/overflow errors)
    if(x <= (pvMinimum + pvStepQ)){
      x = pvMinimum;
    }else if(x >= (pvMaximum - pvStepQ)){
      x = pvMaximum - pvStepQ;
    }
    // Make conversion
    if(pvIntValueSigned){
      m = pvStepAD * x;
    }else{
      if(pvScaleFromMinToMax){
        m = pvStepAD * (x - pvMinimum);
      }else{
        m = pvStepAD * x;
      }
    }
    // Apply C2 and negate if value < 0
    if(pvIntValueSigned && (x < 0.0)){
      m = -m;
      m = (~m) & pvIntValueMaskEnc; // C1
      ++m;  // C2
    }
    m &= pvIntValueMaskEnc;
    m = m << pvIntValueLsbIndexEnc;
    // Store value
    pvValue.setValue(m);
    pvValue.setValue(x, value.isMinusOl(), value.isPlusOl());
  }else{
    pvValue.clear();
  }
  */
  // Check if new value has changed
  // Notify
  if(pvNotifyUi){
    emit(valueChangedForUi(pvValue));
  }
  if(emitValueChanged){
    emit(valueChanged(this));
    emit(valueChanged(pvValue));
  }
}

void mdtAnalogIo::setValueFromInt(const mdtValue &value, bool emitValueChanged)
{
  int m = value.valueInt();
  ///double x;

  // Check if new value has changed
  if((m == pvValue.valueInt())&&(value.isValid() == pvValue.isValid())){
    return;
  }
  if((value.isValid())&&(value.hasValueInt())){
    if(pvConverter){
      pvValue.setValue(pvConverter->binaryToDouble(m));
    }
    pvValue.setValue(m, value.isMinusOl(), value.isPlusOl());
  }else{
    pvValue.clear();
  }
  /**
  // Check validity of new value and process if true
  if((value.isValid())&&(value.hasValueInt())){
    m = value.valueInt();
    // Extract bits in correct range
    m = m >> pvIntValueLsbIndex;
    m &= pvIntValueMask;
    // Apply C2 and negate if sign bit is present
    if((pvIntValueSigned)&&(m & pvIntValueSignMask)){
      m = ( (~m) & pvIntValueMask );  // C1
      ++m;  // C2
      m = -m;
    }
    // Make conversion
    if(pvIntValueSigned){
      x = pvStepQ*(double)m;
    }else{
      if(pvScaleFromMinToMax){
        x = pvStepQ*(double)m + pvMinimum;
      }else{
        x = pvStepQ*(double)m;
      }
    }
    // Store
    pvValue.setValue(x);
    pvValue.setValue(m, value.isMinusOl(), value.isPlusOl());
  }else{
    pvValue.clear();
  }
  */
  // Notify
  if(pvNotifyUi){
    emit(valueChangedForUi(pvValue));
  }
  if(emitValueChanged){
    emit(valueChanged(this));
    emit(valueChanged(pvValue));
  }
}
