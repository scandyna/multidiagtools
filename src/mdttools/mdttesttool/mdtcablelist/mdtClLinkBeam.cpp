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
#include "mdtClLinkBeam.h"
#include "mdtSqlRecord.h"
#include <QSqlRecord>

//#include <QDebug>

mdtClLinkBeam::mdtClLinkBeam(QObject *parent, QSqlDatabase db)
 : mdtTtBase(parent, db)
{
}

QString mdtClLinkBeam::sqlForStartUnitSelection(const QVariant & linkBeamId) 
{
  QString sql;

  sql = "SELECT * FROM Unit_view ";
  sql += "WHERE Unit_Id_PK NOT IN (";
  sql += "SELECT Unit_Id_FK FROM LinkBeam_UnitStart_tbl WHERE LinkBeam_Id_FK = " + linkBeamId.toString();
  sql += ")";

  return sql;
}

QString mdtClLinkBeam::sqlForEndUnitSelection(const QVariant & linkBeamId) 
{
  QString sql;

  sql = "SELECT * FROM Unit_view ";
  sql += "WHERE Unit_Id_PK NOT IN (";
  sql += "SELECT Unit_Id_FK FROM LinkBeam_UnitEnd_tbl WHERE LinkBeam_Id_FK = " + linkBeamId.toString();
  sql += ")";

  return sql;
}

QString mdtClLinkBeam::sqlForLinkSelection(const QList<QVariant> & unitStartIdList, const QList<QVariant> & unitEndIdList) const
{
  QString sql;
  int i;

  sql = "SELECT * FROM UnitLink_view";
  if((unitStartIdList.isEmpty())&&(unitEndIdList.isEmpty())){
    sql += " WHERE StartUnit_Id_FK = -1 AND EndUnit_Id_FK = -1";
    return sql;
  }
  sql += " WHERE LinkBeam_Id_FK IS NULL AND LinkType_Code_FK = 'CABLELINK'";
  // Build start unit filter
  if(unitStartIdList.size() > 0){
    sql += " AND (";
    sql += "StartUnit_Id_FK = " + unitStartIdList.at(0).toString();
    for(i = 1; i < unitStartIdList.size(); ++i){
      sql += " OR StartUnit_Id_FK = " + unitStartIdList.at(i).toString();
    }
    sql += ")";
  }
  // Build end unit filter
  if(unitEndIdList.size() > 0){
    sql += " AND (";
    sql += "EndUnit_Id_FK = " + unitEndIdList.at(0).toString();
    for(i = 1; i < unitEndIdList.size(); ++i){
      sql += " OR EndUnit_Id_FK = " + unitEndIdList.at(i).toString();
    }
    sql += ")";
  }

  return sql;
}

bool mdtClLinkBeam::addStartUnit(const QVariant & unitId, const QVariant & linkBeamId) 
{
  mdtSqlRecord record;

  if(!record.addAllFields("LinkBeam_UnitStart_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  record.setValue("Unit_Id_FK", unitId);
  record.setValue("LinkBeam_Id_FK", linkBeamId);

  return addRecord(record, "LinkBeam_UnitStart_tbl");
}

bool mdtClLinkBeam::removeStartUnit(const QVariant& unitId, const QVariant& linkBeamId) 
{
  return removeData("LinkBeam_UnitStart_tbl", "Unit_Id_FK", unitId, "LinkBeam_Id_FK", linkBeamId);
}

bool mdtClLinkBeam::removeStartUnits(const mdtSqlTableSelection & s)
{
  return removeData("LinkBeam_UnitStart_tbl", s, true);
}

bool mdtClLinkBeam::addEndUnit(const QVariant & unitId, const QVariant & linkBeamId) 
{
  mdtSqlRecord record;

  if(!record.addAllFields("LinkBeam_UnitEnd_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  record.setValue("Unit_Id_FK", unitId);
  record.setValue("LinkBeam_Id_FK", linkBeamId);

  return addRecord(record, "LinkBeam_UnitEnd_tbl");
}

bool mdtClLinkBeam::removeEndUnit(const QVariant & unitId, const QVariant & linkBeamId) 
{
  return removeData("LinkBeam_UnitEnd_tbl", "Unit_Id_FK", unitId, "LinkBeam_Id_FK", linkBeamId);
}

bool mdtClLinkBeam::removeEndUnits(const mdtSqlTableSelection & s)
{
  return removeData("LinkBeam_UnitEnd_tbl", s, true);
}

bool mdtClLinkBeam::addLink(const QVariant & unitConnectionStartId, const QVariant & unitConnectionEndId, const QVariant & linkBeamId)
{
  mdtSqlRecord matchData;
  mdtSqlRecord record;

  // Setup match data
  if(!matchData.addField("UnitConnectionStart_Id_FK", "Link_tbl", database())){
    pvLastError = matchData.lastError();
    return false;
  }
  if(!matchData.addField("UnitConnectionEnd_Id_FK", "Link_tbl", database())){
    pvLastError = matchData.lastError();
    return false;
  }
  matchData.setValue("UnitConnectionStart_Id_FK", unitConnectionStartId);
  matchData.setValue("UnitConnectionEnd_Id_FK", unitConnectionEndId);
  // Setup data record
  if(!record.addField("LinkBeam_Id_FK", "Link_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  record.setValue("LinkBeam_Id_FK", linkBeamId);

  return updateRecord("Link_tbl", record, matchData);
}

bool mdtClLinkBeam::removeLinks(const mdtSqlTableSelection & s)
{
  int i;

  if(!beginTransaction()){
    return false;
  }
  for(i = 0; i < s.rowCount(); ++i){
    if(!removeLink(s.data(i, "UnitConnectionStart_Id_FK"), s.data(i, "UnitConnectionEnd_Id_FK"))){
      rollbackTransaction();
      return false;
    }
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}

bool mdtClLinkBeam::removeLink(const QVariant & unitConnectionStartId, const QVariant & unitConnectionEndId)
{
  mdtSqlRecord matchData;
  mdtSqlRecord record;

  // Setup match data
  if(!matchData.addField("UnitConnectionStart_Id_FK", "Link_tbl", database())){
    pvLastError = matchData.lastError();
    return false;
  }
  if(!matchData.addField("UnitConnectionEnd_Id_FK", "Link_tbl", database())){
    pvLastError = matchData.lastError();
    return false;
  }
  matchData.setValue("UnitConnectionStart_Id_FK", unitConnectionStartId);
  matchData.setValue("UnitConnectionEnd_Id_FK", unitConnectionEndId);
  // Setup data record
  if(!record.addField("LinkBeam_Id_FK", "Link_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  record.setValue("LinkBeam_Id_FK", QVariant());

  return updateRecord("Link_tbl", record, matchData);
}
