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
#include <QString>
#include <QTextStream>
#include <QMetaType>
#include <iostream>

mdtErrorOut *mdtErrorOut::pvInstance = 0;

mdtErrorOut::mdtErrorOut()
{
  // Default log levels
  pvLogLevelsMask = (mdtError::Warning | mdtError::Error);
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
  instance()->pvDialog.exec();
}

/*
 * ** Logger class **
 */

mdtErrorOutLogger::mdtErrorOutLogger(QObject *parent)
 : QThread(parent)
{
  pvLogFile = new QFile;
  Q_ASSERT(pvLogFile != 0);
  pvMutex = new QMutex;
  Q_ASSERT(pvMutex != 0);
  // Set the maximum log file size before backup
  pvLogFileMaxSize = 20000000;
}

mdtErrorOutLogger::~mdtErrorOutLogger()
{
  wait();
  delete pvLogFile;
  delete pvMutex;
}

bool mdtErrorOutLogger::setLogFilePath(const QString &path)
{
  pvMutex->lock();
  pvLogFile->setFileName(path);
  if(!pvLogFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
    qDebug() << "mdtErrorOutLogger::setLogFilePath(): unabble to create/open log file '" << path << "'";
    pvMutex->unlock();
    return false;
  }
  pvLogFile->close();
  pvMutex->unlock();

  return true;
}

QString mdtErrorOutLogger::logFilePath()
{
  QString path;

  pvMutex->lock();
  path = pvLogFile->fileName();
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
  QString backupName;

  while(1){
    // Check how many lines are to write and get a line
    pvMutex->lock();
    dataToWriteCount = pvDataToWrite.size();
    logFileMaxSize = pvLogFileMaxSize;
    if(dataToWriteCount > 0){
      data = pvDataToWrite.takeFirst();
    }
    pvMutex->unlock();
    if(dataToWriteCount < 1){
      break;
    }
    // Write the line
    logFileSize = 0;
    if(!pvLogFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
      qDebug() << "mdtErrorOutLogger::run(): unabble to open log file '" << pvLogFile->fileName() << "'";
    }else{
      pvLogFile->write(data.toAscii());
      pvLogFile->flush();
      logFileSize = pvLogFile->size();
      pvLogFile->close();
    }
    // Make backups
    if(logFileSize > logFileMaxSize){
      // We make it simple, with just 1 backup file
      backupName = pvLogFile->fileName();
      backupName += ".bak";
      if(pvLogFile->exists(backupName)){
          if(!QFile::remove(backupName)){
          qDebug() << "mdtErrorOutLogger::run(): cannot remove backuped logfile '" << backupName << "'";
          break;
        }
      }
      // Rename log file to backup name
      if(!QFile::rename(pvLogFile->fileName(), backupName)){
        qDebug() << "mdtErrorOutLogger::run(): cannot backup logfile to '" << backupName << "'";
        break;
      }
    }
  }
}
