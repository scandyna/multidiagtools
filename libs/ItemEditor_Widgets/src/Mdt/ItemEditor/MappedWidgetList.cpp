/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include <algorithm>
#include <iterator>

namespace Mdt{ namespace ItemEditor{

void MappedWidgetList::addMapping(QWidget* widget, int column)
{
  Q_ASSERT(widget != nullptr);
  Q_ASSERT(column >= 0);
  Q_ASSERT(getIndexForWidget(widget) == -1);
  Q_ASSERT(getIndexForColumn(column) == -1);

  mWidgetList.emplace_back(widget, column);
}

int MappedWidgetList::getIndexForWidget(const QWidget*const widget) const
{
  Q_ASSERT(widget != nullptr);

  const auto it = iteratorForWidget(widget);
  if(it == mWidgetList.cend()){
    return -1;
  }

  return std::distance(mWidgetList.cbegin(), it);
}

int MappedWidgetList::getIndexForColumn(int column) const
{
  Q_ASSERT(column >= 0);

  const auto it = iteratorForColumn(column);
  if(it == mWidgetList.cend()){
    return -1;
  }

  return std::distance(mWidgetList.cbegin(), it);
}

QWidget* MappedWidgetList::widgetAt(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < size());

  return mWidgetList[index].widget();
}

void MappedWidgetList::removeMappingAt(int index)
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < size());

  mWidgetList.erase( mWidgetList.begin() + index );
}

void MappedWidgetList::clear()
{
  mWidgetList.clear();
}

MappedWidgetList::const_iterator MappedWidgetList::iteratorForWidget(const QWidget*const widget) const
{
  const auto pred = [widget](const MappedWidget & mw){
    return ( mw.widget() == widget );
  };
  return std::find_if(mWidgetList.begin(), mWidgetList.end(), pred);
}

MappedWidgetList::const_iterator MappedWidgetList::iteratorForColumn(int column) const
{
  const auto pred = [column](const MappedWidget & mw){
    return ( mw.column() == column);
  };
  return std::find_if(mWidgetList.begin(), mWidgetList.end(), pred);
}

}} // namespace Mdt{ namespace ItemEditor{
