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
#include "mdtSqlTableMappingModel.h"

mdtSqlTableMappingModel::mdtSqlTableMappingModel(QObject* parent)
 : QAbstractTableModel(parent)
{
}

QVariant mdtSqlTableMappingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(role != Qt::DisplayRole){
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  if(orientation != Qt::Horizontal){
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  Q_ASSERT( (section >= 0) && (section < 2) );
  if(section == 0){
    return tr("Source table");
  }
  return tr("Destination table");
}

int mdtSqlTableMappingModel::columnCount(const QModelIndex & parent) const
{
  // Check parent validity (case of use with a tree view)
  if(parent.isValid()){
    return 0;
  }
  return 2;
}

int mdtSqlTableMappingModel::rowCount(const QModelIndex & parent) const
{
  // Check parent validity (case of use with a tree view)
  if(parent.isValid()){
    return 0;
  }
  /// \todo provisoire
  return 0;
}

QVariant mdtSqlTableMappingModel::data(const QModelIndex & index, int role) const
{

}
