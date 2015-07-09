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
#include "mdtClVehicleTypeLink.h"
#include <QString>

mdtClVehicleTypeLink::mdtClVehicleTypeLink(QObject *parent, QSqlDatabase db)
 : mdtClLink(parent, db)
{
}

mdtClVehicleTypeLink::mdtClVehicleTypeLink(QSqlDatabase db)
 : mdtClLink(db)
{
}

QList<mdtClVehicleTypeLinkKeyData> mdtClVehicleTypeLink::getVehicleTypeLinkKeyDataList(const mdtClLinkPkData & pk, bool & ok)
{
  QList<mdtClVehicleTypeLinkKeyData> keyList;
  QList<QSqlRecord> recordList;
  QString sql;

  sql = "SELECT * FROM VehicleType_Link_tbl " \
        " WHERE UnitConnectionStart_Id_FK = " + pk.connectionStartId.toString() + \
        " AND UnitConnectionEnd_Id_FK = " + pk.connectionEndId.toString();
  recordList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return keyList;
  }
  for(const auto & record : recordList){
    mdtClVehicleTypeLinkKeyData key;
    fillData(key, record);
  }

  return keyList;
}

void mdtClVehicleTypeLink::fillData(mdtClVehicleTypeLinkKeyData & key, const QSqlRecord & record)
{
  Q_ASSERT(record.contains("VehicleTypeStart_Id_FK"));
  Q_ASSERT(record.contains("VehicleTypeEnd_Id_FK"));
  Q_ASSERT(record.contains("UnitConnectionStart_Id_FK"));
  Q_ASSERT(record.contains("UnitConnectionEnd_Id_FK"));

  mdtClLinkPkData linkFk;

  key.setVehicleTypeStartId(record.value("VehicleTypeStart_Id_FK"));
  key.setVehicleTypeEndId(record.value("VehicleTypeEnd_Id_FK"));
  linkFk.connectionStartId = record.value("UnitConnectionStart_Id_FK");
  linkFk.connectionEndId = record.value("UnitConnectionEnd_Id_FK");
  key.setLinkFk(linkFk);
}
