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

bool mdtClLinkBeam::removeStartUnit(const QVariant& unitId, const QVariant& linkBeamId, bool handleTransactions) 
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
