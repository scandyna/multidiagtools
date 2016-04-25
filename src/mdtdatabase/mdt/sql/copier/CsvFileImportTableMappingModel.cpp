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

bool CsvFileImportTableMappingModel::setSourceCsvSettings(const mdtCsvParserSettings & settings)
{
  bool ok;

  beginResetModel();
  ok = pvMapping->setSourceCsvSettings(settings);
  endResetModel();
  if(!ok){
    pvLastError = pvMapping->lastError();
    return false;
  }

  return true;
}

bool CsvFileImportTableMappingModel::setSourceFile(const QFileInfo & file, const QByteArray & fileEncoding)
{
  bool ok;

  beginResetModel();
  ok = pvMapping->setSourceFile(file, fileEncoding);
  endResetModel();
  if(!ok){
    pvLastError = pvMapping->lastError();
    return false;
  }

  return true;
}

void CsvFileImportTableMappingModel::setSourceRecordFormat(const csv::RecordFormat & format)
{
  Q_ASSERT(format.fieldCount() == pvMapping->sourceTableFieldCount());

  pvMapping->setSourceRecordFormat(format);
  for(int row = 0; row < rowCount(); ++row){
    auto idx = index(row, SourceFieldTypeIndex);
    emit dataChanged(idx, idx);
  }
}

csv::RecordFormat CsvFileImportTableMappingModel::sourceRecordFormat() const
{
  return pvMapping->sourceRecordFormat();
}

void CsvFileImportTableMappingModel::setDestinationDatabase(const QSqlDatabase & db)
{
  beginResetModel();
  pvMapping->setDestinationDatabase(db);
  endResetModel();
}

bool CsvFileImportTableMappingModel::setDestinationTable(const QString &tableName)
{
  bool ok;

  beginResetModel();
  ok = pvMapping->setDestinationTable(tableName);
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
