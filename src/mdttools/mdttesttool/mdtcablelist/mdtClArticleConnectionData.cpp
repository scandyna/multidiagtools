/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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

mdtClArticleConnectionData::mdtClArticleConnectionData()
 : mdtSqlRecord()
{
}

mdtClArticleConnectionData::mdtClArticleConnectionData(const QSqlRecord & record)
 : mdtSqlRecord(record)
{
  Q_ASSERT(indexOf("Id_PK") >= 0);
  Q_ASSERT(indexOf("Article_Id_FK") >= 0);
  Q_ASSERT(indexOf("ArticleConnector_Id_FK") >= 0);
  Q_ASSERT(indexOf("ArticleContactName") >= 0);
  Q_ASSERT(indexOf("IoType") >= 0);
  Q_ASSERT(indexOf("FunctionEN") >= 0);
  Q_ASSERT(indexOf("FunctionFR") >= 0);
  Q_ASSERT(indexOf("FunctionDE") >= 0);
  Q_ASSERT(indexOf("FunctionIT") >= 0);
}

bool mdtClArticleConnectionData::setup(QSqlDatabase db) 
{
  return addAllFields("ArticleConnection_tbl", db);
}
