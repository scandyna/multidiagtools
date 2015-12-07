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
#include "mdtCsvStringParser.h"
#include "mdtCsvParserTemplate.h"

/*
 * mdtCsvStringParser implementation
 */

mdtCsvStringParser::mdtCsvStringParser(const mdtCsvParserSettings & csvSettings)
 : pvImpl(new mdtCsvParserTemplate<mdtCsvStringParserIterator>(csvSettings))
{
}

mdtCsvStringParser::~mdtCsvStringParser()
{
}

void mdtCsvStringParser::setSource(const QString & source)
{
  pvCurrentPosition = source.cbegin();
  pvEnd = source.cend();
}

bool mdtCsvStringParser::atEnd() const
{
  return (pvCurrentPosition == pvEnd);
}

mdtCsvRecord mdtCsvStringParser::readLine()
{
  mdtCsvRecord record;

  record = pvImpl->readLine(pvCurrentPosition, pvEnd);
  if(record.errorOccured()){
    pvLastError = pvImpl->lastError();
  }

  return record;
}

mdtCsvData mdtCsvStringParser::readAll()
{
  mdtCsvData data;

  while(!atEnd()){
    auto record = readLine();
    if(record.errorOccured()){
      data.setErrorOccured();
      return data;
    }
    data.addRecord(record);
  }

  return data;
}
