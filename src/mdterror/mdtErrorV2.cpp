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
#include "mdtErrorV2.h"
#include <algorithm>
#include <iterator>

#include <QDebug>

mdtErrorV2::mdtErrorV2()
{
}

void mdtErrorV2::clear()
{
  pvShared.reset();
}

void mdtErrorV2::setInformativeText(const QString& text)
{
  Q_ASSERT(!isNull());

  pvShared.detach();
  pvShared->informativeText = text;
}

QString mdtErrorV2::informativeText() const
{
  if(!pvShared){
    return QString();
  }
  return pvShared->informativeText;
}

mdtErrorV2::Level mdtErrorV2::level() const
{
  if(!pvShared){
    return NoError;
  }
  return static_cast<Level>(pvShared->level);
}

QString mdtErrorV2::text() const
{
  if(!pvShared){
    return QString();
  }
  return pvShared->text;
}

void mdtErrorV2::stackError(const mdtErrorV2 & error)
{
  Q_ASSERT(!isNull());
  Q_ASSERT(!error.isNull());
//   if(!pvShared){
//     initShared<mdtGenericError>(mdtGenericError());
//   }
  /*
   * We push errors back (as if it was a queue),
   * Then, when getting them back, we do is reverse order
   */
  Q_ASSERT(pvShared);
  Q_ASSERT(error.pvShared);
  pvShared.detach();
  // Copy given error's stack if available
  auto first = error.pvShared->pvErrorStack.cbegin();
  auto last = error.pvShared->pvErrorStack.cend();
  std::copy(first, last, std::back_inserter(pvShared->pvErrorStack));
  // Push error itself at end
  pvShared->pvErrorStack.push_back(error);
}

std::vector<mdtErrorV2> mdtErrorV2::getErrorStack() const
{
  std::vector<mdtErrorV2> stack;

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
