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
#include "mdtClVehicleTypeLinkAssignationWidgetItem.h"
#include <QString>

mdtClVehicleTypeLinkAssignationWidgetItem::mdtClVehicleTypeLinkAssignationWidgetItem(QWidget *parent, const QSqlRecord &vehicleTypeData, const mdtClLinkPkData & linkFk)
 : QCheckBox(parent)
{
  Q_ASSERT(vehicleTypeData.contains("VehicleType_Id_FK"));
  Q_ASSERT(vehicleTypeData.contains("Type"));
  Q_ASSERT(vehicleTypeData.contains("SubType"));
  Q_ASSERT(vehicleTypeData.contains("SeriesNumber"));
  Q_ASSERT(!vehicleTypeData.value("VehicleType_Id_FK").isNull());
  Q_ASSERT(!linkFk.isNull());

  QString type = vehicleTypeData.value("Type").toString().trimmed();
  QString subType = vehicleTypeData.value("SubType").toString().trimmed();
  QString serie = vehicleTypeData.value("SeriesNumber").toString().trimmed();
  QString str;

  // Store keys
  pvKeyData.setVehicleTypeStartId(vehicleTypeData.value("VehicleType_Id_FK"));
  pvKeyData.setVehicleTypeEndId(vehicleTypeData.value("VehicleType_Id_FK"));
  pvKeyData.setLinkFk(linkFk);
  // Set label text
  str = type;
  if(!subType.isEmpty()){
    str += ", " + subType;
  }
  if(!serie.isEmpty()){
    str += ", serie: " + serie;
  }
  setText(str);
}

void mdtClVehicleTypeLinkAssignationWidgetItem::setCheckedIfMatches(const mdtClVehicleTypeLinkKeyData &vtlKey)
{
  if( (vtlKey.vehicleTypeStartId() == pvKeyData.vehicleTypeStartId()) && (vtlKey.vehicleTypeEndId() == pvKeyData.vehicleTypeEndId()) \
       && (vtlKey.linkFk().connectionStartId == pvKeyData.linkFk().connectionStartId) && (vtlKey.linkFk().connectionEndId == pvKeyData.linkFk().connectionEndId) ){
    setChecked(true);
  }else{
    setChecked(false);
  }
}
