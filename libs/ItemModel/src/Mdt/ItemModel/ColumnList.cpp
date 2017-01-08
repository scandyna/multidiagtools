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
#include "ColumnList.h"
#include <QtGlobal>
#include <set>

#ifdef QT_DEBUG
#define mdtItemModelColumnListAssert(list)  \
for(const auto col : list)  \
{                           \
  Q_ASSERT(col >= 0);       \
  std::set<int> s(mColumnList.cbegin(), mColumnList.cend());  \
  Q_ASSERT(s.size() == mColumnList.size());                   \
}
#else
#define mdtItemModelColumnListAssert(list)
#endif // #ifdef QT_DEBUG

namespace Mdt{ namespace ItemModel{

ColumnList::ColumnList(std::initializer_list< int > list)
 : mColumnList(list)
{
  mdtItemModelColumnListAssert(mColumnList);
}


}} // namespace Mdt{ namespace ItemModel{
