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
  pvIsOn = false;
}

mdtDigitalIo::~mdtDigitalIo()
{
}

bool mdtDigitalIo::isOn() const
{
  return pvIsOn;
}

void mdtDigitalIo::setOn(QVariant on, bool emitValueChanged)
{
  if((!on.isValid())||(on.type() != QVariant::Bool)){
    setOn(false, false, emitValueChanged);
  }else{
    setOn(on.toBool(), true, emitValueChanged);
  }
}

void mdtDigitalIo::setOn(bool on, bool isValid, bool emitValueChanged)
{
  bool notifyUi = false;

  // Check if UI must be updated
  if((on != pvIsOn)||(isValid != pvHasValidData)){
    notifyUi = true;
  }
  // Check validity
  pvHasValidData = isValid;
  if(!pvHasValidData){
    on = false;
  }
  if(pvIsOn != on){
    pvIsOn = on;
    if(emitValueChanged){
      emit(stateChanged(pvIsOn));
      emit(stateChanged(addressWrite()));
    }
  }
  if(notifyUi){
    emit(stateChangedForUi(pvIsOn));
  }
}

void mdtDigitalIo::setOn(bool on)
{
  setOn(on, true);
}

void mdtDigitalIo::setStateFromUi(bool on)
{
  if(pvIsOn != on){
    pvHasValidData = true;
    pvIsOn = on;
    emit(stateChanged(pvIsOn));
    emit(stateChanged(addressWrite()));
  }
}
