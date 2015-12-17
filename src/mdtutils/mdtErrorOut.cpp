/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#include "mdtErrorOut.h"
#include <QDebug>
#include <QMutexLocker>
#include <QDateTime>
#include <QMetaType>
#include <QFile>
#include <QFileInfo>
#include <iostream>
#include <stdio.h>
#include <errno.h>

mdtErrorOut *mdtErrorOut::pvInstance = 0;

mdtErrorOut::mdtErrorOut()
{
  // Default log levels
  pvLogLevelsMask = (mdtError::Info | mdtError::Warning | mdtError::Error);
  pvDialogLevelsMask = (mdtError::Warning | mdtError::Error);
  qRegisterMetaType<mdtError>();
  // Logger
  pvLogger = new mdtErrorOutLogger;
  Q_ASSERT(pvLogger != 0);
  // MessageBox
  ///pvDialog.setWindowFlags(Qt::Dialog);
}

mdtErrorOut::~mdtErrorOut()
{
  delete pvLogger;
}

mdtErrorOut::mdtErrorOut(const mdtErrorOut &orig)
{
}

bool mdtErrorOut::init(const QString &logFile)
{
  // If this is not the first call, we fail
  if(pvInstance != 0){
    qDebug() << "mdtErrorOut::init(): an instance was allready created";
    return false;
  }
  // Create new instance
  pvInstance = new mdtErrorOut;
  // Set logger filename
  if(!instance()->pvLogger->setLogFilePath(logFile)){
    return false;
  }
  // Make connection for the dialog output
  connect(instance(), SIGNAL(sigShowDialog(mdtError)), instance(), SLOT(showDialog(mdtError)));

  return true;
}

void mdtErrorOut::setLogLevelsMask(int mask)
{
  Q_ASSERT(instance() != 0);

  QMutexLocker lock(&mdtErrorOut::instance()->pvMutex);

  instance()->pvLogLevelsMask = mask;
}

void mdtErrorOut::setDialogLevelsMask(int mask)
{
  Q_ASSERT(instance() != 0);

  QMutexLocker lock(&mdtErrorOut::instance()->pvMutex);

  instance()->pvDialogLevelsMask = mask;
}

void mdtErrorOut::addError(mdtError &error)
{
  Q_ASSERT(instance() != 0);

  QMutexLocker lock(&mdtErrorOut::instance()->pvMutex);
  QString lineBegin, data, num;

  // stderr output
  std::cerr << error.functionName().toStdString() << ": error number " << error.number() << " occured:" << std::endl;
  std::cerr << "-> " << error.text().toStdString() << std::endl;
  if(error.systemNumber() != 0){
    std::cerr << "-> System returned error number " << error.systemNumber() << ":" << std::endl;
    std::cerr << "-> " << error.systemText().toStdString() << std::endl;
  }
  if(error.fileLine() > 0){
    std::cerr << "-> File: " << error.fileName().toStdString() << std::endl;
    std::cerr << "-> Line: " << error.fileLine() << std::endl;
  }
  // Log output
  if(error.level() & instance()->pvLogLevelsMask){
    // Set the timestamp
    lineBegin = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss");
    lineBegin += " ";
    // Set the level
    switch(error.level()){
      case mdtError::Info:
        lineBegin += "[Info]";
        break;
      case mdtError::Warning:
        lineBegin += "[Warning]";
        break;
      case mdtError::Error:
        lineBegin += "[Error]";
        break;
      default:
        lineBegin += "[Unknow]";
    }
    lineBegin += ": ";
    // Build log lines
    data = lineBegin;
    data += error.functionName();
    data += ": error number ";
    data += num.setNum(error.number());
    data += " occured:\n";
    data += lineBegin;
    data += "-> ";
    data += error.text();
    data += "\n";
    if(error.systemNumber() != 0){
      data += lineBegin;
      data += "-> System returned error number ";
      data += num.setNum(error.systemNumber());
      data += ":\n";
      data += lineBegin;
      data += "-> ";
      data += error.systemText();
      data += "\n";
    }
    if(error.fileLine() != 0){
      data += lineBegin;
      data += "-> File: ";
      data += error.fileName();
      data += "\n";
      data += lineBegin;
      data += "-> Line: ";
      data += num.setNum(error.fileLine());
      data += "\n";
    }
    // Send line to logger
    instance()->pvLogger->putData(data);
  }
  // Dialog output
  if(error.level() & instance()->pvDialogLevelsMask){
    emit instance()->sigShowDialog(error);
  }
}

