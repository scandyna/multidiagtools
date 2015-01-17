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
#include "mdtValue.h"
#include <cfloat>
#include <cmath>
#include <limits>
#include <QtGlobal>
#include <QDebug>

/*
 * mdtValueDouble
 */

mdtValueDouble::mdtValueDouble()
 : pvValue(std::numeric_limits<double>::quiet_NaN()),
   pvIsNull(true)
{
}

mdtValueDouble::mdtValueDouble(double value, bool isOl)
{
  setValue(value, isOl);
}

void mdtValueDouble::clear()
{
  pvValue = std::numeric_limits<double>::quiet_NaN();
  pvIsNull = true;
}

/// \todo Use functions from cmath !
void mdtValueDouble::setValue(double value, bool isOl)
{
  pvIsNull = false;
  if(isOl){
    if( (value > 0.0) || ( (value > -std::numeric_limits<double>::epsilon()) && (value < std::numeric_limits<double>::epsilon()) ) ){
      pvValue = std::numeric_limits<double>::infinity();
    }else{
      pvValue = -std::numeric_limits<double>::infinity();
    }
  }else{
    pvValue = value;
  }
}

bool mdtValueDouble::isMinusOl() const
{
  if(pvValue > 0.0){
    return false;
  }
  return std::isinf(pvValue);
}

bool mdtValueDouble::isPlusOl() const
{
  if(pvValue < 0.0){
    return false;
  }
  return std::isinf(pvValue);
}

bool mdtValueDouble::isNaN() const
{
  return std::isnan(pvValue);
}

/// \todo Use functions from cmath !
bool mdtValueDouble::operator==(const mdtValueDouble & other) const
{
  if(pvIsNull || other.pvIsNull){
    return false;
  }
  if(isMinusOl() && other.isMinusOl()){
    return true;
  }
  if(isPlusOl() && other.isPlusOl()){
    return true;
  }
  return (::fabs(pvValue - other.pvValue) < std::numeric_limits<double>::epsilon());
}

QDebug operator<<(QDebug dbg, const mdtValueDouble & value)
{
  if(value.isNull()){
    return dbg.nospace() << "mdtValueDouble(Null)";
  }
  if(value.isMinusOl()){
    return dbg.nospace() << "mdtValueDouble(-OL)";
  }
  if(value.isPlusOl()){
    return dbg.nospace() << "mdtValueDouble(+OL)";
  }
  if(value.isNaN()){
    return dbg.nospace() << "mdtValueDouble(NaN)";
  }
  return dbg.nospace() << "mdtValueDouble(" << value.value() << ")";
}

/*
 * mdtValueInt
 */

QDebug operator<<(QDebug dbg, const mdtValueInt & value)
{
  if(value.isNull()){
    return dbg.nospace() << "mdtValueInt(Null)";
  }
  return dbg.nospace() << "mdtValueInt(" << value.value() << ")";
}

/*
 * mdtValueBool
 */

QDebug operator<<(QDebug dbg, const mdtValueBool & value)
{
  if(value.isNull()){
    return dbg.nospace() << "mdtValueBool(Null)";
  }
  if(value.value()){
    return dbg.nospace() << "mdtValueBool(true)";
  }
  return dbg.nospace() << "mdtValueBool(false)";
}

/*
 * mdtValue
 */

mdtValue::mdtValue()
{
  setDefaultValue(0.0);
  setDefaultValue(0);
  setDefaultValue(false);
  clear();
}

mdtValue::mdtValue(double value, bool isMinusOl, bool isPlusOl)
{
  setDefaultValue(0.0);
  setDefaultValue(0);
  setDefaultValue(false);
  clear();
  setValue(value, isMinusOl, isPlusOl);
}

mdtValue::mdtValue(int value, bool isMinusOl, bool isPlusOl)
{
  setDefaultValue(0.0);
  setDefaultValue(0);
  setDefaultValue(false);
  clear();
  setValue(value, isMinusOl, isPlusOl);
}

mdtValue::mdtValue(bool value)
{
  setDefaultValue(0.0);
  setDefaultValue(0);
  setDefaultValue(false);
  clear();
  setValue(value);
}

void mdtValue::clear()
{
  pvValueDouble = pvDefaultValueDouble;
  pvValueInt = pvDefaultValueInt;
  pvValueBool = pvDefaultValueBool;
  pvIsValid = false;
  pvHasValueDouble = false;
  pvIsMinusOl = false;
  pvIsPlusOl = false;
  pvHasValueInt = false;
  pvHasValueBool = false;
}

void mdtValue::setDefaultValue(double value)
{
  pvDefaultValueDouble = value;
}

void mdtValue::setDefaultValue(int value)
{
  pvDefaultValueInt = value;
}

void mdtValue::setDefaultValue(bool value)
{
  pvDefaultValueBool = value;
}

void mdtValue::setValue(double value, bool isMinusOl, bool isPlusOl)
{
  pvIsValid = true;
  pvHasValueDouble = true;
  pvValueDouble = value;
  pvIsMinusOl = isMinusOl;
  pvIsPlusOl = isPlusOl;
}

bool mdtValue::hasValueDouble() const
{
  return pvHasValueDouble;
}

double mdtValue::valueDouble() const
{
  return pvValueDouble;
}

void mdtValue::setValue(int value, bool isMinusOl, bool isPlusOl)
{
  pvIsValid = true;
  pvHasValueInt = true;
  pvValueInt = value;
  pvIsMinusOl = isMinusOl;
  pvIsPlusOl = isPlusOl;
}

bool mdtValue::hasValueInt() const
{
  return pvHasValueInt;
}

int mdtValue::valueInt() const
{
  return pvValueInt;
}

bool mdtValue::isMinusOl() const
{
  return pvIsMinusOl;
}

bool mdtValue::isPlusOl() const
{
  return pvIsPlusOl;
}

void mdtValue::setValue(bool value)
{
  pvIsValid = true;
  pvHasValueBool = true;
  pvValueBool = value;
}

bool mdtValue::hasValueBool() const
{
  return pvHasValueBool;
}

bool mdtValue::valueBool() const
{
  return pvValueBool;
}

bool mdtValue::isValid() const
{
  return pvIsValid;
}

bool mdtValue::operator==(const mdtValue &other) const
{
  if(qAbs(pvValueDouble - other.pvValueDouble) > DBL_EPSILON){
    return false;
  }
  if(pvValueInt != other.pvValueInt){
    return false;
  }
  if(pvValueBool != other.pvValueBool){
    return false;
  }
  if(pvIsValid != other.pvIsValid){
    return false;
  }
  if(pvHasValueDouble != other.pvHasValueDouble){
    return false;
  }
  if(pvIsMinusOl != other.pvIsMinusOl){
    return false;
  }
  if(pvIsPlusOl != other.pvIsPlusOl){
    return false;
  }
  if(pvHasValueInt != other.pvHasValueInt){
    return false;
  }
  if(pvHasValueBool != other.pvHasValueBool){
    return false;
  }
  return true;
}

bool mdtValue::operator!=(const mdtValue &other) const
{
  return !(operator==(other));
}

QDebug operator<<(QDebug dbg, const mdtValue &value)
{
  return dbg.nospace() << "mdtValue(isValid: " << value.isValid() \
                        << ", Double: " << value.valueDouble() << ", Int: " << value.valueInt() << ", Bool: " << value.valueBool() \
                        << ", -OL: " << value.isMinusOl() << ", +OL: " << value.isPlusOl() << ")";
}
