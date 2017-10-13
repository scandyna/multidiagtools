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
#include "FileBackend.h"
#include "FileBackendFormatEngine.h"
#include <QFile>
#include <QStringBuilder>
#include <QDebug>
#include <QDebugStateSaver>
#include <QTextStream>

namespace Mdt{ namespace ErrorLogger {

FileBackend::FileBackend(QObject *parent)
 : Backend(parent),
   mMaxFileSize(1024*1024)
{
  setFormatEngine<FileBackendFormatEngine>();
}

FileBackend::~FileBackend()
{
}

bool FileBackend::setLogFilePath(const QString & path, qint64 maxFileSize)
{
  // Try to open the file to see if path is valid and writeable
  QFile file(path);
  if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
    qWarning() << tr("Mdt::ErrorLogger::FileBackend::setLogFilePath() : could not open file %1.\n Error: %2")
                    .arg(path, file.errorString());
    return false;
  }
  file.close();
  // Store infos
  mMaxFileSize = maxFileSize;
  mFilePath = path;

  return true;
}

QString FileBackend::logFilePath() const
{
  return mFilePath;
}

QString FileBackend::backupLogFilePath() const
{
  if(mFilePath.isEmpty()){
    return QString();
  }
  return mFilePath % QLatin1String(".save");
}

qint64 FileBackend::maxFileSize() const
{
  return mMaxFileSize;
}

void FileBackend::logError(const Error & error)
{
  QFile file(mFilePath);

  // Open log file
  if(!openFile(file)){
    return;
  }
  // backup log file if needed
  if(file.size() >= mMaxFileSize){
    file.close();
    backupLogFile();
    if(!openFile(file)){
      return;
    }
  }
  Q_ASSERT(file.isOpen());
  Q_ASSERT(file.isWritable());
  // Write error to file
  QTextStream stream(&file);
  stream << formatError(error) << "\n";
  stream.flush();
  file.close();
}

void FileBackend::cleanup()
{
  if(mFilePath.isEmpty()){
    return;
  }
  // If we have a empty log file, we remove it
  QFile file(mFilePath);
  if(!file.open(QFile::ReadOnly | QFile::Text)){
    qWarning() << tr("Mdt::ErrorLogger::FileBackend::cleanup() : could not open file %1.\n Error: %2")
                    .arg(mFilePath, file.errorString());
    return;
  }
  if(file.size() > 0){
    file.close();
    return;
  }
  if(!file.remove()){
    qWarning() << tr("Mdt::ErrorLogger::FileBackend::cleanup() : could not remove file %1.\n Error: %2")
                    .arg(mFilePath, file.errorString());
  }
}

bool FileBackend::openFile(QFile & file)
{
  Q_ASSERT(!file.isOpen());
  Q_ASSERT(!file.fileName().isEmpty());

  if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
    qWarning() << tr("Mdt::ErrorLogger::FileBackend::openFile() : could not open file %1.\n Error: %2")
                    .arg(mFilePath, file.errorString());
    return false;
  }
  return true;
}

void FileBackend::backupLogFile()
{
  if(QFile::exists( backupLogFilePath() )){
    if(!QFile::remove( backupLogFilePath() )){
    qWarning() << tr("Mdt::ErrorLogger::FileBackend::backupLogFile() : could not remove old backup file %1.")
	                .arg( backupLogFilePath() );
      return;
    }
  }
  if(!QFile::rename( mFilePath, backupLogFilePath() )){
    qWarning() << tr("Mdt::ErrorLogger::FileBackend::backupLogFile() : could not rename file %1.")
                    .arg(mFilePath);
  }
}

}}  // namespace Mdt{ namespace ErrorLogger {
