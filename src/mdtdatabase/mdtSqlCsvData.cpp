/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtSqlCsvData.h"
#include "mdtSqlField.h"
#include <QSqlField>
#include <QMetaType>

namespace mdtSqlCsvData
{

  QMetaType::Type csvFieldTypeFromMdtSqlFieldType(mdtSqlFieldType::Type sqlType)
  {
    switch(sqlType){
      case mdtSqlFieldType::UnknownType:
        return QMetaType::UnknownType;
      case mdtSqlFieldType::Boolean:
        return QMetaType::Bool;
      case mdtSqlFieldType::Integer:
        return QMetaType::Int;
      case mdtSqlFieldType::Float:
        return QMetaType::Float;
      case mdtSqlFieldType::Double:
        return QMetaType::Double;
      case mdtSqlFieldType::Varchar:
        return QMetaType::QString;
      case mdtSqlFieldType::Date:
        return QMetaType::QDate;
      case mdtSqlFieldType::Time:
        return QMetaType::QTime;
      case mdtSqlFieldType::DateTime:
        return QMetaType::QDateTime;
    }
    return QMetaType::UnknownType;
  }

  mdtCsvRecord csvRecordFromQsqlRecord(const QSqlRecord & sqlRecord)
  {
    mdtCsvRecord csvRecord;

    csvRecord.columnDataList.reserve(sqlRecord.count());
    for(int i = 0; i < sqlRecord.count(); ++i){
      csvRecord.columnDataList.append(sqlRecord.value(i));
    }

    return csvRecord;
  }

  mdtCsvRecordFormat csvRecordFormatFromQsqlRecord(const QSqlRecord & sqlRecord)
  {
    mdtCsvRecordFormat format(sqlRecord.count());

    for(int i = 0; i < sqlRecord.count(); ++i){
      format.setFieldType(i, static_cast<QMetaType::Type>(sqlRecord.field(i).type()));
    }

    return format;
  }

  mdtExpected<bool> fillQSqlRecord(QSqlRecord & sqlRecord, mdtCsvRecord csvRecord)
  {
    Q_ASSERT(sqlRecord.count() == csvRecord.count());

    // Format (the copy of) CSV record regarding SQL record
    auto format = csvRecordFormatFromQsqlRecord(sqlRecord);
    if(!format.formatRecord(csvRecord)){
      return format.lastError();
    }
    // Copy data
    for(int i = 0; i < sqlRecord.count(); ++i){
      sqlRecord.setValue(i, csvRecord.columnDataList.at(i));
    }

    return true;
  }
}