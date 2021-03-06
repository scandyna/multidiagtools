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
#include "TranslationInfoList.h"

namespace Mdt{ namespace Translation{

void TranslationInfoList::addTranslation(const TranslationInfo & translation)
{
  if(mList.contains(translation)){
    return;
  }
  mList.append(translation);
}

void TranslationInfoList::addTranslations(const TranslationInfoList& translations)
{
  for(const auto & translation : translations){
    addTranslation(translation);
  }
}

QStringList TranslationInfoList::getUsedFileSuffixes() const
{
  QStringList suffixes;

  const auto & til = mList;
  for(const auto & ti : til){
    if(!suffixes.contains(ti.fileSuffix(), Qt::CaseSensitive)){
      suffixes.append(ti.fileSuffix());
    }
  }

  return suffixes;
}

TranslationInfoList TranslationInfoList::getTranslationsForFileSuffix(const QString& fileSuffix) const
{
  Q_ASSERT(!fileSuffix.isEmpty());

  TranslationInfoList tilForSuffix;

  const auto & til = mList;
  for(const auto & ti : til){
    if(ti.fileSuffix() == fileSuffix){
      tilForSuffix.addTranslation(ti);
    }
  }

  return tilForSuffix;
}

TranslationInfoList TranslationInfoList::getTranslationsForFileSuffixes(const QStringList& fileSuffixes) const
{
  Q_ASSERT(!fileSuffixes.isEmpty());

  TranslationInfoList tilForSuffixes;

  const auto & til = mList;
  for(const auto & ti : til){
    if(fileSuffixes.contains(ti.fileSuffix(), Qt::CaseSensitive)){
      tilForSuffixes.addTranslation(ti);
    }
  }

  return tilForSuffixes;
}

TranslationInfoList TranslationInfoList::getTranslationsForFileLanguageCode(LanguageCode languageCode) const
{
  TranslationInfoList tilForLanguageCode;

  const auto & til = mList;
  for(const auto & ti : til){
    if(ti.languageCode() == languageCode){
      tilForLanguageCode.addTranslation(ti);
    }
  }

  return tilForLanguageCode;
}

QStringList TranslationInfoList::toQmFilePathList() const
{
  QStringList qmFiles;

  for(const auto & ti : mList){
    qmFiles.append(ti.absoluteFilePath());
  }

  return qmFiles;
}

LanguageCodeList TranslationInfoList::toLanguageCodeList() const
{
  LanguageCodeList codes;

  for(const auto & ti : mList){
    codes.addLanguageCode(ti.languageCode());
  }

  return codes;
}

TranslationInfoList TranslationInfoList::fromQmFilePathList(const QStringList & qmFilePathList)
{
  TranslationInfoList list;

  for(const auto filePath : qmFilePathList){
    list.addTranslation( TranslationInfo::fromQmFilePath(filePath) );
  }

  return list;
}

}} // namespace Mdt{ namespace Translation{
