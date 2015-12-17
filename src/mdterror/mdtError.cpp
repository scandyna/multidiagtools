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
#include "mdtError.h"
#include "mdt/error/Logger.h"
#include <QLatin1String>
#include <QObject>
#include <QMetaObject>
#include <algorithm>
#include <iterator>

//#include <QDebug>

mdtError::mdtError()
{
}

void mdtError::clear()
{
  pvShared.reset();
}

void mdtError::updateText(const QString & text)
{
  Q_ASSERT(!isNull());

  pvShared.detach();
  pvShared->text = text;
}

void mdtError::setInformativeText(const QString& text)
{
  Q_ASSERT(!isNull());

  pvShared.detach();
  pvShared->informativeText = text;
}

QString mdtError::informativeText() const
{
  if(!pvShared){
    return QString();
  }
  return pvShared->informativeText;
}

mdtError::Level mdtError::level() const
{
  if(!pvShared){
    return NoError;
  }
  return static_cast<Level>(pvShared->level);
}

QString mdtError::text() const
{
  if(!pvShared){
    return QString();
  }
  return pvShared->text;
}

void mdtError::stackError(const mdtError & error)
{
  Q_ASSERT(pvShared);
  Q_ASSERT(error.pvShared);

  pvShared.detach();
  /*
   * We push errors back (as if it was a queue),
   * Then, when getting them back, we do is reverse order
   */
  // Copy given error's stack if available
  auto first = error.pvShared->pvErrorStack.cbegin();
  auto last = error.pvShared->pvErrorStack.cend();
  std::copy(first, last, std::back_inserter(pvShared->pvErrorStack));
  // Push error itself at end
  pvShared->pvErrorStack.push_back(error);
}

std::vector<mdtError> mdtError::getErrorStack() const
{
  std::vector<mdtError> stack;

  if(!pvShared){
    return stack;
  }
  if(pvShared->pvErrorStack.empty()){
    return stack;
  }
  auto first = pvShared->pvErrorStack.cbegin();
  auto last = pvShared->pvErrorStack.cend();
  std::reverse_copy(first, last, std::back_inserter(stack));

  return stack;
}

void mdtError::setSource(const QString & fileName, int fileLine, const QString & className, const QString & functionName)
{
  Q_ASSERT(pvShared);

  pvShared.detach();
  pvShared->fileName = fileName;
  pvShared->lineNumber = fileLine;
  pvShared->functionName =  className + QLatin1String("::") + functionName + QLatin1String("()");
}

void mdtError::setSource(const QString& fileName, int fileLine, const QObject*const obj, const QString& functionName)
{
  Q_ASSERT(obj != nullptr);
  Q_ASSERT(obj->metaObject() != nullptr);

  setSource(fileName, fileLine, obj->metaObject()->className(), functionName);
}

void mdtError::commit()
{
  /// \todo Add a flag to know if allready logged. Logger must also support logging stack.
  mdt::error::Logger::logError(*this);
}

QString mdtError::fileName() const
{
  if(!pvShared){
    return QString();
  }
  return pvShared->fileName;
}

int mdtError::fileLine() const
{
  if(!pvShared){
    return 0;
  }
  return pvShared->lineNumber;
}

QString mdtError::functionName() const
{
  if(!pvShared){
    return QString();
  }
  return pvShared->functionName;
}

void mdtError::setSystemError(int, const QString&)
{
}

/*
 * Clone template specialization:
 * We have a QExplicitlySharedDataPointer<mdtErrorPrivateBase>,
 * but we must return a new mdtErrorPrivate object.
 * This is the reason of this specialization,
 * and also clone() function in mdtErrorPrivateBase and mdtErrorPrivate.
 */
template <>
mdtErrorPrivateBase *QExplicitlySharedDataPointer<mdtErrorPrivateBase>::clone()
{
  return d->clone();
}
