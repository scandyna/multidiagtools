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
#include "EditVehicleTypeTableModel.h"

namespace Mdt{ namespace Railway{ namespace TaskList{

EditVehicleTypeTableModel::EditVehicleTypeTableModel(QObject *parent)
 : BaseClass(parent)
{
}

int EditVehicleTypeTableModel::columnCount(const QModelIndex & /* parent*/) const
{
  return 3;
}

QVariant EditVehicleTypeTableModel::horizontalHeaderDisplayRoleData(int column) const
{
  if(column == nameColumn()){
    return tr("Name");
  }
  if(column == aliasColumn()){
    return tr("Alias");
  }
  if(column == manufacturerSerieColumn()){
    return tr("Serie");
  }
  return QVariant();
}

bool EditVehicleTypeTableModel::fetchRecords(int count)
{
}

}}} // namespace Mdt{ namespace Railway{ namespace TaskList{
