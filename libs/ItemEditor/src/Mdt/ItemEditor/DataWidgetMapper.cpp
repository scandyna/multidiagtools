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
#include "DataWidgetMapper.h"

#include <QAbstractItemModel>
#include <QStyledItemDelegate>
#include <QStyledItemDelegate>
#include <QWidget>

#include <QDebug>

namespace Mdt{ namespace ItemEditor{

DataWidgetMapper::DataWidgetMapper(QObject* parent)
 : QObject(parent),
   pvCurrentRow(-1)
{
  setItemDelegate(new QStyledItemDelegate(this));
}

void DataWidgetMapper::setModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);
  Q_ASSERT_X(pvMappedWidgetList.isEmpty(), "DataWidgetMapper::setModel()", "setting model while widgets are allready mapped is not allowed");

  if(model == pvModel){
    return;
  }
  if(!pvModel.isNull()){
    // disconnect()
  }
  pvModel = model;
  // connect()
}

QAbstractItemModel* DataWidgetMapper::model() const
{
  return pvModel;
}

void DataWidgetMapper::setItemDelegate(QAbstractItemDelegate* delegate)
{
  Q_ASSERT(delegate != nullptr);
  
  if(!pvDelegate.isNull()){
    // disconnect() ...
  }
  pvDelegate = delegate;
  // connect()

}

QAbstractItemDelegate* DataWidgetMapper::itemDelegate() const
{
  return pvDelegate;
}

void DataWidgetMapper::addMapping(QWidget* widget, int column)
{

}

void DataWidgetMapper::clearMapping()
{

}

void DataWidgetMapper::setCurrentRow(int row)
{

}

bool DataWidgetMapper::submit()
{

}

void DataWidgetMapper::revert()
{

}

}} // namespace Mdt{ namespace ItemEditor{
