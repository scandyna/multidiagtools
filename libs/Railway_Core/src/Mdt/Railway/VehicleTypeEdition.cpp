/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "VehicleTypeEdition.h"
#include "Mdt/ItemModel/SortProxyModel.h"

using namespace Mdt::Entity;
using namespace Mdt::ItemModel;

namespace Mdt{ namespace Railway{

VehicleTypeEdition::VehicleTypeEdition()
{
  /// \todo toy
  VehicleTypeClassDataList vtcl;
  VehicleTypeClassData vtbd;
  vtbd.setName( QLatin1String("RBDe 560 DO") );
  vtcl.append(vtbd);
  vtbd.setName( QLatin1String("RABDe 500") );
  vtcl.append(vtbd);
//   mVehicleTypeClassTableModel.setTable(vtcl);

  auto *sortModel = new SortProxyModel(&mVehicleTypeClassTableModel);
  sortModel->addColumnToSortOrder(vehicleTypeNameColumn(), Qt::AscendingOrder);

  mVehicleTypeClassTableModelContainer.setSourceModel(&mVehicleTypeClassTableModel);
  mVehicleTypeClassTableModelContainer.appendProxyModel(sortModel);
}

bool VehicleTypeEdition::setVehicleTypeName(const QString& name)
{
  VehicleTypeClassData vehicleTypeClass;

  if( !mFieldDataValidator.validateData(name, vehicleTypeClass.def().name()) ){
    mLastError = mFieldDataValidator.toGenericError();
    return false;
  }
  vehicleTypeClass.setName(name);
  
//   mVehicleTypeClassCurrentRow = mVehicleTypeClassTableModel.rowOfVehicleTypeName( vehicleTypeClass.name() );
  if(mVehicleTypeClassCurrentRow < 0){
    mVehicleTypeClassCurrentRow = mVehicleTypeClassTableModel.rowCount();
   /// mVehicleTypeClassTableModel.appendRow(vehicleTypeClass);
  }
//   if(!mVehicleTypeClassTableModel.containsVehicleTypeName(name)){
// //     VehicleTypeClassData vtc;
// //     vtc.setName(name);
//     mVehicleTypeClassTableModel.appendRow(vehicleTypeClass);
//   }

  return true;
}

FieldAttributes VehicleTypeEdition::vehicleTypeNameFieldAttributes() const
{
  return mVehicleTypeClassTableModel.def().name().fieldAttributes();
//   return mCurrentVehicleTypeClassData.def().name().fieldAttributes();
}

bool VehicleTypeEdition::setManufacturerSerie(const QString& serie)
{
  if( !mFieldDataValidator.validateData(serie, mVehicleTypeData.def().manufacturerSerie()) ){
    mLastError = mFieldDataValidator.toGenericError();
    return false;
  }
  mVehicleTypeData.setManufacturerSerie(serie);

  return true;
}

Mdt::Entity::FieldAttributes VehicleTypeEdition::manufacturerSerieFieldAttributes() const
{
  return mVehicleTypeData.def().manufacturerSerie().fieldAttributes();
}

}} // namespace Mdt{ namespace Railway{
