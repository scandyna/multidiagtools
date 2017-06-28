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
#include "Record.h"

namespace Mdt{ namespace PlainText{

// Record::Record(std::initializer_list<QVariant> list)
//  : mRecord(list)
// {
// }
// 
// void Record::appendColumn()
// {
//   setColumnCount( columnCount() + 1 );
// }
// 
// void Record::setColumnCount(int count)
// {
//   Q_ASSERT(count > 0);
// 
//   mRecord.resize(count);
// }
// 
// void Record::setData(int column, const QVariant & data)
// {
//   Q_ASSERT(column >= 0);
//   Q_ASSERT(column < columnCount());
// 
//   mRecord[column] = data;
// }
// 
// void Record::clear()
// {
//   mRecord.clear();
// }
// 
// Record & Record::operator<<(const QVariant & data)
// {
//   mRecord << data;
//   return *this;
// }

}} // namespace Mdt{ namespace PlainText{