mdtErrorOut *mdtErrorOut::instance()
{
  return pvInstance;
}

QString mdtErrorOut::logFile()
{
  Q_ASSERT(instance() != 0);

  QMutexLocker lock(&mdtErrorOut::instance()->pvMutex);

  return instance()->pvLogger->logFilePath();
}

void mdtErrorOut::destroy()
{
  if(pvInstance != 0){
    delete pvInstance;
    pvInstance = 0;
  }
}

void mdtErrorOut::setLogFileMaxSize(qint64 maxSize)
{
  Q_ASSERT(instance() != 0);

  QMutexLocker lock(&mdtErrorOut::instance()->pvMutex);
  instance()->pvLogger->setLogFileMaxSize(maxSize);
}

void mdtErrorOut::showDialog(mdtError error)
{
  QString msg, num;

  // Set base text
  msg = tr("Error number ");
  num.setNum(error.number());
  msg += num;
  msg += tr(" occured:\n");
  msg += error.text();
  instance()->pvDialog.setText(msg);
  // Set system returned details
  if(error.systemNumber() != 0){
    msg = tr("System returned error number ");
    num.setNum(error.systemNumber());
    msg += num;
    msg += tr(" :\n");
    msg += error.systemText();
    instance()->pvDialog.setInformativeText(msg);
  }
  // Set the source
  if(error.fileLine() > 0){
    msg = tr("Function: ");
    msg += error.functionName();
    msg += tr("\nFile: ");
    msg += error.fileName();
    msg += tr("\nLine: ");
    num.setNum(error.fileLine());
    msg += num;
    instance()->pvDialog.setDetailedText(msg);
  }
  // Set the icon
  ///instance()->pvDialog.setIcon(error.levelIcon());
  /**
  switch(error.level()){
    case mdtError::Info:
      instance()->pvDialog.setIcon(QMessageBox::Information);
      break;
    case mdtError::Warning:
      instance()->pvDialog.setIcon(QMessageBox::Warning);
      break;
    case mdtError::Error:
      instance()->pvDialog.setIcon(QMessageBox::Critical);
      break;
    default:
      instance()->pvDialog.setIcon(QMessageBox::NoIcon);
  }
  */
  instance()->pvDialog.exec();
}

/*
 * ** Logger class **
 */

/*
 * Note: 2012.07.12
 * Had a strange problem by using QFile in this class, so I rewrote this part with ANCI C calls (fopen, fclose, ...)
 * On Linux, all worked Ok.
 * On Wine, with MinGW, the symtom was:
 *  - Write a error (open, write, close): Ok
 *  - Write a new error: (same calls): Nok, the first error was overwritten !?
 *  - Write a new error: (same calls): Ok, second error was keeped, and new appended !?
 * After replacing QFile by C calls, all worked fine.
 * Platform: Wine, MinGW32, Qt 4.8.2
 */

mdtErrorOutLogger::mdtErrorOutLogger(QObject *parent)
 : QThread(parent)
{
  pvMutex = new QMutex;
  Q_ASSERT(pvMutex != 0);
  // Set the maximum log file size before backup
  pvLogFileMaxSize = 20000000;
}

mdtErrorOutLogger::~mdtErrorOutLogger()
{
  wait();
  delete pvMutex;
  // Delete log file if it is empty
  QFileInfo fileInfo(pvLogFilePath);
  if(fileInfo.exists()){
    if(fileInfo.size() == 0){
      if(!QFile::remove(fileInfo.absoluteFilePath())){
        qDebug() << "mdtErrorOutLogger::~mdtErrorOutLogger(): cannot remove log file " << pvLogFilePath;
      }
    }
  }
}

