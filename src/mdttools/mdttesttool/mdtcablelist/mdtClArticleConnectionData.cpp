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
#include "mdtClArticleConnectionData.h"

//#include <QDebug>

void mdtClArticleConnectionData::setKeyData(const mdtClArticleConnectionKeyData &key)
{
  Q_ASSERT(!key.articleId().isNull());
  Q_ASSERT( (key.articleConnectorFk().isNull()) || (key.articleConnectorFk().articleId() == key.articleId()) );
  pvKeyData = key;
}

void mdtClArticleConnectionData::setArticleConnectorFk(const mdtClArticleConnectorKeyData & fk)
{
  Q_ASSERT(!fk.articleId().isNull());
  Q_ASSERT(fk.articleId() == pvKeyData.articleId());
  pvKeyData.setArticleConnectorFk(fk);
}

void mdtClArticleConnectionData::clear()
{
  pvKeyData.clear();
  name.clear();
  resistance.clear();
  ioType.clear();
  functionEN.clear();
  functionFR.clear();
  functionDE.clear();
  functionIT.clear();
}
