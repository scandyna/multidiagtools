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
#include "mdtCsvStringParser.h"
#include "mdtCsvParserTemplate.h"

mdtCsvStringParser::mdtCsvStringParser()
 : pvImpl(new mdtCsvParserTemplate<mdtCsvStringParserIterator>())
{
}

mdtCsvStringParser::mdtCsvStringParser(const mdtCsvParserSettings & csvSettings)
 : mdtCsvStringParser()
{
  Q_ASSERT(csvSettings.isValid());
  pvImpl->setupParser(csvSettings);
}

mdtCsvStringParser::~mdtCsvStringParser()
{
}

void mdtCsvStringParser::setCsvSettings(const mdtCsvParserSettings & csvSettings)
{
  Q_ASSERT(csvSettings.isValid());

  pvImpl->setupParser(csvSettings);
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

mdtExpected<mdtCsvRecord> mdtCsvStringParser::readLine()
{
  Q_ASSERT_X(pvImpl->isValid(), "mdtCsvStringParser", "No CSV settings set");

  return pvImpl->readLine(pvCurrentPosition, pvEnd);
}

mdtExpected<mdtCsvData> mdtCsvStringParser::readAll()
{
  Q_ASSERT_X(pvImpl->isValid(), "mdtCsvStringParser", "No CSV settings set");

  mdtCsvData data;

  while(!atEnd()){
    auto record = readLine();
    if(!record){
      return record.error();
    }
    data.addRecord(record.value());
  }

  return data;
}
