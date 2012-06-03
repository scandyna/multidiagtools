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
#include "mdtError.h"
#include "mdtErrorOut.h"
#ifdef Q_OS_WIN
 #include <windows.h>
#endif

#include <QDebug>

mdtError::mdtError()
{
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

void mdtError::setSource(const QString &fileName, int fileLine, const QString &className, const QString &functionName)
{
  pvFileName = fileName;
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

int mdtError::number()
{
  return pvNumber;
}

QString mdtError::text()
{
  return pvText;
}

mdtError::level_t mdtError::level()
{
  return pvLevel;
}

int mdtError::systemNumber()
{
  return pvSystemNumber;
}

QString mdtError::systemText()
{
  return pvSystemText;
}

QString mdtError::functionName()
{
  return pvFunctionName;
}

QString mdtError::fileName()
{
  return pvFileName;
}

int mdtError::fileLine()
{
  return pvFileLine;
}

