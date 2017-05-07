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
#include "RowColumnListBase.h"
#include <QtGlobal>
#include <set>
#include <algorithm>

#ifdef QT_DEBUG
#define mdtItemModelRowColumnListAssert(list)  \
for(const auto col : list)  \
{                           \
  Q_ASSERT(col >= 0);       \
  std::set<int> s(mList.cbegin(), mList.cend());  \
  Q_ASSERT_X(s.size() == mList.size(), "RowColumnListBase", "at least one column exists more than once, which is not allowed");                   \
}
#else
#define mdtItemModelRowColumnListAssert(list)
#endif // #ifdef QT_DEBUG

namespace Mdt{ namespace ItemModel{

RowColumnListBase::RowColumnListBase(std::initializer_list< int > list)
 : mList(list)
{
  mdtItemModelRowColumnListAssert(mList);
}

bool RowColumnListBase::contains(int e) const
{
  return (std::find(mList.cbegin(), mList.cend(), e) != mList.cend());
}

void RowColumnListBase::append(int e)
{
  Q_ASSERT(e >= 0);
  Q_ASSERT(!contains(e));
  mList.push_back(e);
}

int RowColumnListBase::greatest() const
{
  const auto it = std::max_element(mList.cbegin(), mList.cend());
  if(it == mList.cend()){
    return -1;
  }
  return *it;
}

void RowColumnListBase::clear()
{
  mList.clear();
}

}} // namespace Mdt{ namespace ItemModel{
