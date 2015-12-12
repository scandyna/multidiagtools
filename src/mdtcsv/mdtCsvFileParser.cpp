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
#include "mdtCsvFileParser.h"
#include "mdtCsvParserTemplate.h"
#include <boost/spirit/include/support_multi_pass.hpp>
#include <QObject>
#include <QDir>

mdtCsvFileParser::mdtCsvFileParser(const mdtCsvParserSettings & csvSettings)
 : pvParser(new mdtCsvParserTemplate<mdtCsvFileParserMultiPassIterator>(csvSettings))
{
}

mdtCsvFileParser::~mdtCsvFileParser()
{
}

bool mdtCsvFileParser::openFile(const QFileInfo & fileInfo, const QByteArray & encoding)
{
  // Close possibly previously open file
  if(pvFile.isOpen()){
    closeFile();
  }
  // Open file
  pvFile.setFileName(fileInfo.absoluteFilePath());
  if(!pvFile.open(QIODevice::ReadOnly)){
    QString msg = tr("Could not open file '") + fileInfo.fileName() + tr("'\n") \
                  + tr("Directory: '") + fileInfo.dir().absolutePath() + tr("'");
    pvLastError.setError(msg, mdtError::Error);
    pvLastError.setSystemError(0, pvFile.errorString());
    MDT_ERROR_SET_SRC(pvLastError, "mdtCsvFileParserIteratorSharedData");
    pvLastError.commit();
    return false;
  }
  // Assign file iterator
  if(!pvFileIterator.setSource(&pvFile, encoding)){
    pvLastError = pvFileIterator.lastError();
    return false;
  }

  return true;
}

void mdtCsvFileParser::closeFile()
{
  pvFileIterator.clear();
  pvFile.close();
}

bool mdtCsvFileParser::atEnd() const
{
  return pvFileIterator.isEof();
}

mdtCsvRecord mdtCsvFileParser::readLine()
{
  mdtCsvRecord record;

  // Create multi pass iterators
  auto first = mdtCsvFileParserMultiPassIterator(pvFileIterator);
  auto last = mdtCsvFileParserMultiPassIterator(mdtCsvFileParserIterator());
  // Parse a line
  record = pvParser->readLine(first, last);
  if(record.errorOccured()){
    // Maybe a error on file
    if(pvFileIterator.errorOccured()){
      pvLastError = pvFileIterator.lastError();
    }else{
      pvLastError = pvParser->lastError();
    }
  }

  return record;
}

mdtCsvData mdtCsvFileParser::readAll()
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

QString mdtCsvFileParser::tr(const char* sourceText)
{
  return QObject::tr(sourceText);
}
