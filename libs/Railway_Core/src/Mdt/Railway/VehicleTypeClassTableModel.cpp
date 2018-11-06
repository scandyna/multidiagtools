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
#include "VehicleTypeClassTableModel.h"
#include "EditVehicleTypeClass.h"

namespace Mdt{ namespace Railway{

// int VehicleTypeClassTableModel::rowOfVehicleTypeName(const QString& name) const
// {
//   return table().rowOfVehicleTypeName(name);
// }

// bool VehicleTypeClassTableModel::containsVehicleTypeName(const QString& name) const
// {
//   return table().containsVehicleTypeName(name);
// }

VehicleTypeClassTableModel::VehicleTypeClassTableModel(QObject* parent)
 : CachedEntityRepositoryTableModel(parent)
{
  setModelEditable(true);
}

int VehicleTypeClassTableModel::nameColumn() const
{
  return fieldIndex(def().name());
}

int VehicleTypeClassTableModel::aliasColumn() const
{
  return fieldIndex(def().alias());
}

// bool VehicleTypeClassTableModel::setEditRoleData(int row, int column, const QVariant& value)
// {
//   Q_ASSERT(row >= 0);
//   Q_ASSERT(row <= rowCount());
//   Q_ASSERT(column >= 0);
//   Q_ASSERT(column <= columnCount());
// 
//   if(column == nameColumn()){
//     return setName(row, value.toString());
//   }else if(column == aliasColumn()){
//     return setAlias(row, value.toString());
//   }
// 
//   return false;
// }

bool VehicleTypeClassTableModel::setName(int row, const QString& name)
{
  EditVehicleTypeClass edit;

  if(!edit.setName(name)){
    return false;
  }

  return ParentClass::setEditRoleData(row, nameColumn(), edit.name());
}

bool VehicleTypeClassTableModel::setAlias(int row, const QString& alias)
{
  EditVehicleTypeClass edit;

  if(!edit.setAlias(alias)){
    return false;
  }

  return ParentClass::setEditRoleData(row, aliasColumn(), edit.alias());
}

}} // namespace Mdt{ namespace Railway{
