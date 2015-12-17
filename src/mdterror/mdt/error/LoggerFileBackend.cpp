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
#include "LoggerFileBackend.h"
#include <QFile>
#include <QLatin1String>
#include <QDebug>
#include <QDebugStateSaver>
#include <QDateTime>

namespace mdt{ namespace error {

LoggerFileBackend::LoggerFileBackend()
 : pvMaxFileSize(1024*1024)
{
}

LoggerFileBackend::~LoggerFileBackend()
{
}

bool LoggerFileBackend::setLogFilePath(const QString & path, qint64 maxFileSize)
{
  // Store infos
  pvMaxFileSize = maxFileSize;
  pvFilePath = path;
  pvBackupFilePath = path + QLatin1String(".save");
  // Try to open the file to see if path is valid and writeable
  QFile file(pvFilePath);
  if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
    qDebug() << tr("LoggerFileBackend::setLogFilePath() : could not open file ") << pvFilePath \
             << tr(" , error: ") << file.errorString();
    return false;
  }
  file.close();

  return true;
}

QString LoggerFileBackend::logFilePath() const
{
  return pvFilePath;
}

QString LoggerFileBackend::backupLogFilePath() const
{
  return pvBackupFilePath;
}

qint64 LoggerFileBackend::maxFileSize() const
{
  return pvMaxFileSize;
}

void LoggerFileBackend::logError(const mdtError & error)
{
  QFile file(pvFilePath);

  // Open log file
  if(!openFile(file)){
    return;
  }
  // backup log file if needed
  if(file.size() >= pvMaxFileSize){
    file.close();
    backupLogFile();
    if(!openFile(file)){
      return;
    }
  }
  Q_ASSERT(file.isOpen());
  Q_ASSERT(file.isWritable());
  // Use a scope for QDebug, so we are shure that it can flush before the file is closed
  {
    // Setup debugger
    QDebug dbg(&file);
    QDebugStateSaver stateSaver(dbg);
    dbg.resetFormat();
    dbg.noquote();
    dbg.nospace();
    /*
    * We build the string and then write it
    * (prevents splitting of informations of the same error)
    * Note: because potentially all errors are logged,
    *       we not have to write the error stack here.
    */
    dbg << getErrorString(error);
  }
  file.close();
}

bool LoggerFileBackend::openFile(QFile & file)
{
  Q_ASSERT(!file.isOpen());
  Q_ASSERT(!file.fileName().isEmpty());

  if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
    qWarning() << tr("LoggerFileBackend::setLogFilePath() : could not open file ") << pvFilePath \
             << tr(" , error: ") << file.errorString();
    return false;
  }
  return true;
}

QString LoggerFileBackend::getErrorString(const mdtError & error) const
{
  QString str;
  QString informativeText;
  QString lineBegin;

  // Prepare line beginning
  lineBegin = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss") + QLatin1String(" ") \
            + errorLevelText(error.level()) + QLatin1String(": ");
  // Build message
  str = lineBegin + error.functionName() + QLatin1String("\n") \
      + lineBegin + QLatin1String("-> ") + tr("Text: ") + error.text() + QLatin1String("\n");
  informativeText = error.informativeText();
  if(!informativeText.isEmpty()){
    str += lineBegin + QLatin1String("-> ") + error.informativeText() + QLatin1String("\n");
  }
  str += lineBegin + QLatin1String("-> ") + tr("File: ") + error.fileName() + QLatin1String("\n") \
       + lineBegin + QLatin1String("-> ") + tr("Line: ") + QString::number(error.fileLine()) + QLatin1String("\n");

  return str;
}

QString LoggerFileBackend::errorLevelText(mdtError::Level level) const
{
  switch(level){
    case mdtError::NoError:
      return QLatin1String("[NoError]");
    case mdtError::Error:
      return QLatin1String("[Error]");
    case mdtError::Warning:
      return QLatin1String("[Warning]");
    case mdtError::Info:
      return QLatin1String("[Info]");
  }
  return QString();
}

void LoggerFileBackend::backupLogFile()
{
  if(QFile::exists(pvBackupFilePath)){
    if(!QFile::remove(pvBackupFilePath)){
      qWarning() << tr("LoggerFileBackend::backupLogFile() , removing backup log file '") \
                << pvBackupFilePath << tr(" failed.");
      return;
    }
  }
  if(!QFile::rename(pvFilePath, pvBackupFilePath)){
    qWarning() << tr("LoggerFileBackend::backupLogFile() , backup log file '") \
               << pvFilePath << tr(" failed");
  }
}

}}  // namespace mdt{ namespace error {
