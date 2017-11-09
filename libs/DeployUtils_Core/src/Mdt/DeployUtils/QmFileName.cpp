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
#include "QmFileName.h"
#include <QStringBuilder>
#include <QLatin1String>
#include <QList>
#include <QFileInfo>

// #include <QDebug>

namespace Mdt{ namespace DeployUtils{

QmFileName::QmFileName(const QString & fullName)
{
  mFullName = fullName;
  const auto fullBaseName = QFileInfo(fullName).baseName();
  const auto items = fullBaseName.split('_', QString::SkipEmptyParts);

  if(items.isEmpty()){
    return;
  }
  if(items.size() == 1){
    // We have only base name
    mBaseName = fullBaseName;
  }else if(items.size() == 2){
    // We have maybe base name and language
    if( isLanguageCode(items.at(1)) ){
      mBaseName = items.at(0);
      mLanguageSuffix = items.at(1);
    }else{
      mBaseName = fullBaseName;
    }
  }else{
    Q_ASSERT(items.size() >= 3);
    /*
     * We can have:
     *  - base
     *  - base language
     *  - base language country
     * Note: base can be some_base_name
     */
    const auto lastIndex = items.size() - 1;
    if( isCountryCode(items.at(lastIndex)) && isLanguageCode(items.at(lastIndex-1)) ){
      setBaseName(items, lastIndex-2);
      mLanguageSuffix = items.at(lastIndex-1);
      mCountrySuffix = items.at(lastIndex);
    }else if( isLanguageCode(items.at(lastIndex)) ){
      mLanguageSuffix = items.at(lastIndex);
      setBaseName(items, lastIndex-1);
    }else{
      mBaseName = fullBaseName;
    }
  }
}

QString QmFileName::suffix() const
{
  if( !mLanguageSuffix.isEmpty() && !mCountrySuffix.isEmpty() ){
    return mLanguageSuffix % QLatin1String("_") % mCountrySuffix;
  }else if( mCountrySuffix.isEmpty() ){
    return mLanguageSuffix;
  }else if( mLanguageSuffix.isEmpty() ){
    return mCountrySuffix;
  }else{
    return QString();
  }
}

bool QmFileName::isLanguageCode(const QString & str)
{
  return (str.length() == 2);
}

bool QmFileName::isCountryCode(const QString & str)
{
  return (str.length() == 2);
}

void QmFileName::setBaseName(const QStringList & items, int indexOfLastItem)
{
  Q_ASSERT(indexOfLastItem >= 0);
  Q_ASSERT(indexOfLastItem < items.size());

  for(int i = 0; i < indexOfLastItem; ++i){
    mBaseName += items.at(i) % QLatin1String("_");
  }
  mBaseName += items.at(indexOfLastItem);
}

}} // namespace Mdt{ namespace DeployUtils{
