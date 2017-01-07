/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "AddressModel.h"

using namespace Mdt::ItemModel;

AddressModel::AddressModel(QObject* parent)
 : VariantTableModel(VariantTableModelStorageRule::GroupDisplayAndEditRoleData, parent)
{
  resize(2, 3);
  populateColumn(0, {11,21});
  populateColumn(1, {1, 2});
  populateColumn(2, {"Street 11","Street 21"});
}

QVariant AddressModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(role != Qt::DisplayRole){
    return VariantTableModel::headerData(section, orientation, role);
  }
  if(orientation != Qt::Horizontal){
    return VariantTableModel::headerData(section, orientation, role);
  }
  switch(section){
    case 0:
      return "Id";
    case 1:
      return "ClientId";
    case 2:
      return "Street";
  }
  return VariantTableModel::headerData(section, orientation, role);
}
