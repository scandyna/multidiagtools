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
                  + tr("Director: '") + fileInfo.dir().absolutePath() + tr("'");
    pvLastError.setError(msg, mdtError::Error);
    pvLastError.setSystemError(0, pvFile.errorString());
    MDT_ERROR_SET_SRC(pvLastError, "mdtCsvFileParserIteratorSharedData");
    pvLastError.commit();
    return false;
  }
  // Assign file iterator
  pvFileIterator = mdtCsvFileParserIterator(&pvFile, encoding);
  if(pvFileIterator.errorOccured()){
    pvLastError = pvFileIterator.lastError();
    return false;
  }
  // Assign multi pass iterators
//   pvCurrentPosition = boost::spirit::make_multi_pass<mdtCsvFileParserMultiPassPolicy, mdtCsvFileParserIterator>(mdtCsvFileParserIterator(pvFileIterator));
//   pvEnd = boost::spirit::make_multi_pass<mdtCsvFileParserMultiPassPolicy, mdtCsvFileParserIterator>(mdtCsvFileParserIterator());

  return true;
}

void mdtCsvFileParser::closeFile()
{
  // Iterators ...
  
  // Close file
  pvFile.close();
}

mdtCsvRecord mdtCsvFileParser::readLine()
{

  auto first = boost::spirit::make_multi_pass<mdtCsvFileParserMultiPassPolicy, mdtCsvFileParserIterator>(mdtCsvFileParserIterator(pvFileIterator));
  auto last = boost::spirit::make_multi_pass<mdtCsvFileParserMultiPassPolicy, mdtCsvFileParserIterator>(mdtCsvFileParserIterator());

  return pvParser->readLine(first, last);
}

QString mdtCsvFileParser::tr(const char* sourceText)
{
  return QObject::tr(sourceText);
}
