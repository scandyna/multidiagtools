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
#include "mdtTtTestSystemComponent.h"
#include "mdtSqlRecord.h"

mdtTtTestSystemComponent::mdtTtTestSystemComponent(QObject* parent, QSqlDatabase db)
 : mdtTtBase(parent, db)
{
}

mdtTtTestSystemComponent::mdtTtTestSystemComponent(QSqlDatabase db)
 : mdtTtBase(nullptr, db)
{
}

bool mdtTtTestSystemComponent::addTestSystemAssignation(const mdtTtTestSystemAssignationData & data)
{
  mdtSqlRecord record;

  if(!record.addAllFields("TestSystem_TestSystemComponent_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  record.setValue("TestSystem_Id_FK", data.testSystemId);
  record.setValue("TestSystemComponent_Id_FK", data.testSystemComponentId);

  return addRecord(record, "TestSystem_TestSystemComponent_tbl");
}

bool mdtTtTestSystemComponent::removeTestSystemAssignation(const mdtTtTestSystemAssignationData & data)
{
  return removeData("TestSystem_TestSystemComponent_tbl", "TestSystem_Id_FK", data.testSystemId, "TestSystemComponent_Id_FK", data.testSystemComponentId);
}

bool mdtTtTestSystemComponent::removeTestSystemAssignation(const mdtSqlTableSelection& s)
{
  return removeData("TestSystem_TestSystemComponent_tbl", s, true);
}

QVariant mdtTtTestSystemComponent::createUnit(const QVariant & componentId, const QString & typeCode, const QVariant & baseAticleId)
{
  QSqlQuery query(database());
  mdtSqlRecord uRecord, tsuRecord;
  QVariant id;

  // Setup records
  if(!uRecord.addAllFields("Unit_tbl", database())){
    pvLastError = uRecord.lastError();
    return QVariant();
  }
  if(!tsuRecord.addAllFields("TestSystemUnit_tbl", database())){
    pvLastError = tsuRecord.lastError();
    return QVariant();
  }
  // Start a trasaction
  if(!beginTransaction()){
    return QVariant();
  }
  // Insert into Unit_tbl and get created ID
  ///uRecord.setValue("SchemaPosition", "");
  uRecord.setValue("Article_Id_FK", baseAticleId);
  if(!addRecord(uRecord, "Unit_tbl", query)){
    rollbackTransaction();
    return QVariant();
  }
  id = query.lastInsertId();
  if(id.isNull()){
    rollbackTransaction();
    pvLastError.setError(tr("Creating test system unit failed: created Id_PK in Unit_tbl is Null"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestSystemComponent");
    pvLastError.commit();
    return QVariant();
  }
  // Insert into TestSystemUnit_tbl
  tsuRecord.setValue("Unit_Id_FK_PK", id);
  tsuRecord.setValue("TestSystemComponent_Id_FK", componentId);
  tsuRecord.setValue("Type_Code_FK", typeCode);
  if(!addRecord(tsuRecord, "TestSystemUnit_tbl", query)){
    rollbackTransaction();
    return QVariant();
  }
  // Commit transaction
  if(!commitTransaction()){
    return QVariant();
  }

  return id;
}

bool mdtTtTestSystemComponent::removeUnit(const QVariant& id, bool handleTransaction)
{
  if(handleTransaction){
    if(!beginTransaction()){
      return false;
    }
  }
  if(!removeData("TestSystemUnit_tbl", "Unit_Id_FK_PK", id)){
    if(handleTransaction){
      rollbackTransaction();
    }
    return false;
  }
  if(!removeData("Unit_tbl", "Id_PK", id)){
    if(handleTransaction){
      rollbackTransaction();
    }
    return false;
  }
  if(handleTransaction){
    if(!commitTransaction()){
      return false;
    }
  }

  return true;
}

bool mdtTtTestSystemComponent::removeUnits(const mdtSqlTableSelection & s)
{
  auto idList = s.dataList("Unit_Id_FK_PK");

  if(idList.isEmpty()){
    return true;
  }
  if(!beginTransaction()){
    return false;
  }
  for(const auto & id : idList){
    if(!removeUnit(id, false)){
      rollbackTransaction();
      return false;
    }
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}




