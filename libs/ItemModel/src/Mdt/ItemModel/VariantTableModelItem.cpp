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
#include "VariantTableModelItem.h"
#include <Qt>

// #include <QDebug>

namespace Mdt{ namespace ItemModel{

void VariantTableModelItem::setDisplayRoleData(const QVariant& data)
{
  mDisplayRoleData = data;
}

void VariantTableModelItem::setEditRoleData(const QVariant& data)
{
  if( isEditRoleDataSeparate() ){
    mEditRoleData = data;
  }else{
    mDisplayRoleData = data;
  }
}

void VariantTableModelItem::setData(const QVariant& data, int role)
{
  Q_ASSERT( (role == Qt::DisplayRole) || (role == Qt::EditRole) );

  if(role == Qt::DisplayRole){
    setDisplayRoleData(data);
  }else{
    setEditRoleData(data);
  }
}

Qt::ItemFlags VariantTableModelItem::flags(Qt::ItemFlags currentFlags) const
{
  if(mIsEnabled){
    currentFlags |= Qt::ItemIsEnabled;
  }else{
    currentFlags &= Qt::ItemFlags(~Qt::ItemIsEnabled);
  }
  if(mIsEditable){
    currentFlags |= Qt::ItemIsEditable;
  }else{
    currentFlags &= Qt::ItemFlags(~Qt::ItemIsEditable);
  }
  return currentFlags;
}

}} // namespace Mdt{ namespace ItemModel{