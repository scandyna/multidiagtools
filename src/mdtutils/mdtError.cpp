/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtError.h"
#include "mdtErrorOut.h"
#ifdef Q_OS_WIN
 #include <windows.h>
#endif
#include <QFileInfo>
#include <QObject>

mdtError::mdtError()
{
  pvLevel = NoError;
  pvType = 0;
}

mdtError::mdtError(int number, const QString &text, mdtError::level_t level)
{
  pvNumber = number;
  pvText = text;
  pvLevel = level;
  pvSystemNumber = 0;
  pvSystemText = "";
  pvFunctionName = "";
  pvFileName = "";
  pvFileLine = 0;
}

mdtError::mdtError(const QString &text, level_t level)
{
  pvNumber = 0;
  pvText = text;
  pvLevel = level;
  pvSystemNumber = 0;
  pvSystemText = "";
  pvFunctionName = "";
  pvFileName = "";
  pvFileLine = 0;
  pvType = 0;
}

void mdtError::setError(const QString &text, level_t level)
{
  clear();
  pvText = text;
  pvLevel = level;
}

void mdtError::updateText(const QString & text)
{
  pvText = text;
}

void mdtError::clear()
{
  pvNumber = 0;
  pvText.clear();
  pvLevel = NoError;
  pvSystemNumber = 0;
  pvSystemText.clear();
  pvFunctionName.clear();
  pvFileName.clear();
  pvFileLine = 0;
  pvType = 0;
}

void mdtError::setSystemError(int number, const QString &text)
{
  pvSystemNumber = number;
  pvSystemText = text;
}

#ifdef Q_OS_WIN
void mdtError::setSystemErrorWinApi()
{
  char *errMsg = 0;

  pvSystemNumber = (int)GetLastError();
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, (DWORD)pvSystemNumber, 0, (LPTSTR)&errMsg, 0, NULL);
  pvSystemText = (char*)errMsg;
  LocalFree(errMsg);
}
#endif

void mdtError::setSource(const QString &filePath, int fileLine, const QString &className, const QString &functionName)
{
  QFileInfo file(filePath);

  pvFileName = file.fileName();
  pvFileLine = fileLine;
  pvFunctionName = className;
  pvFunctionName += "::";
  pvFunctionName += functionName;
  pvFunctionName += "()";
}

void mdtError::commit()
{
  mdtErrorOut::addError(*this);
}

int mdtError::number() const
{
  return pvNumber;
}

QString mdtError::text() const
{
  return pvText;
}

mdtError::level_t mdtError::level() const
{
  return pvLevel;
}

QMessageBox::Icon mdtError::levelIcon() const
{
  switch(pvLevel){
    case NoError:
      return QMessageBox::Information;
    case Info:
      return QMessageBox::Information;
    case Warning:
      return QMessageBox::Warning;
    case Error:
      return QMessageBox::Critical;
  }
  return QMessageBox::NoIcon;
}

int mdtError::systemNumber() const
{
  return pvSystemNumber;
}

QString mdtError::systemText() const
{
  return pvSystemText;
}

QString mdtError::systemErrorString(QObject *obj) const
{
  QString str;
  QString num;

  if(pvSystemText.isEmpty()){
    return str;
  }
  num.setNum(pvSystemNumber);
  if(obj != 0){
    str = obj->tr("System returned error number ") + num + obj->tr(":\n");
  }else{
    str = "System returned error number " + num + ":\n";
  }
  str += pvSystemText;

  return str;
}

void mdtError::setInformativeText(const QString &text)
{
  pvInformativeText = text;
}

QString mdtError::informativeText() const
{
  return pvInformativeText;
}

QString mdtError::functionName() const
{
  return pvFunctionName;
}

QString mdtError::fileName() const
{
  return pvFileName;
}

int mdtError::fileLine() const
{
  return pvFileLine;
}

