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
#include "mdtClUnitConnectionData.h"

mdtClUnitConnectionData::mdtClUnitConnectionData()
{
}

mdtClUnitConnectionData::~mdtClUnitConnectionData()
{
}

void mdtClUnitConnectionData::clear()
{
  pvId.clear();
  pvUintId.clear();
  pvArticleConnectionId.clear();
  pvSchemaPage.clear();
  pvFunctionEN.clear();
  pvFunctionFR.clear();
  pvFunctionDE.clear();
  pvFunctionIT.clear();
  pvSignalName.clear();
  pvSwAddress.clear();
  pvUnitConnectorName.clear();
  pvUnitContactName.clear();
}

bool mdtClUnitConnectionData::isValid()
{
  if(pvId.isNull()){
    return false;
  }
  if(pvUintId.isNull()){
    return false;
  }

  return true;
}

void mdtClUnitConnectionData::setId(const QVariant & id)
{
  pvId = id;
}

QVariant mdtClUnitConnectionData::id()
{
  return pvId;
}

void mdtClUnitConnectionData::setUnitId(const QVariant & id)
{
  pvUintId = id;
}

QVariant mdtClUnitConnectionData::unitId() const
{
  return pvUintId;
}

void mdtClUnitConnectionData::setArticleConnectionId(const QVariant & id)
{
  pvArticleConnectionId = id;
}

QVariant mdtClUnitConnectionData::articleConnectionId() const
{
  return pvArticleConnectionId;
}

void mdtClUnitConnectionData::setSchemaPage(const QVariant & schemaPage)
{
  pvSchemaPage = schemaPage;
}

QVariant mdtClUnitConnectionData::schemaPage() const
{
  return pvSchemaPage;
}

void mdtClUnitConnectionData::setFunctionEN(const QVariant & function)
{
  pvFunctionEN = function;
}

QVariant mdtClUnitConnectionData::functionEN() const
{
  return pvFunctionEN;
}

void mdtClUnitConnectionData::setFunctionFR(const QVariant & function)
{
  pvFunctionFR = function;
}

QVariant mdtClUnitConnectionData::functionFR() const
{
  return pvFunctionFR;
}

void mdtClUnitConnectionData::setFunctionDE(const QVariant & function)
{
  pvFunctionDE = function;
}

QVariant mdtClUnitConnectionData::functionDE() const
{
  return pvFunctionDE;
}

void mdtClUnitConnectionData::setFunctionIT(const QVariant & function)
{
  pvFunctionIT = function;
}

QVariant mdtClUnitConnectionData::functionIT() const
{
  return pvFunctionIT;
}

void mdtClUnitConnectionData::setSignalName(const QVariant & name)
{
  pvSignalName = name;
}

QVariant mdtClUnitConnectionData::signalName() const
{
  return pvSignalName;
}

void mdtClUnitConnectionData::setSwAddress(const QVariant & address)
{
  pvSwAddress = address;
}

QVariant mdtClUnitConnectionData::swAddress() const
{
  return pvSwAddress;
}

void mdtClUnitConnectionData::setUnitConnectorName(const QVariant & name)
{
  pvUnitConnectorName = name;
}

QVariant mdtClUnitConnectionData::unitConnectorName() const
{
  return pvUnitConnectorName;
}

void mdtClUnitConnectionData::setUnitContactName(const QVariant & name)
{
  pvUnitContactName = name;
}

QVariant mdtClUnitConnectionData::unitContactName() const
{
  return pvUnitContactName;
}
