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
#include "CsvStringImportTableMapping.h"
#include "mdtCsvStringParser.h"

namespace mdt{ namespace sql{ namespace copier{

bool CsvStringImportTableMapping::setSourceString(const QString & csv)
{
  Q_ASSERT(sourceCsvSettings().isValid());

  pvSourceString = csv;

  return parseSourceHeader();
}

bool CsvStringImportTableMapping::parseSourceHeader()
{
  Q_ASSERT(sourceCsvSettings().isValid());

  mdtCsvStringParser parser(sourceCsvSettings());

  clearFieldMapping();
  parser.setSource(pvSourceString);
  auto record = parser.readLine();
  if(!record){
    pvLastError = record.error();
    return false;
  }
  setSourceHeader(record.value());
  resetFieldMapping();

  return true;
}

}}} // namespace mdt{ namespace sql{ namespace copier{
