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
#include "mdtAbstractIo.h"

mdtAbstractIo::mdtAbstractIo(QObject *parent)
 : QObject(parent)
{
  pvAddress = 0;
  pvHasValidData = false;
  ///pvType = UNKNOWN;
}

mdtAbstractIo::~mdtAbstractIo()
{
}

/**
mdtAbstractIo::type_t mdtAbstractIo::type() const
{
  return pvType;
}
*/

void mdtAbstractIo::setAddress(int adr)
{
  pvAddress = adr;
  emit(addressChangedForUi(pvAddress));
}

int mdtAbstractIo::address() const
{
  return pvAddress;
}

void mdtAbstractIo::setLabelShort(const QString &text)
{
  pvLabelShort = text;
  emit(labelShortChangedForUi(pvLabelShort));
}

QString mdtAbstractIo::labelShort() const
{
  return pvLabelShort;
}

void mdtAbstractIo::setLabel(const QString &text)
{
  pvLabel = text;
  emit(labelChangedForUi(pvLabel));
}

QString mdtAbstractIo::label() const
{
  return pvLabel;
}

void mdtAbstractIo::setDetails(const QString &text)
{
  pvDetails = text;
  emit(detailsChangedForUi(pvDetails));
}

QString mdtAbstractIo::details() const
{
  return pvDetails;
}

bool mdtAbstractIo::hasValidData() const
{
  return pvHasValidData;
}

void mdtAbstractIo::setEnabled(bool enabled)
{
  emit(enabledStateChangedForUi(enabled));
}

/**
void mdtAbstractIo::setType(mdtAbstractIo::type_t type)
{
  pvType = type;
}
*/
