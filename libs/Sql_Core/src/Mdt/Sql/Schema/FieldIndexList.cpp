/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "FieldIndexList.h"
#include <algorithm>

namespace Mdt{ namespace Sql{ namespace Schema{

void FieldIndexList::addFieldIndex(int fieldIndex)
{
  Q_ASSERT(fieldIndex >= 0);
  Q_ASSERT(!containsFieldIndex(fieldIndex));

  mList.push_back(fieldIndex);
}

bool FieldIndexList::containsFieldIndex(int fieldIndex) const
{
  return ( std::find(mList.cbegin(), mList.cend(), fieldIndex) != mList.cend() );
}

void FieldIndexList::clear()
{
  mList.clear();
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{
