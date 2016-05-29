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
#include "FieldTypeListModel.h"
#include "FieldTypeName.h"

namespace Mdt{ namespace Sql{ namespace Schema{

FieldTypeListModel::FieldTypeListModel(QObject* parent)
 : QAbstractListModel(parent)
{
}

void FieldTypeListModel::setFieldTypeList(const FieldTypeList& ftl)
{
  beginResetModel();
  pvFieldTypeList = ftl;
  endResetModel();
}

QVariant FieldTypeListModel::data(const QModelIndex & index, int role) const
{
  if(role != Qt::DisplayRole){
    return QVariant();
  }
  if(!index.isValid()){
    return QVariant();
  }
  Q_ASSERT(index.row() >= 0);
  Q_ASSERT(index.row() < pvFieldTypeList.size());

  return FieldTypeName::nameFromType( pvFieldTypeList.at(index.row()) );
}

FieldType FieldTypeListModel::fieldType(int row)
{
  if( (row < 0) || (row >= pvFieldTypeList.size()) ){
    return FieldType::UnknownType;
  }
  return pvFieldTypeList.at(row);
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{
