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
#include "mdtClVehicleTypeLinkAssignationWidget.h"
#include "mdtClUnitVehicleType.h"
#include "mdtClVehicleTypeLink.h"
#include <QVBoxLayout>

mdtClVehicleTypeLinkAssignationWidget::mdtClVehicleTypeLinkAssignationWidget(QWidget *parent, QSqlDatabase db)
 : QWidget(parent),
   pvDatabase(db),
   pvLayout(new QVBoxLayout)
{
  pvLayout->addStretch(1);
  setLayout(pvLayout);
}

bool mdtClVehicleTypeLinkAssignationWidget::setUnitId(const QVariant & unitId)
{
  mdtClUnitVehicleType uvt(pvDatabase);
  QList<QSqlRecord> vehicleTypeDataList;
  bool ok;

  /// \todo clear
  
  // Get vehicle type data list
  vehicleTypeDataList = uvt.getVehicleTypeDataList(unitId, ok);
  if(!ok){
    pvLastError = uvt.lastError();
    return false;
  }
}
