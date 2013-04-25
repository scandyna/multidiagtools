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
#include "mdtValue.h"
#include <cfloat>
#include <QtGlobal>

mdtValue::mdtValue()
{
  clear();
}

mdtValue::mdtValue(double value, bool isMinusOl, bool isPlusOl)
{
  clear();
  setValue(value, isMinusOl, isPlusOl);
}

mdtValue::mdtValue(int value, bool isMinusOl, bool isPlusOl)
{
  clear();
  setValue(value, isMinusOl, isPlusOl);
}

mdtValue::mdtValue(bool value)
{
  clear();
  setValue(value);
}

void mdtValue::clear()
{
  pvValueDouble = 0.0;
  pvValueInt = 0;
  pvValueBool = false;
  pvIsValid = false;
  pvHasValueDouble = false;
  pvIsMinusOl = false;
  pvIsPlusOl = false;
  pvHasValueInt = false;
  pvHasValueBool = false;
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
