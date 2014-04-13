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
#include "mdtTtTestNodeUnitData.h"
#include <QString>

mdtTtTestNodeUnitData::mdtTtTestNodeUnitData()
{
}

mdtTtTestNodeUnitData::mdtTtTestNodeUnitData(const QSqlRecord & record)
 : mdtSqlRecord(record)
{
  Q_ASSERT(contains("Unit_Id_FK_PK"));
  Q_ASSERT(contains("TestNode_Id_FK"));
  Q_ASSERT(contains("Type_Code_FK"));
  Q_ASSERT(contains("TestConnection_Id_FK"));
  Q_ASSERT(contains("Bus"));
  Q_ASSERT(contains("IoPosition"));
}

bool mdtTtTestNodeUnitData::setup(const QSqlDatabase& db, bool setupConnectionData)
{
  if(!addAllFields("TestNodeUnit_tbl", db)){
    return false;
  }
  if(!pvUnitData.addAllFields("Unit_tbl", db)){
    pvLastError = pvUnitData.lastError();
    return false;
  }
  if(setupConnectionData){
    if(!pvTestConnectionData.setup(db, false)){
      pvLastError = pvTestConnectionData.lastError();
      return false;
    }
  }

  return true;
}

void mdtTtTestNodeUnitData::clearValues()
{
  pvUnitData.clearValues();
  pvTestConnectionData.clearValues();
  mdtSqlRecord::clearValues();
}

void mdtTtTestNodeUnitData::setUnitData(const mdtSqlRecord& data)
{
  Q_ASSERT(data.contains("Id_PK"));
  Q_ASSERT(data.contains("Article_Id_FK"));
  Q_ASSERT(data.contains("SchemaPosition"));
  Q_ASSERT(data.contains("Alias"));

  setValue("Unit_Id_FK_PK", data.value("Id_PK"));
  pvUnitData = data;
}

void mdtTtTestNodeUnitData::setTestConnectionData(const mdtClUnitConnectionData& data)
{
  setValue("TestConnection_Id_FK", data.value("Id_PK"));
  pvTestConnectionData = data;
}
