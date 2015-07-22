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
#include "mdtClUnitConnectorData.h"
#include <QString>

//#include <QDebug>

void mdtClUnitConnectorData::setPk(const mdtClUnitConnectorPkData & pk)
{
  pvKeyData.pk = pk;
  for(auto & unitConnectionData : pvConnectionDataList){
    unitConnectionData.setUnitConnectorFk(pvKeyData);
  }
}

void mdtClUnitConnectorData::setId(const QVariant & id)
{
  pvKeyData.pk.id = id;
  for(auto & unitConnectionData : pvConnectionDataList){
    unitConnectionData.setUnitConnectorFk(pvKeyData);
  }
}

void mdtClUnitConnectorData::clear()
{
  pvKeyData.clear();
  name.clear();
  pvConnectionDataList.clear();
}

void mdtClUnitConnectorData::addConnectionData(mdtClUnitConnectionData data) 
{
  Q_ASSERT(!pvKeyData.unitId().isNull());
  Q_ASSERT( ( (data.keyData().pk.isNull()) && (!data.isPartOfUnitConnector()) ) ||
            ( (!data.keyData().pk.isNull()) && (data.keyData().unitConnectorFk().pk.id == pvKeyData.pk.id)
               && (data.keyData().unitConnectorFk().unitId() == pvKeyData.unitId()) ) );

  mdtClUnitConnectionKeyData key;

  key.pk = data.keyData().pk;
  key.setUnitId(pvKeyData.unitId());
  key.setUnitConnectorFk(pvKeyData);
  if(data.isBasedOnArticleConnection()){
    key.setArticleConnectionFk(data.keyData().articleConnectionFk());
  }else{
    key.setConnectionType(data.connectionType());
  }
  data.setKeyData(key);
  pvConnectionDataList.append(data);
}

void mdtClUnitConnectorData::setConnectionDataList(const QList<mdtClUnitConnectionData> & dataList) 
{
  pvConnectionDataList.clear();
  for(const auto & data : dataList){
    addConnectionData(data);
  }
}
