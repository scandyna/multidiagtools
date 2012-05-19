
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
  // MessageBox
  ///pvDialog.setWindowFlags(Qt::Dialog);
}

mdtErrorOut::~mdtErrorOut()
{
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
  // Create/Open the log file
  pvInstance->pvLogFile.setFileName(logFile);
  if(!pvInstance->pvLogFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
    qDebug() << "mdtErrorOut::init(): unabble to create/open log file '" << logFile << "'";
    return false;
  }
  pvInstance->pvLogFile.close();
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
  QString lineBegin;

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
    if(!instance()->pvLogFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
      std::cerr << "mdtErrorOut::addError(): unable to open log file" << std::endl;
    }else{
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
      // Init a stream
      QTextStream log(&instance()->pvLogFile);
      log << lineBegin << error.functionName() << ": error number " << error.number() << " occured:\n";
      log << lineBegin << "-> " << error.text() << "\n";
      if(error.systemNumber() != 0){
        log << lineBegin << "-> System returned error number " << error.systemNumber() << ":\n";
        log << lineBegin << "-> " << error.systemText() << "\n";
      }
      if(error.fileLine() > 0){
        log << lineBegin << "-> File: " << error.fileName() << "\n";
        log << lineBegin << "-> Line: " << error.fileLine() << "\n";
      }
      log.flush();
      instance()->pvLogFile.close();
    }
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
  
  return instance()->pvLogFile.fileName();
}

void mdtErrorOut::destroy()
{
  if(pvInstance != 0){
    delete pvInstance;
    pvInstance = 0;
  }
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
