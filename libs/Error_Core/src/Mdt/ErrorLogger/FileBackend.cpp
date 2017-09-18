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
#include "FileBackend.h"
#include <QFile>
#include <QLatin1String>
#include <QStringBuilder>
#include <QDebug>
#include <QDebugStateSaver>
#include <QDateTime>
#include <QTextStream>

namespace Mdt{ namespace ErrorLogger {

FileBackend::FileBackend()
 : pvMaxFileSize(1024*1024)
{
}

FileBackend::~FileBackend()
{
}

bool FileBackend::setLogFilePath(const QString & path, qint64 maxFileSize)
{
  // Store infos
  pvMaxFileSize = maxFileSize;
  pvFilePath = path;
  pvBackupFilePath = path + QLatin1String(".save");
  // Try to open the file to see if path is valid and writeable
  QFile file(pvFilePath);
  if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
    qDebug() << tr("FileBackend::setLogFilePath() : could not open file ") << pvFilePath \
             << tr(" , error: ") << file.errorString();
    return false;
  }
  file.close();

  return true;
}

QString FileBackend::logFilePath() const
{
  return pvFilePath;
}

QString FileBackend::backupLogFilePath() const
{
  return pvBackupFilePath;
}

qint64 FileBackend::maxFileSize() const
{
  return pvMaxFileSize;
}

void FileBackend::logError(const Error & error)
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
  // Write error to file
  QTextStream stream(&file);
  stream << getErrorStackString(error) << getErrorString(error) << QStringLiteral("\n");
  stream.flush();
  file.close();
}

bool FileBackend::openFile(QFile & file)
{
  Q_ASSERT(!file.isOpen());
  Q_ASSERT(!file.fileName().isEmpty());

  if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
    qWarning() << tr("FileBackend::setLogFilePath() : could not open file ") << pvFilePath \
             << tr(" , error: ") << file.errorString();
    return false;
  }
  return true;
}

QString FileBackend::getErrorString(const Error & error) const
{
  QString str;
  QString informativeText;
  QString lineBegin;

  // Prepare line beginning
  lineBegin = QDateTime::currentDateTime().toString(QLatin1String("yyyy.MM.dd hh:mm:ss")) + QStringLiteral(" ") \
            % errorLevelText(error.level()) % QStringLiteral(": ");
  // Build message
  str = lineBegin % QStringLiteral("In ") % error.functionName() % QStringLiteral("\n") \
      % lineBegin + QStringLiteral("-> ") % tr("Text: ") % error.text() % QStringLiteral("\n");
  informativeText = error.informativeText();
  if(!informativeText.isEmpty()){
    str += lineBegin % QStringLiteral("-> ") % error.informativeText() % QStringLiteral("\n");
  }
  str += lineBegin % QStringLiteral("-> ") % tr("File: ") % error.fileName() % QStringLiteral("\n") \
       % lineBegin % QStringLiteral("-> ") % tr("Line: ") % QString::number(error.fileLine()); /// % QStringLiteral("\n");

  return str;
}

QString FileBackend::getErrorStackString(const Error & error) const
{
  QString str;
  const auto errorStack = error.getErrorStack();

  for(const auto & e : errorStack){
    /// \todo check if allready commited
    str += getErrorString(e) % QStringLiteral("\n");
  }

  return str;
}

QString FileBackend::errorLevelText(Error::Level level) const
{
  switch(level){
    case Error::NoError:
      return QStringLiteral("[NoError]");
    case Error::Critical:
      return QStringLiteral("[Critical]");
    case Error::Warning:
      return QStringLiteral("[Warning]");
    case Error::Info:
      return QStringLiteral("[Info]");
  }
  return QString();
}

void FileBackend::backupLogFile()
{
  if(QFile::exists(pvBackupFilePath)){
    if(!QFile::remove(pvBackupFilePath)){
      qWarning() << tr("FileBackend::backupLogFile() , removing backup log file '") \
                << pvBackupFilePath << tr(" failed.");
      return;
    }
  }
  if(!QFile::rename(pvFilePath, pvBackupFilePath)){
    qWarning() << tr("FileBackend::backupLogFile() , backup log file '") \
               << pvFilePath << tr(" failed");
  }
}

}}  // namespace Mdt{ namespace ErrorLogger {
