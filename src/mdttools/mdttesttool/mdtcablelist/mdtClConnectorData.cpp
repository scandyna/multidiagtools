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
#include "mdtClConnectorData.h"
#include <QString>

//#include <QDebug>

mdtClConnectorData::mdtClConnectorData()
{
}

mdtClConnectorData::mdtClConnectorData(const QSqlRecord& record)
 : mdtSqlRecord(record)
{
  Q_ASSERT(contains("Id_PK"));
  /// \todo Once fields are defined, complete this part
}


bool mdtClConnectorData::setup(const QSqlDatabase & db)
{
  if(!addAllFields("Connector_tbl", db)){
    return false;
  }
  return true;
}

void mdtClConnectorData::setContactDataList(const QList<mdtSqlRecord> & dataList) 
{
  pvContactDataList = dataList;
}

QList<mdtSqlRecord> mdtClConnectorData::contactDataList() const
{
  return pvContactDataList;
}

void mdtClConnectorData::addContactData(const mdtSqlRecord & data) 
{
  Q_ASSERT(data.contains("Id_PK"));
  Q_ASSERT(data.contains("Connector_Id_FK"));

  mdtSqlRecord _data = data;
  _data.setValue("Connector_Id_FK", value("Id_PK"));
  pvContactDataList.append(_data);
}

bool mdtClConnectorData::setContactData(const QVariant & contactId, const mdtSqlRecord & data) 
{
  int i;

  for(i = 0; i < pvContactDataList.size(); ++i){
    if(pvContactDataList.at(i).value("Id_PK") == contactId){
      pvContactDataList[i] = data;
      return true;
    }
  }
  // ID was not found
  pvLastError.setError("Could not find contact data with ID " + contactId.toString(), mdtError::Error);
  MDT_ERROR_SET_SRC(pvLastError, "mdtClConnectorData");
  pvLastError.commit();

  return false;
}

mdtSqlRecord mdtClConnectorData::contactData(const QVariant & contactId, bool *ok) const
{
  Q_ASSERT(ok != 0);

  int i;

  for(i = 0; i < pvContactDataList.size(); ++i){
    if(pvContactDataList.at(i).value("Id_PK") == contactId){
      *ok = true;
      return pvContactDataList.at(i);
    }
  }
  // ID was not found
  *ok = false;

  return mdtSqlRecord();

}

