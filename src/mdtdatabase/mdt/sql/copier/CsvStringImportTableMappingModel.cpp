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
#include "CsvStringImportTableMappingModel.h"

namespace mdt{ namespace sql{ namespace copier{

CsvStringImportTableMappingModel::CsvStringImportTableMappingModel(QObject* parent)
 : mdt::sql::copier::TableMappingModel(parent),
   pvMapping(std::make_shared<CsvStringImportTableMapping>())
{
}

bool CsvStringImportTableMappingModel::setSourceString(const QString & csv)
{
  bool ok;

  beginResetModel();
  ok = pvMapping->setSourceString(csv);
  endResetModel();
  if(!ok){
    pvLastError = pvMapping->lastError();
    return false;
  }

  return true;
}

bool CsvStringImportTableMappingModel::setDestinationTable(const QString & tableName, const QSqlDatabase & db)
{
  bool ok;

  beginResetModel();
  ok = pvMapping->setDestinationTable(tableName, db);
  endResetModel();
  if(!ok){
    pvLastError = pvMapping->lastError();
    return false;
  }

  return true;
}

}}} // namespace mdt{ namespace sql{ namespace copier{
