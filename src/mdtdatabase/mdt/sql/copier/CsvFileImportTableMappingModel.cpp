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
#include "CsvFileImportTableMappingModel.h"

namespace mdt{ namespace sql{ namespace copier{

CsvFileImportTableMappingModel::CsvFileImportTableMappingModel(QObject* parent)
 : mdt::sql::copier::TableMappingModel(parent),
   pvMapping(std::make_shared<CsvFileImportTableMapping>())
{
}

bool CsvFileImportTableMappingModel::setSourceCsvFile(const QFileInfo & csvFile, const QByteArray & csvFileEncoding, const mdtCsvParserSettings & settings)
{
  bool ok;

  beginResetModel();
  ok = pvMapping->setSourceCsvFile(csvFile, csvFileEncoding, settings);
  endResetModel();
  if(!ok){
    pvLastError = pvMapping->lastError();
    return false;
  }

  return true;
}

bool CsvFileImportTableMappingModel::setDestinationTable(const QString & tableName, const QSqlDatabase & db)
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

void CsvFileImportTableMappingModel::setMapping(const std::shared_ptr<CsvFileImportTableMapping> & m)
{
  Q_ASSERT(m);

  beginResetModel();
  pvMapping = m;
  endResetModel();
}

}}} // namespace mdt{ namespace sql{ namespace copier{
