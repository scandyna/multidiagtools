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
#include "mdtDigitalIo.h"

//#include <QDebug>

mdtDigitalIo::mdtDigitalIo(QObject *parent)
 : mdtAbstractIo(parent)
{
}

mdtDigitalIo::~mdtDigitalIo()
{
}

void mdtDigitalIo::setValue(const mdtValue &value, bool emitValueChanged)
{
  // Check if state has changed
  if((pvValue.isValid() == value.isValid())&&(pvValue.valueBool() == value.valueBool())){
    return;
  }
  // Check validity and store
  if(!value.isValid()){
    pvValue.clear();
  }else{
    pvValue.setValue(value.valueBool());
  }
  // Notify
  emit(valueChangedForUi(pvValue));
  if(emitValueChanged){
    emit(valueChanged(this));
    emit(valueChanged(pvValue));
  }
}

void mdtDigitalIo::setStateFromUi(bool on)
{
  // Check if state has changed
  if((pvValue.isValid())&&(pvValue.valueBool() == on)){
    return;
  }
  // Store and Notify
  pvValue.setValue(on);
  emit(valueChanged(this));
  emit(valueChanged(pvValue));
}
