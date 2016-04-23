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
#include "CsvFileImportTableMapping.h"
#include "mdtCsvFileParser.h"

//#include <QDebug>

namespace mdt{ namespace sql{ namespace copier{

bool CsvFileImportTableMapping::setSourceFile(const QFileInfo & file, const QByteArray & fileEncoding)
{
  Q_ASSERT(sourceCsvSettings().isValid());

  pvSourceFile = file;
  pvSourceFileEncoding = fileEncoding;

  return parseSourceHeader();
}

bool CsvFileImportTableMapping::parseSourceHeader()
{
  Q_ASSERT(sourceCsvSettings().isValid());

  mdtCsvFileParser parser(sourceCsvSettings());

  clearFieldMapping();
  if(!parser.openFile(pvSourceFile, pvSourceFileEncoding)){
    pvLastError = parser.lastError();
    return false;
  }
  auto record = parser.readLine();
  if(!record){
    pvLastError = record.error();
    return false;
  }
  setSourceHeader(record.value());
///   setFormat(mdtCsvRecordFormat(fieldCount(), QMetaType::QString));
  resetFieldMapping();

  return true;
}

}}} // namespace mdt{ namespace sql{ namespace copier{
