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
#include "mdtSqlCopierDataMapping.h"
#include "mdt/sql/copier/TableMapping.h"
#include <QSqlRecord>

void mdtSqlCopierDataMapping::setSourceRecord(const QSqlRecord & sourceRecord, const std::shared_ptr<mdt::sql::copier::TableMapping> & mapping)
{
  Q_ASSERT(mapping);

  clear();
  for(int i = 0; i < sourceRecord.count(); ++i){
    addSourceRecordItem(i, sourceRecord.value(i), mapping);
  }
}

void mdtSqlCopierDataMapping::addSourceRecordItem(int sourceRecordIndex, const QVariant & sourceRecordItem, const std::shared_ptr<mdt::sql::copier::TableMapping> & mapping)
{
  destinationRecord.append(sourceRecordItem);
}

