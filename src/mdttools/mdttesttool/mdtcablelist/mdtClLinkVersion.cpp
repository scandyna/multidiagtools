/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "mdtClLinkVersion.h"
#include "mdtSqlTransaction.h"

mdtClLinkVersionData mdtClLinkVersion::pvCurrentVersionData;

mdtClLinkVersion::mdtClLinkVersion(QObject *parent, QSqlDatabase db)
 : mdtTtBase(parent, db)
{
}

mdtClLinkVersion::mdtClLinkVersion(QSqlDatabase db)
 : mdtTtBase(nullptr, db)
{
}

bool mdtClLinkVersion::setCurrentVersion(double version)
{
  Q_ASSERT(version > 0.0);

  mdtClLinkVersionData data;
  bool ok;

  pvCurrentVersionData.clear();
  data.setVersion(version);
  data = getVersionData(data.pk(), ok);
  if(!ok){
    return false;
  }
  if(data.isNull()){
    QString msg = tr("Could not find requested version");
    msg += " '" + QString::number(version) + "' ";
    msg += tr("in LinkVersion_tbl.");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClLinkVersion");
    pvLastError.commit();
    return false;
  }
  pvCurrentVersionData = data;

  return true;
}

bool mdtClLinkVersion::setLastVersionAsCurrentVersion()
{
  bool ok;
  mdtClLinkVersionData data;

  pvCurrentVersionData.clear();
  data = getLastVersionData(ok);
  if(!ok){
    return false;
  }
  pvCurrentVersionData = data;

  return true;
}

bool mdtClLinkVersion::addVersion(const mdtClLinkVersionData &data)
{
  mdtSqlRecord record;

  if(!record.addAllFields("LinkVersion_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  record.setValue("Version_PK", data.pk().version());
  record.setValue("Version", data.version());

  return addRecord(record, "LinkVersion_tbl");
}

mdtClLinkVersionData mdtClLinkVersion::getVersionData(LinkVersionPk key, bool & ok)
{
  mdtClLinkVersionData data;
  QList<QSqlRecord> dataList;
  QString sql;

  sql = "SELECT * FROM LinkVersion_tbl WHERE Version_PK = " + QString::number(key.version());
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return data;
  }
  if(dataList.isEmpty()){
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  data.setVersionPk(dataList.at(0).value("Version_PK"));

  return data;
}

mdtClLinkVersionData mdtClLinkVersion::getLastVersionData(bool & ok)
{
  mdtClLinkVersionData data;
  QList<QSqlRecord> dataList;
  QString sql;

  sql = "SELECT * FROM LinkVersion_tbl WHERE Version_PK = (SELECT max(Version_PK) FROM LinkVersion_tbl)";
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return data;
  }
  if(dataList.isEmpty()){
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  data.setVersionPk(dataList.at(0).value("Version_PK"));

  return data;
}

bool mdtClLinkVersion::removeVersion(LinkVersionPk key)
{
  if(!removeData("LinkVersion_tbl", "Version_PK", key.version())){
    return false;
  }
  if(key == pvCurrentVersionData.pk()){
    pvCurrentVersionData.clear();
  }
//   if(key.versionPk.value() == pvCurrentVersionData.versionPk()){
//     pvCurrentVersionData.clear();
//   }

  return true;
}

void mdtClLinkVersion::createVersion(QWidget *parentWidget)
{

}
