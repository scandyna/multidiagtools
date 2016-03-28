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

#include <QDebug>

mdtCsvFileParserModel::mdtCsvFileParserModel(QObject *parent)
 : mdtCsvParserModel(parent)
{
}

mdtCsvFileParserModel::~mdtCsvFileParserModel()
{
}

bool mdtCsvFileParserModel::setCsvSettings(const mdtCsvParserSettings & csvSettings)
{
  Q_ASSERT(csvSettings.isValid());

  pvCsvSettings = csvSettings;
  // If no file was set, do nothing else
  if(pvFileInfo.fileName().isEmpty()){
    return true;
  }
  if(pvFileEncoding.isEmpty()){
    return true;
  }
  // Update
  bool ok;
  beginResetModel();
  ok = openFile();
  endResetModel();

  return ok;
}

bool mdtCsvFileParserModel::setFile(const QFileInfo & fileInfo, const QByteArray & fileEncoding)
{
  Q_ASSERT(!fileInfo.fileName().isEmpty());
  Q_ASSERT(!fileEncoding.isEmpty());

  pvFileInfo = fileInfo;
  pvFileEncoding = fileEncoding;
  // If we have no CSV settings, do nothing
  if(!pvCsvSettings.isValid()){
    pvLastError = mdtErrorNewQ(tr("Could not open file '") + pvFileInfo.fileName() + tr("' because of invalid CSV settings."), mdtError::Error, this);
    return false;
  }
  // Update
  bool ok;
  beginResetModel();
  ok = openFile();
  endResetModel();

  return ok;
}

bool mdtCsvFileParserModel::canFetchMore(const QModelIndex & parent) const
{
  if(parent.isValid()){
    return false;
  }
  qDebug() << "canFetchMore(): " << (!pvParser.atEnd());
  return (!pvParser.atEnd());
}

void mdtCsvFileParserModel::fetchMore(const QModelIndex & parent)
{
  if(parent.isValid()){
    return;
  }
  qDebug() << "fetchMore() ...";
//   Q_ASSERT(pvParser);
  if(pvParser.atEnd()){
    qDebug() << "-> at END...(?)";
    return;
  }
  Q_ASSERT(!pvParser.atEnd());
  auto record = pvParser.readLine();
  if(record){
    qDebug() << "REC: " << record.value().columnDataList;
    addRecord(record.value());
  }
  /// \todo Error handling..
}

bool mdtCsvFileParserModel::openFile()
{
  Q_ASSERT(!pvFileInfo.fileName().isEmpty());
  Q_ASSERT(!pvFileEncoding.isEmpty());
  Q_ASSERT(pvCsvSettings.isValid());

  // Close possibly open file
  pvParser.closeFile();
  clearCache();
  // Open file
  pvParser.setCsvSettings(pvCsvSettings);
  if(!pvParser.openFile(pvFileInfo, pvFileEncoding)){
    pvLastError = mdtErrorNewQ(tr("Could not open file '") + pvFileInfo.fileName() + tr("'"), mdtError::Error, this);
    pvLastError.stackError(pvParser.lastError());
    return false;
  }
  // Parse header
  auto record = pvParser.readLine();
  if(!record){
    pvLastError = mdtErrorNewQ(tr("Could not parse header of file '") + pvFileInfo.fileName() + tr("'"), mdtError::Error, this);
    pvLastError.stackError(record.error());
    return false;
  }
  setHeader(record.value());

  return true;
}
