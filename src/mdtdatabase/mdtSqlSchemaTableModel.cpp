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
#include "mdtSqlSchemaTableModel.h"
#include <QComboBox>

//#include <QDebug>

mdtSqlSchemaTableModel::mdtSqlSchemaTableModel(QObject *parent)
 : QAbstractTableModel(parent)
{
}

void mdtSqlSchemaTableModel::setTableSchema(const mdtSqlSchemaTable &st)
{
  beginResetModel();
  pvSchema = st;
  endResetModel();
}

// void mdtSqlSchemaTableModel::setFieldList(const QList<QSqlField> & fields)
// {
//   beginResetModel();
//   pvFields = fields;
//   endResetModel();
// }

// void mdtSqlSchemaTableModel::setPrimaryKey(const QSqlIndex & pk)
// {
//   pvPrimaryKey = pk;
// }

QSqlField mdtSqlSchemaTableModel::field(int row) const
{
  if( (row < 0) || (row >= pvSchema.fieldCount()) ){
    return QSqlField();
  }
  return pvSchema.fieldList().at(row);
//   if( (row < 0) || (row >= pvFields.size()) ){
//     return QSqlField();
//   }
//   return pvFields.at(row);
}

QSqlField mdtSqlSchemaTableModel::currentField(QComboBox *cb) const
{
  Q_ASSERT(cb != nullptr);

  return field(cb->currentIndex());
}

bool mdtSqlSchemaTableModel::isPartOfPrimaryKey(int row) const
{
  return pvSchema.primaryKey().contains(field(row).name());
//   return pvPrimaryKey.contains(field(row).name());
}

int mdtSqlSchemaTableModel::rowCount(const QModelIndex & parent) const
{
  // Check parent validity (case of use with a tree view)
  if(parent.isValid()){
    return 0;
  }
  return pvSchema.fieldCount();
//   return pvFields.size();
}

int mdtSqlSchemaTableModel::columnCount(const QModelIndex & /*parent */) const
{
  return 4;
}

QVariant mdtSqlSchemaTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(orientation != Qt::Horizontal){
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  if(role != Qt::DisplayRole){
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  switch(static_cast<ColumnIndex_t>(section)){
    case NameIndex:
      return tr("Name");
    case TypeIndex:
      return tr("Type");
    case LengthIndex:
      return tr("Length");
    case IsPkIndex:
      return tr("PK");
  }

  return section;
}

QVariant mdtSqlSchemaTableModel::data(const QModelIndex & index, int role) const
{
  if(!index.isValid()){
    return QVariant();
  }
  if( (role != Qt::DisplayRole) && (role != Qt::EditRole) ){
    return QVariant();
  }
  int row = index.row();
  Q_ASSERT( (row >= 0) && (row < pvSchema.fieldCount()) );
//   Q_ASSERT( (row >= 0) && (row < pvFields.size()) );
  switch(index.column()){
    case NameIndex:
      return pvSchema.fieldList().at(row).name();
//       return pvFields.at(row).name();
    case TypeIndex:
      return pvSchema.fieldList().at(row).type();
//       return pvFields.at(row).type();
    case LengthIndex:
      return pvSchema.fieldList().at(row).length();
//       return pvFields.at(row).length();
    case IsPkIndex:
      return isPartOfPrimaryKey(row);
  }

  return QVariant();
}

/// Currently, edition is not supported
// Qt::ItemFlags mdtSqlSchemaTableModel::flags(const QModelIndex & index) const
// {
//   if(index.column() == NameIndex){
//     return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
//   }
//   return QAbstractTableModel::flags(index);
// }

/// Currently, edition is not supported
// bool mdtSqlSchemaTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
// {
//   if(!index.isValid()){
//     return false;
//   }
//   if(role != Qt::EditRole){
//     return false;
//   }
//   Q_ASSERT(index.column() == NameIndex); /// Currently, we only support editing Name
//   int row = index.row();
//   Q_ASSERT( (row >= 0) && (row < pvFields.size()) );
//   pvFields[row].setName(value.toString());
//   // Signal that we updated data
//   emit dataChanged(index, index);
// 
//   return true;
// }
