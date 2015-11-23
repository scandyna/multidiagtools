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
#include "mdtCsvParser.h"
#include "mdtCsvParserTemplate.h"

/*
 * mdtCsvStringParser implementation
 */

mdtCsvStringParser::mdtCsvStringParser()
 : pvImpl(new mdtCsvParserTemplate<std::wstring::const_iterator>)
{
}

mdtCsvStringParser::~mdtCsvStringParser()
{
}

void mdtCsvStringParser::setSettings(const mdtCsvParserSettings & s)
{
  pvImpl->setSettings(s);
}

void mdtCsvStringParser::setSource(const QString & source)
{
  /*
   * We have to convert source to std::wstring type,
   * witch is supported by boost::spirit.
   * A try was made to include QString support in spirit,
   * but this never worked.
   */
  pvSource = source.toStdWString();
  pvImpl->setSource(pvSource.cbegin(), pvSource.cend());
}

bool mdtCsvStringParser::atEnd() const
{
  return pvImpl->atEnd();
}

mdtCsvRecord mdtCsvStringParser::readLine()
{
  mdtCsvRecord record;
  mdtCsvRawRecord rawRecord;

  rawRecord = pvImpl->readLine();
  record.columnDataList.reserve(rawRecord.count());
  for(const auto & data : rawRecord.columnDataList){
    record.columnDataList.append(QString::fromStdWString(data));
  }

  return record;
}
