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
#include "CsvFileParser.h"
#include "CsvParserTemplate.h"
#include <QDir>
#include <QCoreApplication>

#define tr(sourceText) QCoreApplication::translate("MyClass", sourceText)

namespace Mdt{ namespace PlainText{

CsvFileParser::CsvFileParser()
 : mParser( std::make_unique< CsvParserTemplate<FileMultiPassIterator> >() )
{
}

CsvFileParser::~CsvFileParser()
{
}

void CsvFileParser::setCsvSettings(const CsvParserSettings & settings)
{
  Q_ASSERT(settings.isValid());
  Q_ASSERT(!mFile.isOpen());

  mParser->setupParser(settings);
}

bool CsvFileParser::openFile(const QFileInfo & fileInfo, const QByteArray & encoding)
{
  // Close possibly previously open file
  if(mFile.isOpen()){
    closeFile();
  }
  // Open file
  mFile.setFileName(fileInfo.absoluteFilePath());
  if(!mFile.open(QIODevice::ReadOnly)){
    const auto msg = tr("Could not open file '%1'\nDirectory: '%2'").arg(fileInfo.fileName(), fileInfo.dir().absolutePath());
    mLastError = mdtErrorNew(msg, Mdt::Error::Critical, "CsvFileParser");
    mLastError.stackError(mdtErrorFromQFile(mFile, "CsvFileParser"));
    mLastError.commit();
    return false;
  }
  // Assign file iterator
  if(!mFileIterator.setSource(&mFile, encoding)){
    mLastError = mFileIterator.lastError();
    return false;
  }

  return true;
}

bool CsvFileParser::isOpen() const
{
  return mFile.isOpen();
}

void CsvFileParser::closeFile()
{
  mFileIterator.clear();
  mFile.close();
}

bool CsvFileParser::atEnd() const
{
  return mFileIterator.isEof();
}

Expected<StringRecord> CsvFileParser::readLine()
{
  Q_ASSERT_X(mParser->isValid(), "CsvFileParser", "No CSV settings set");

  Expected<StringRecord> record;

  // Create multi pass iterators
  auto first = makeFileMultiPassIterator(mFileIterator);
  auto last = makeFileMultiPassIterator(FileInputIterator());
  // Parse a line
  record = mParser->readLine(first, last);
  if(!record){
    // Maybe a error on file
    if(mFileIterator.errorOccured()){
      record.error().stackError(mFileIterator.lastError());
    }
  }

  return record;
}

Expected<StringRecordList> CsvFileParser::readAll()
{
  Q_ASSERT_X(mParser->isValid(), "CsvFileParser", "No CSV settings set");

  StringRecordList recList;

  while(!atEnd()){
    auto record = readLine();
    if(!record){
      return record.error();
    }
    recList.appendRecord(record.value());
  }

  return recList;
}


}} // namespace Mdt{ namespace PlainText{
