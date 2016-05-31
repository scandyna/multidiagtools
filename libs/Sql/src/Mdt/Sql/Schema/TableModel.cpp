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
#include "TableModel.h"
#include "FieldTypeName.h"
#include <QStringBuilder>

namespace Mdt{ namespace Sql{ namespace Schema{

TableModel::TableModel(QObject* parent)
 : QAbstractTableModel(parent)
{
}

void TableModel::setTable(const Table& table)
{
  beginResetModel();
  pvTable = table;
  endResetModel();
}

int TableModel::rowCount(const QModelIndex & parent) const
{
  // Check parent validity (case of use with a tree view)
  if(parent.isValid()){
    return 0;
  }
  return pvTable.fieldCount();
}

int TableModel::columnCount(const QModelIndex & /*parent*/) const
{
  return 2;
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(orientation != Qt::Horizontal){
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  if(role != Qt::DisplayRole){
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  switch(static_cast<ColumnIndex>(section)){
    case FieldNameColumn:
      return tr("Field name");
    case FieldTypeColumn:
      return tr("Field type");
  }

  return section;
}

QVariant TableModel::data(const QModelIndex & index, int role) const
{
  if(!index.isValid()){
    return QVariant();
  }
  if( (role != Qt::DisplayRole) && (role != Qt::EditRole) ){
    return QVariant();
  }
  int row = index.row();
  Q_ASSERT( (row >= 0) && (row < pvTable.fieldCount()) );
  switch(index.column()){
    case FieldNameColumn:
      return pvTable.fieldName(row);
    case FieldTypeColumn:
      return fieldTypeName(row);
  }
  return QVariant();
}

QString TableModel::fieldTypeName(int row) const
{
  QString name;
  int length = pvTable.fieldLength(row);

  if(length >= 0){
    name = FieldTypeName::nameFromType(pvTable.fieldType(row)) % QStringLiteral("(") %  QString::number(length) % QStringLiteral(")");
  }else{
    name = FieldTypeName::nameFromType(pvTable.fieldType(row));
  }

  return name;
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{