bool mdtErrorOutLogger::setLogFilePath(const QString &path)
{
  FILE *f;

  pvMutex->lock();
  pvLogFilePath = path;
  // Check if file can be open for write
  f = fopen(pvLogFilePath.toStdString().c_str(), "a");
  if(f == 0){
    qDebug() << "mdtErrorOutLogger::setLogFilePath(): unabble to open log file '" << pvLogFilePath << "'";
    qDebug() << "-> System returned error : " << errno << strerror(errno);
    pvMutex->unlock();
    return false;
  }
  // Write ok, close and return
  if(fclose(f) != 0){
    qDebug() << "mdtErrorOutLogger::setLogFilePath(): fclose call failed";
    qDebug() << "-> System returned error : " << errno << strerror(errno);
    pvMutex->unlock();
    return false;
  }
  pvMutex->unlock();

  return true;
}

QString mdtErrorOutLogger::logFilePath()
{
  QString path;

  pvMutex->lock();
  path = pvLogFilePath;
  pvMutex->unlock();

  return path;
}

void mdtErrorOutLogger::putData(QString data)
{
  pvMutex->lock();
  pvDataToWrite << data;
  pvMutex->unlock();

  if(!isRunning()){
    start();
  }
}

void mdtErrorOutLogger::setLogFileMaxSize(qint64 maxSize)
{
  pvMutex->lock();
  pvLogFileMaxSize = maxSize;
  pvMutex->unlock();
}

void mdtErrorOutLogger::run()
{
  int dataToWriteCount;
  QString data;
  qint64 logFileSize;
  qint64 logFileMaxSize;
  QString logFilePath;
  QString backupName;
  FILE *logFile;
  qint64 written;

  while(1){
    // Check how many lines are to write and get a line
    pvMutex->lock();
    dataToWriteCount = pvDataToWrite.size();
    logFileMaxSize = pvLogFileMaxSize;
    if(dataToWriteCount > 0){
      data = pvDataToWrite.takeFirst();
    }
    logFilePath = pvLogFilePath;
    pvMutex->unlock();
    if(dataToWriteCount < 1){
      break;
    }
    // Write the data
    logFileSize = 0;
    logFile = fopen(logFilePath.toStdString().c_str(), "a");
    if(logFile == 0){
      qDebug() << "mdtErrorOutLogger::run(): unabble to open log file '" << logFilePath << "'";
      qDebug() << "-> System returned error : " << errno << strerror(errno);
      break;
    }
    written = fwrite(data.toStdString().c_str(), 1, data.size(), logFile);
    if(written < data.size()){
      qDebug() << "mdtErrorOutLogger::run(): fwrite could not write all data";
    }
    if(fflush(logFile) < 0){
      qDebug() << "mdtErrorOutLogger::run(): ffluh call failed";
      qDebug() << "-> System returned error : " << errno << strerror(errno);
    }
    // Get log file size
    rewind(logFile);
    if(fseek(logFile, 0L, SEEK_END) != 0){
      qDebug() << "mdtErrorOutLogger::run(): fseek call failed";
      qDebug() << "-> System returned error : " << errno << strerror(errno);
    }else{
      logFileSize = ftell(logFile);
      if(logFileSize < 0){
        qDebug() << "mdtErrorOutLogger::run(): ftell call failed";
        qDebug() << "-> System returned error : " << errno << strerror(errno);
        logFileSize = 0;
      }
    }
    if(fclose(logFile) < 0){
      qDebug() << "mdtErrorOutLogger::run(): fclose call failed";
      qDebug() << "-> System returned error : " << errno << strerror(errno);
    }
    // Make backups
    if(logFileSize > logFileMaxSize){
      // We make it simple, with just 1 backup file
      backupName = logFilePath;
      backupName += ".bak";
      // Check if backup exists, and remove if true
      logFile = fopen(backupName.toStdString().c_str(), "r");
      if(logFile != 0){
        if(remove(backupName.toStdString().c_str()) != 0){
          qDebug() << "mdtErrorOutLogger::run(): remove call failed";
          qDebug() << "-> System returned error : " << errno << strerror(errno);
          fclose(logFile);
          break;
        }
        if(fclose(logFile) < 0){
          qDebug() << "mdtErrorOutLogger::run(): fclose call failed";
          qDebug() << "-> System returned error : " << errno << strerror(errno);
        }
      }
      // Rename log file to backup name
      if(rename(logFilePath.toStdString().c_str(), backupName.toStdString().c_str()) != 0){
        qDebug() << "mdtErrorOutLogger::run(): rename call failed";
        qDebug() << "-> System returned error : " << errno << strerror(errno);
        break;
      }
    }
  }
}
