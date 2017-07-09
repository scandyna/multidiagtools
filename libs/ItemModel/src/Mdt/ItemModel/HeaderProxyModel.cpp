/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
 ** along with Mdt. If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "HeaderProxyModel.h"

namespace Mdt{ namespace ItemModel{

HeaderProxyModel::HeaderProxyModel(QObject* parent)
 : QIdentityProxyModel(parent)
{
}

QVariant HeaderProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(orientation == Qt::Horizontal){
    return horizontalHeaderData(section, role);
  }
  return QIdentityProxyModel::headerData(section, orientation, role);
}

QVariant HeaderProxyModel::horizontalHeaderData(int column, int role) const
{
  if(column < 0){
    return QVariant();
  }
  if(column >= mHorizontalHeders.count()){
    if(sourceModel() == nullptr){
      return QVariant();
    }
    return sourceModel()->headerData(column, Qt::Horizontal, role);
  }
  switch(static_cast<Qt::ItemDataRole>(role)){
    case Qt::DisplayRole:
      return mHorizontalHeders.labelAt(column);
    default:
      return QIdentityProxyModel::headerData(column, Qt::Horizontal, role);
  }
  return QVariant();
}

void HeaderProxyModel::setHorizontalHeaderLabels(const QStringList & labels)
{
  mHorizontalHeders.setLabels(labels);
  if(mHorizontalHeders.count() > 0){
    emit headerDataChanged(Qt::Horizontal, 0, mHorizontalHeders.count()-1);
  }
}

}} // namespace Mdt{ namespace ItemModel{
