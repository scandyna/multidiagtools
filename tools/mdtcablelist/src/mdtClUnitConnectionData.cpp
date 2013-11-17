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

bool mdtClUnitConnectionData::isValid()
{
  if(connectionId().isNull()){
    return false;
  }
  if(pvUintId.isNull()){
    return false;
  }

  return true;
}

void mdtClUnitConnectionData::setUnitId(const QVariant & id)
{
  pvUintId = id;
}

QVariant mdtClUnitConnectionData::unitId() const
{
  return pvUintId;
}

void mdtClUnitConnectionData::setSchemaPage(const QVariant & schemaPage)
{
  pvSchemaPage = schemaPage;
}

QVariant mdtClUnitConnectionData::schemaPage() const
{
  return pvSchemaPage;
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

void mdtClUnitConnectionData::clearSub()
{
  pvUintId.clear();
  pvSchemaPage.clear();
  pvSignalName.clear();
  pvSwAddress.clear();
  pvArticleConnectionData.clear();
}

void mdtClUnitConnectionData::setArticleConnectionData(const mdtClArticleConnectionData & data)
{
  pvArticleConnectionData = data;
}

void mdtClUnitConnectionData::copyArticleConnectionAttributes()
{
  setConnectorName(pvArticleConnectionData.connectorName());
  setContactName(pvArticleConnectionData.contactName());
  setFunctionEN(pvArticleConnectionData.functionEN());
  setFunctionFR(pvArticleConnectionData.functionFR());
  setFunctionDE(pvArticleConnectionData.functionDE());
  setFunctionIT(pvArticleConnectionData.functionIT());
}

const mdtClArticleConnectionData & mdtClUnitConnectionData::articleConnectionData() const
{
  return pvArticleConnectionData;
}

mdtClArticleConnectionData & mdtClUnitConnectionData::articleConnectionData()
{
  return pvArticleConnectionData;
}
