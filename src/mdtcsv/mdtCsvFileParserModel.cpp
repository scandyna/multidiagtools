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
#include "mdtCsvFileParserModel.h"
#include "mdtCsvFileParser.h"

#include <QDebug>

mdtCsvFileParserModel::mdtCsvFileParserModel(QObject *parent)
 : mdtCsvParserModel(parent)
{
}

mdtCsvFileParserModel::~mdtCsvFileParserModel()
{
}

mdtExpected<bool> mdtCsvFileParserModel::openFile(const QFileInfo & fileInfo, const QByteArray & fileEncoding, const mdtCsvParserSettings & settings)
{
  Q_ASSERT(settings.isValid());

  beginResetModel();
  // Reset parser and open file
  pvParser.reset(new mdtCsvFileParser(settings));
  if(!pvParser->openFile(fileInfo, fileEncoding)){
    endResetModel();
    return pvParser->lastError();
  }
  // Parse header
  auto record = pvParser->readLine();
  if(!record){
    endResetModel();
    return record.error();
  }
  setHeader(record.value());
  endResetModel();

  return true;
}

bool mdtCsvFileParserModel::canFetchMore(const QModelIndex & parent) const
{
  if(parent.isValid()){
    return false;
  }
  qDebug() << "canFetchMore(): " << (pvParser && (!pvParser->atEnd()) );
  return (pvParser && (!pvParser->atEnd()) );
}

void mdtCsvFileParserModel::fetchMore(const QModelIndex & parent)
{
  if(parent.isValid()){
    return;
  }
  qDebug() << "fetchMore() ...";
  Q_ASSERT(pvParser);
  if(pvParser->atEnd()){
    qDebug() << "-> at END...(?)";
    return;
  }
  Q_ASSERT(!pvParser->atEnd());
  auto record = pvParser->readLine();
  if(record){
    qDebug() << "REC: " << record.value().columnDataList;
    addRecord(record.value());
  }
}
