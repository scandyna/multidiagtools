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
#include "mdtClVehicleTypeLinkTestData.h"
#include "mdtClVehicleTypeLink.h"
#include "mdtTest.h"
#include "mdtTtBase.h"
#include "mdtSqlTransaction.h"
#include "mdtSqlRecord.h"
#include <QSqlRecord>
#include <QDebug>

void mdtClVehicleTypeLinkTestData::clear()
{
  removeCreatedVehicleTypeLinkAssignations();
  removeCreatedVehicleTypeUnitAssignations();
  removeCreatedVehicleTypes();
}

bool mdtClVehicleTypeLinkTestData::createVehicleType(int id, const QString &type, const QString &subType, const QString &seriesNumber)
{
  mdtTtBase tb(nullptr, pvDatabase);
  mdtSqlRecord record;

  if(!record.addAllFields("VehicleType_tbl", pvDatabase)){
    return false;
  }
  record.setValue("Id_PK", id);
  record.setValue("Type", type);
  record.setValue("SubType", subType);
  record.setValue("SeriesNumber", seriesNumber);
  if(!tb.addRecord(record, "VehicleType_tbl")){
    return false;
  }
  pvCreatedVehicleTypeIdList.append(id);

  return true;
}

void mdtClVehicleTypeLinkTestData::removeCreatedVehicleTypes()
{
  mdtTtBase tb(nullptr, pvDatabase);
  mdtSqlTransaction transaction(pvDatabase);

  QVERIFY(transaction.begin());
  for(const auto & id : pvCreatedVehicleTypeIdList){
    QVERIFY(tb.removeData("VehicleType_tbl", "Id_PK", id));
  }
  QVERIFY(transaction.commit());
}

bool mdtClVehicleTypeLinkTestData::addVehicleTypeUnitAssignation(int vtId, int unitId)
{
  mdtTtBase tb(nullptr, pvDatabase);
  mdtSqlRecord record;
  vtuData vtu{vtId, unitId};

  if(!record.addAllFields("VehicleType_Unit_tbl", pvDatabase)){
    return false;
  }
  record.setValue("Unit_Id_FK", unitId);
  record.setValue("VehicleType_Id_FK", vtId);
  if(!tb.addRecord(record, "VehicleType_Unit_tbl")){
    return false;
  }
  pvCreatedVtuList.append(vtu);

  return true;
}

void mdtClVehicleTypeLinkTestData::removeCreatedVehicleTypeUnitAssignations()
{
  mdtTtBase tb(nullptr, pvDatabase);
  mdtSqlTransaction transaction(pvDatabase);

  QVERIFY(transaction.begin());
  for(const auto & vtu : pvCreatedVtuList){
    QVERIFY(tb.removeData("VehicleType_Unit_tbl", "Unit_Id_FK", vtu.unitId, "VehicleType_Id_FK", vtu.vtId));
  }
  QVERIFY(transaction.commit());
}

bool mdtClVehicleTypeLinkTestData::addVehicleTypeLinkAssignation(int vtId, int ucsId, int uceId, int versionPk, const QString & modificationCode)
{
  mdtClVehicleTypeLink vtl(pvDatabase);
  ///mdtTtBase tb(nullptr, pvDatabase);
  ///mdtSqlRecord record;
  mdtClVehicleTypeLinkKeyData vtlKey;
  mdtClLinkPkData linkFk;

  linkFk.connectionStartId = ucsId;
  linkFk.connectionEndId = uceId;
  linkFk.versionFk.versionPk = versionPk;
  linkFk.modificationFk.code = modificationCode;
  vtlKey.setVehicleTypeStartId(vtId);
  vtlKey.setVehicleTypeEndId(vtId);
  vtlKey.setLinkFk(linkFk);
  if(!vtl.addVehicleTypeLink(vtlKey)){
    return false;
  }

//   if(!record.addAllFields("VehicleType_Link_tbl", pvDatabase)){
//     return false;
//   }
//   record.setValue("VehicleTypeStart_Id_FK", vtId);
//   record.setValue("VehicleTypeEnd_Id_FK", vtId);
//   record.setValue("UnitConnectionStart_Id_FK", ucsId);
//   record.setValue("UnitConnectionEnd_Id_FK", uceId);
//   if(!tb.addRecord(record, "VehicleType_Link_tbl")){
//     return false;
//   }
  pvCreatedVtlList.append(vtlKey);

  return true;
}

void mdtClVehicleTypeLinkTestData::removeCreatedVehicleTypeLinkAssignations()
{
  mdtClVehicleTypeLink vtl(pvDatabase);
  mdtSqlTransaction transaction(pvDatabase);

  QVERIFY(transaction.begin());
  for(const auto & key : pvCreatedVtlList){
    QVERIFY(vtl.removeVehicleTypeLink(key));
  }
  QVERIFY(transaction.commit());
}

void mdtClVehicleTypeLinkTestData::debugSqlError(const QSqlError &error)
{
  qDebug() << error;
}

void mdtClVehicleTypeLinkTestData::debugSqlError(const QSqlQuery &query)
{
  qDebug() << "Error on query '" << query.lastQuery() << "': " << query.lastError();
}
