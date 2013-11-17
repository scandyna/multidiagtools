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
#include "mdtClConnectionData.h"

void mdtClConnectionData::clear()
{
  clearSub();
  pvConnectorId.clear();
  pvConnectionId.clear();
  pvConnectorName.clear();
  pvContactName.clear();
  pvFunctionEN.clear();
  pvFunctionFR.clear();
  pvFunctionDE.clear();
  pvFunctionIT.clear();
}

void mdtClConnectionData::setConnectorId(const QVariant & id) 
{
  pvConnectorId = id;
}

QVariant mdtClConnectionData::connectorId() const 
{
  return pvConnectorId;
}

void mdtClConnectionData::setConnectionId(const QVariant & id) 
{
  pvConnectionId = id;
}

QVariant mdtClConnectionData::connectionId() const 
{
  return pvConnectionId;
}

void mdtClConnectionData::setConnectorName(const QVariant & name) 
{
  pvConnectorName = name;
}

QVariant mdtClConnectionData::connectorName() const 
{
  return pvConnectorName;
}

void mdtClConnectionData::setContactName(const QVariant & name) 
{
  pvContactName = name;
}

QVariant mdtClConnectionData::contactName() const 
{
  return pvContactName;
}

void mdtClConnectionData::setFunctionEN(const QVariant & function)
{
  pvFunctionEN = function;
}

QVariant mdtClConnectionData::functionEN() const
{
  return pvFunctionEN;
}

void mdtClConnectionData::setFunctionFR(const QVariant & function)
{
  pvFunctionFR = function;
}

QVariant mdtClConnectionData::functionFR() const
{
  return pvFunctionFR;
}

void mdtClConnectionData::setFunctionDE(const QVariant & function)
{
  pvFunctionDE = function;
}

QVariant mdtClConnectionData::functionDE() const
{
  return pvFunctionDE;
}

void mdtClConnectionData::setFunctionIT(const QVariant & function)
{
  pvFunctionIT = function;
}

QVariant mdtClConnectionData::functionIT() const
{
  return pvFunctionIT;
}
