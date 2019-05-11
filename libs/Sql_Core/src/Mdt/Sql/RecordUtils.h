/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#ifndef MDT_SQL_RECORD_UTILS_H
#define MDT_SQL_RECORD_UTILS_H

#include "Mdt/Container/VariantRecord.h"
#include "MdtSql_CoreExport.h"
#include <QSqlRecord>

namespace Mdt{ namespace Sql{

  /*! \brief Get a variant record from a sql record
   */
  inline
  Mdt::Container::VariantRecord MDT_SQL_CORE_EXPORT variantRecordFromSqlRecord(const QSqlRecord & sqlRecord)
  {
    Mdt::Container::VariantRecord record( sqlRecord.count() );

    const int n = sqlRecord.count();
    for(int col = 0; col < n; ++col){
      record.setValue(col, sqlRecord.value(col));
    }

    return record;
  }

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_RECORD_UTILS_H
