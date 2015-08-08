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
#include "mdtSqlFieldSetupTableModel.h"
#include <QComboBox>

//#include <QDebug>

mdtSqlFieldSetupTableModel::mdtSqlFieldSetupTableModel(QObject *parent)
 : QAbstractTableModel(parent)
{
}

void mdtSqlFieldSetupTableModel::setFieldList(const QList<QSqlField> & fields)
{
  pvFields = fields;
}

QSqlField mdtSqlFieldSetupTableModel::field(int row) const
{
  if( (row < 0) || (row >= pvFields.size()) ){
    return QSqlField();
  }
  return pvFields.at(row);
}

QSqlField mdtSqlFieldSetupTableModel::currentField(QComboBox *cb) const
{
  Q_ASSERT(cb != nullptr);

  return field(cb->currentIndex());
}

int mdtSqlFieldSetupTableModel::rowCount(const QModelIndex & parent) const
{
  // Check parent validity (case of use with a tree view)
  if(parent.isValid()){
    return 0;
  }
  return pvFields.size();
}

int mdtSqlFieldSetupTableModel::columnCount(const QModelIndex & /*parent */) const
{
  return 3;
}

QVariant mdtSqlFieldSetupTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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
  }

  return section;
}

QVariant mdtSqlFieldSetupTableModel::data(const QModelIndex & index, int role) const
{
  if(!index.isValid()){
    return QVariant();
  }
  if( (role != Qt::DisplayRole) && (role != Qt::EditRole) ){
    return QVariant();
  }
  int row = index.row();
  Q_ASSERT( (row >= 0) && (row < pvFields.size()) );
  switch(index.column()){
    case NameIndex:
      return pvFields.at(row).name();
    case TypeIndex:
      return pvFields.at(row).type();
    case LengthIndex:
      return pvFields.at(row).length();
  }

  return QVariant();
}

Qt::ItemFlags mdtSqlFieldSetupTableModel::flags(const QModelIndex & index) const
{
  /// Currently, we only support editing Name
  if(index.column() == NameIndex){
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
  }
  return QAbstractTableModel::flags(index);
}

bool mdtSqlFieldSetupTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if(!index.isValid()){
    return false;
  }
  if(role != Qt::EditRole){
    return false;
  }
  Q_ASSERT(index.column() == NameIndex); /// Currently, we only support editing Name
  int row = index.row();
  Q_ASSERT( (row >= 0) && (row < pvFields.size()) );
  pvFields[row].setName(value.toString());
  // Signal that we updated data
  emit dataChanged(index, index);

  return true;
}
