/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "CsvStringParser.h"
#include "CsvParserTemplate.h"

namespace Mdt{ namespace PlainText{

CsvStringParser::CsvStringParser()
 : mParser(std::make_unique< CsvParserTemplate<StringConstIterator> >())
{
}

CsvStringParser::~CsvStringParser()
{
}

void CsvStringParser::setCsvSettings(const CsvParserSettings& settings)
{
  Q_ASSERT(settings.isValid());

  mParser->setupParser(settings);
}

void CsvStringParser::setSource(const QString & source)
{
  mCurrentPosition = source.cbegin();
  mEnd = source.cend();
}

bool CsvStringParser::atEnd() const
{
  return (mCurrentPosition == mEnd);
}

Expected<StringRecord> CsvStringParser::readLine()
{
  Q_ASSERT_X(mParser->isValid(), "CsvStringParser", "No CSV settings set");

  return mParser->readLine(mCurrentPosition, mEnd);
}

Expected<StringRecordList> CsvStringParser::readAll()
{
  Q_ASSERT_X(mParser->isValid(), "CsvStringParser", "No CSV settings set");

  StringRecordList recordList;

  while(!atEnd()){
    const auto record = readLine();
    if(!record){
      return record.error();
    }
    recordList.appendRecord(record.value());
  }

  return recordList;
}

Error CsvStringParser::lastError() const
{

}

}} // namespace Mdt{ namespace PlainText{
