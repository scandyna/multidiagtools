/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "Translation.h"
#include "FileCopier.h"
#include "LconvertWrapper.h"
#include "Console.h"
#include <QFileInfo>
#include <QDir>
#include <QLatin1String>
#include <QStringBuilder>

#include <QDebug>

namespace Mdt{ namespace DeployUtils{

Translation::Translation(QObject* parent)
 : QObject(parent),
   mLconvert(new LconvertWrapper(this))
{
}

bool Translation::joinTranslations(const TranslationInfoList& inTranslations, const QStringList& binaryFiles, const QString& destinationDirectoryPath, const PathList& pathPrefixList)
{
  mLconvert->findQtBinPath(pathPrefixList);
  if(!createDestinationDirectory(destinationDirectoryPath)){
    return false;
  }
  const auto suffixes = inTranslations.getUsedFileSuffixes();
  for(const auto & binaryFile : binaryFiles){
    const QFileInfo bfi(binaryFile);
    if(!joinTranslationsForSuffixes(inTranslations, bfi.baseName(), suffixes, destinationDirectoryPath)){
      return false;
    }
  }
  return true;
}

bool Translation::createDestinationDirectory(const QString& path)
{
  FileCopier cp;

  if(!cp.createDirectory(path)){
    setLastError(cp.lastError());
    return false;
  }

  return true;
}

bool Translation::joinTranslationsForSuffixes(const TranslationInfoList& translations, const QString& baseName, const QStringList& suffixes, const QString& destinationDirectoryPath)
{
  for(const auto & suffix : suffixes){
    const auto outQmFilePath = QDir::cleanPath( destinationDirectoryPath % QLatin1String("/") % baseName % QLatin1String("_") % suffix % QLatin1String(".qm") );
    qDebug() << "GEN " << outQmFilePath;
    if( !joinQmFiles(translations.getTranslationsForFileSuffix(suffix), outQmFilePath) ){
      return false;
    }
  }
  return true;
}

bool Translation::joinQmFiles(const TranslationInfoList & translations, const QString& outFilePath)
{
  Console::info(2) << " Creating " << outFilePath;
  if(!mLconvert->executeJoinQmFiles(translations.toQmFilePathList(), outFilePath)){
    setLastError(mLconvert->lastError());
    return false;
  }
  return true;
}

void Translation::setLastError(const Error & error)
{
  mLastError = error;
}

}} // namespace Mdt{ namespace DeployUtils{
