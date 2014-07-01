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
#include "mdtTtTestData.h"

mdtTtTestData::mdtTtTestData()
 : mdtSqlRecord()
{
}

mdtTtTestData::mdtTtTestData(const QSqlRecord & record)
 : mdtSqlRecord(record)
{
  Q_ASSERT(contains("Id_PK"));
  Q_ASSERT(contains("TestModel_Id_FK"));
  Q_ASSERT(contains("Date"));
  Q_ASSERT(contains("DutSerialNumber"));
}

bool mdtTtTestData::setup(const QSqlDatabase & db, bool setupModelData)
{
  if(setupModelData){
    if(!pvModelData.setup(db)){
      return false;
    }
  }
  return addAllFields("Test_tbl", db);
}

void mdtTtTestData::clearValues()
{
  pvModelData.clearValues();
  mdtSqlRecord::clearValues();
}

void mdtTtTestData::setModelData(const mdtTtTestModelData & data)
{
  Q_ASSERT(contains("TestModel_Id_FK"));
  Q_ASSERT(data.contains("Id_PK"));

  setValue("TestModel_Id_FK", data.value("Id_PK"));
  pvModelData = data;
}
