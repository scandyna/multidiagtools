/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "IndexListModel.h"

namespace Mdt{ namespace Sql{ namespace Schema{

IndexListModel::IndexListModel(QObject* parent)
 : QAbstractTableModel(parent)
{
}

void IndexListModel::setIndexList(const IndexList & list)
{
  beginResetModel();
  pvIndexList = list;
  endResetModel();
}

int IndexListModel::columnCount(const QModelIndex & /*parent*/) const
{
  return 4;
}

int IndexListModel::rowCount(const QModelIndex& parent) const
{
  // Check parent validity (case of use with a tree view)
  if(parent.isValid()){
    return 0;
  }
  return pvIndexList.size();
}

QVariant IndexListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(orientation != Qt::Horizontal){
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  if(role != Qt::DisplayRole){
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  switch(static_cast<ColumnIndex>(section)){
    case IndexNameColumn:
      return tr("Index");
    case TableNameColumn:
      return tr("Table");
    case FieldNameListColumn:
      return tr("Fields");
    case IsUniqueColumn:
      return tr("Unique");
  }

  return section;
}

QVariant IndexListModel::data(const QModelIndex& index, int role) const
{
  if(!index.isValid()){
    return QVariant();
  }
  if( (role != Qt::DisplayRole) && (role != Qt::EditRole) ){
    return QVariant();
  }
  int row = index.row();
  Q_ASSERT( (row >= 0) && (row < pvIndexList.size()) );
  switch( static_cast<ColumnIndex>(index.column()) ){
    case IndexNameColumn:
      return pvIndexList.at(row).name();
    case TableNameColumn:
      return pvIndexList.at(row).tableName();
    case FieldNameListColumn:
      return pvIndexList.at(row).fieldNameList().join(QStringLiteral(", "));
    case IsUniqueColumn:
      return pvIndexList.at(row).isUnique();
  }

  return QVariant();
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{
