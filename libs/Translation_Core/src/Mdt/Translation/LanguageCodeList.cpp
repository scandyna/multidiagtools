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
#include "LanguageCodeList.h"
#include <QStringList>
#include <algorithm>

namespace Mdt{ namespace Translation{

void LanguageCodeList::addLanguageCode(const LanguageCode& languageCode)
{
  if(containsLanguageCode(languageCode)){
    return;
  }
  mList.push_back(languageCode);
}

bool LanguageCodeList::containsLanguageCode(const LanguageCode& languageCode) const noexcept
{
  return (std::find(mList.cbegin(), mList.cend(), languageCode) != mList.cend());
}

QStringList LanguageCodeList::toStringList() const
{
  QStringList stringList;

  for(const auto & lc : mList){
    stringList.append(lc.toString());
  }

  return stringList;
}

}} // namespace Mdt{ namespace Translation{
