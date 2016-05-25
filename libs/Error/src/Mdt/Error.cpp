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
#include "Error.h"
#include "ErrorLogger/Logger.h"
#include <QLatin1String>
#include <QStringBuilder>
#include <QObject>
#include <QFileInfo>
#include <QMetaObject>
#include <algorithm>
#include <iterator>

//#include <QDebug>

namespace Mdt{

Error::Error()
{
}

void Error::clear()
{
  pvShared.reset();
}

void Error::updateText(const QString & text)
{
  Q_ASSERT(!isNull());

  pvShared.detach();
  pvShared->text = text;
}

void Error::setInformativeText(const QString& text)
{
  Q_ASSERT(!isNull());

  pvShared.detach();
  pvShared->informativeText = text;
}

QString Error::informativeText() const
{
  if(!pvShared){
    return QString();
  }
  return pvShared->informativeText;
}

Error::Level Error::level() const
{
  if(!pvShared){
    return NoError;
  }
  return static_cast<Level>(pvShared->level);
}

QString Error::text() const
{
  if(!pvShared){
    return QString();
  }
  return pvShared->text;
}

void Error::stackError(const Error & error)
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

std::vector<Error> Error::getErrorStack() const
{
  std::vector<Error> stack;

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

void Error::setSource(const QString & fileName, int fileLine, const QString & className, const QString & functionName)
{
  Q_ASSERT(pvShared);

  pvShared.detach();
  pvShared->fileName =  QFileInfo(fileName).fileName();
  pvShared->lineNumber = fileLine;
  QString fn = className;
  if(!functionName.isEmpty()){
    fn += QStringLiteral("::") % functionName % QStringLiteral("()");
  }
  pvShared->functionName = fn;
}

void Error::setSource(const QString& fileName, int fileLine, const QObject*const obj, const QString& functionName)
{
  Q_ASSERT(obj != nullptr);
  Q_ASSERT(obj->metaObject() != nullptr);

  setSource(fileName, fileLine, obj->metaObject()->className(), functionName);
}

void Error::commit()
{
  /// \todo Add a flag to know if allready logged. Logger must also support logging stack.
  Mdt::ErrorLogger::Logger::logError(*this);
}

QString Error::fileName() const
{
  if(!pvShared){
    return QString();
  }
  return pvShared->fileName;
}

int Error::fileLine() const
{
  if(!pvShared){
    return 0;
  }
  return pvShared->lineNumber;
}

QString Error::functionName() const
{
  if(!pvShared){
    return QString();
  }
  return pvShared->functionName;
}

} // namespace Mdt{

/*
 * Clone template specialization:
 * We have a QExplicitlySharedDataPointer<mdtErrorPrivateBase>,
 * but we must return a new mdtErrorPrivate object.
 * This is the reason of this specialization,
 * and also clone() function in mdtErrorPrivateBase and mdtErrorPrivate.
 */
template <>
Mdt::ErrorPrivateBase *QExplicitlySharedDataPointer<Mdt::ErrorPrivateBase>::clone()
{
  return d->clone();
}
