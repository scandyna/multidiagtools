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
#include "RecordList.h"

namespace Mdt{ namespace PlainText{

RecordList::RecordList(std::initializer_list<Record> list)
 : mRecordList(list)
{
}

void RecordList::appendRecord(const Record & record)
{
  mRecordList.append(record);
}

void RecordList::setData(int row, int column, const QVariant & data)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < columnCount(row));

  mRecordList[row].setData(column, data);
}

void RecordList::clear()
{
  mRecordList.clear();
}

RecordList & RecordList::operator<<(const Record & record)
{
  mRecordList << record;
  return *this;
}

}} // namespace Mdt{ namespace PlainText{
