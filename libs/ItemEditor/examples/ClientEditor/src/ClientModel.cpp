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
#include "ClientModel.h"

using namespace Mdt::ItemModel;

ClientModel::ClientModel(QObject* parent)
 : VariantTableModel(VariantTableModelStorageRule::GroupDisplayAndEditRoleData, parent)
{
  resize(2, 3);
  populateColumn(0, {1,2});
  populateColumn(1, {"First name 1","First name 2"});
  populateColumn(2, {"Last name 1","Last name 2"});
}

QVariant ClientModel::headerData(int section, Qt::Orientation orientation, int role) const
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
      return "First name";
    case 2:
      return "Last name";
  }
  return VariantTableModel::headerData(section, orientation, role);
}
