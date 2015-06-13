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
#include "mdtClConnectorData.h"
#include <QString>

//#include <QDebug>

void mdtClConnectorData::setKeyData(const mdtClConnectorKeyData & key)
{
  Q_ASSERT(!key.isNull());

  // Update contacts with new ID
  for(auto & cd : pvContactDataList){
    cd.setConnectorFk(key);
  }
  pvKeyData = key;
}

void mdtClConnectorData::clear()
{
  pvKeyData.clear();
  gender.clear();
  form.clear();
  manufacturer.clear();
  manufacturerConfigCode.clear();
  manufacturerArticleCode.clear();
  pvContactDataList.clear();
}

void mdtClConnectorData::addContactData(mdtClConnectorContactData data)
{
  if(!pvKeyData.isNull()){
    data.setConnectorFk(pvKeyData);
  }
  pvContactDataList.append(data);
}

void mdtClConnectorData::setContactDataList(const QList< mdtClConnectorContactData > &dataList)
{
  for(const auto & data : dataList){
    addContactData(data);
  }
}
