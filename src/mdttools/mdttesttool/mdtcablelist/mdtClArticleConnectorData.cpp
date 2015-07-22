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
#include "mdtClArticleConnectorData.h"
#include <QString>

//#include <QDebug>

void mdtClArticleConnectorData::setId(const QVariant &id)
{
  pvKeyData.id = id;
  for(auto & articleConnectionData : pvConnectionDataList){
    articleConnectionData.setArticleConnectorFk(pvKeyData);
  }
}

void mdtClArticleConnectorData::clear()
{
  pvKeyData.clear();
  name.clear();
  pvConnectionDataList.clear();
}

void mdtClArticleConnectorData::addConnectionData(mdtClArticleConnectionData data)
{
  Q_ASSERT(!pvKeyData.articleId().isNull());

  mdtClArticleConnectionKeyData key;

  key.id = data.keyData().id;
  key.setConnectionType(data.connectionType());
  key.setArticleId(pvKeyData.articleId());
  key.setArticleConnectorFk(pvKeyData);
  data.setKeyData(key);
  pvConnectionDataList.append(data);
}

void mdtClArticleConnectorData::setConnectionDataList(const QList<mdtClArticleConnectionData> & dataList)
{
  pvConnectionDataList.clear();
  for(const auto & data : dataList){
    addConnectionData(data);
  }
}
