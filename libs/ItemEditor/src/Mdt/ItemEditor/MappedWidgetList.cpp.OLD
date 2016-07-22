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
#include "MappedWidgetList.h"
#include <QAbstractItemModel>

// #include <QDebug>

namespace Mdt{ namespace ItemEditor{

MappedWidgetList::MappedWidgetList(QObject* parent)
 : QObject(parent)
{
}

void MappedWidgetList::addWidget(QWidget* widget, int column)
{
  Q_ASSERT(widget != nullptr);
  Q_ASSERT(column >= 0);

  updateWidgetEnableState(widget, column);
  pvWidgetList.emplace_back(widget, column);
}

void MappedWidgetList::setModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  pvModel = model;
  updateAllWidgetsEnableState();
}

void MappedWidgetList::clear()
{
  pvWidgetList.clear();
}

void MappedWidgetList::setRowState(RowState rs)
{
  pvRowState = rs;
  updateAllWidgetsEnableState();
}

void MappedWidgetList::updateWidgetEnableState(QWidget*const widget, int column)
{
  if(widget != nullptr){
    if(pvModel.isNull()){
      widget->setEnabled(false);
    }else{
      widget->setEnabled( (!pvRowState.isNull()) && (column < pvModel->columnCount()) );
    }
  }
}

void MappedWidgetList::updateAllWidgetsEnableState()
{
  for(auto & wc : pvWidgetList){
    updateWidgetEnableState(wc.widget, wc.column);
  }
}

}} // namespace Mdt{ namespace ItemEditor{