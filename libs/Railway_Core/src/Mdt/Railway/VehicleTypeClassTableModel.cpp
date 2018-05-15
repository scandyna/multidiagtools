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
#include "VehicleTypeClassEdition.h"

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

bool VehicleTypeClassTableModel::setName(const QModelIndex & index, const QString& name)
{
  Q_ASSERT(index.column() == nameColumn());

  VehicleTypeClassEdition edit;

  if(!edit.setName(name)){
    return false;
  }

  return ParentClass::setData(index, edit.name());
}

int VehicleTypeClassTableModel::aliasColumn() const
{
  return fieldIndex(def().alias());
}

bool VehicleTypeClassTableModel::setAlias(const QModelIndex& index, const QString& alias)
{
  Q_ASSERT(index.column() == aliasColumn());

  VehicleTypeClassEdition edit;

  if(!edit.setAlias(alias)){
    return false;
  }

  return ParentClass::setData(index, edit.alias());
}

bool VehicleTypeClassTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if( !index.isValid() || (role != Qt::EditRole) ){
    return ParentClass::setData(index, value, role);
  }

  const int column = index.column();
  if(column == nameColumn()){
    return setName(index, value.toString());
  }
  if(column == aliasColumn()){
    return setAlias(index, value.toString());
  }

  return false;
}

}} // namespace Mdt{ namespace Railway{
