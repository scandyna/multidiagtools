/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "LanguageCode.h"
#include <QString>
#include <QChar>
#include <cstring>

namespace Mdt{ namespace Translation{

LanguageCode::LanguageCode() noexcept
 : mCode{'\0', '\0'}
{
}

LanguageCode::LanguageCode(const char * const code)
 : mCode{code[0], code[1]}
{
  Q_ASSERT(std::strlen(code) == 2);
  Q_ASSERT(QChar::fromLatin1(code[0]).isLetter());
  Q_ASSERT(QChar::fromLatin1(code[1]).isLetter());
  Q_ASSERT(QChar::fromLatin1(code[0]).isLower());
  Q_ASSERT(QChar::fromLatin1(code[1]).isLower());
}

QString LanguageCode::toString() const
{
  if(isNull()){
    return QString();
  }
  Q_ASSERT(mCode.size() == 2);
  return QString::fromLatin1(mCode.data(), 2);
}

LanguageCode LanguageCode::fromString(const QString& code)
{
  Q_ASSERT(code.length() == 2);

  LanguageCode lc;
  lc.mCode[0] = code.at(0).toLatin1();
  lc.mCode[1] = code.at(1).toLatin1();

  return lc;
}

}} // namespace Mdt{ namespace Translation{
