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
#include "LanguageSelection.h"
#include "Mdt/Translation/FindTranslation.h"
#include "Mdt/Expected.h"
#include <QCoreApplication>
#include <QLatin1String>
#include <QString>
#include <QVariant>
#include <QStringBuilder>
#include <QDir>
#include <QMenu>
#include <QToolBar>
#include <QAction>
#include <QActionGroup>
#include <QLocale>
#include <QTranslator>
#include <QEvent>
#include <QDebug>

using namespace Mdt::FileSystem;

namespace Mdt{ namespace Translation{

LanguageSelection::LanguageSelection(QObject* parent)
 : QObject(parent)
{
}

LanguageSelection::~LanguageSelection()
{
}

bool LanguageSelection::findTranslations()
{
  return findTranslations( PathList{QCoreApplication::applicationDirPath()} );
}

bool LanguageSelection::findTranslations(const PathList & pathPrefixList)
{
  /*
   * If actions have allready be created, we fail.
   * To support updating available translations,
   * we have to take care about actions allready attached to a menu,
   * a tool bar, etc..
   */
  if(!mLanguageSelectionActions.isEmpty()){
    const auto msg = tr("Finding translations cannot be done, because language selection actions have allready been created.");
    auto error = mdtErrorNewQ(msg, Mdt::Error::Warning, this);
    setLastError(error);
    return false;
  }

  const auto translations = Mdt::Translation::findTranslations(pathPrefixList);
  if(!translations){
    setLastError(translations.error());
    return false;
  }
  mTranslations = *translations;

  return true;
}

void LanguageSelection::addAvailableLanguagesToMenu(QMenu* menu)
{
  Q_ASSERT(menu != nullptr);

  if(mLanguageSelectionActions.isEmpty()){
    createLanguageSelectionActions(menu);
  }
  menu->addActions(mLanguageSelectionActions);
}

void LanguageSelection::addAvailableLanguagesToToolBar(QToolBar* toolBar)
{
  Q_ASSERT(toolBar != nullptr);

  if(mLanguageSelectionActions.isEmpty()){
    createLanguageSelectionActions(toolBar);
  }
  toolBar->addActions(mLanguageSelectionActions);
}

void LanguageSelection::createLanguageSelectionActions(QObject* parent)
{
  Q_ASSERT(mLanguageSelectionActions.isEmpty());

  const auto currentLanguageCode = LanguageCode::fromLanguageCountry( QLocale::system().name() );
  const auto availableLanguageCodes = mTranslations.toLanguageCodeList();
  auto *actionGroup = new QActionGroup(parent);
  actionGroup->setExclusive(true);
  connect( actionGroup, &QActionGroup::triggered, this, &LanguageSelection::onLanguageSelected );
  for(const auto & languageCode : availableLanguageCodes){
    const auto locale = QLocale(languageCode.toString());
    const auto language = QLocale::languageToString( locale.language() );
    auto *action = new QAction(language, parent);
    action->setCheckable(true);
    action->setData(languageCode.toString());
    actionGroup->addAction(action);
    if(languageCode == currentLanguageCode){
      action->setChecked(true);
      loadLanguage(languageCode);
    }
    mLanguageSelectionActions.append(action);
  }
}

void LanguageSelection::handleChangeEvent(QEvent* event)
{
  if(event == nullptr){
    return;
  }
  if(event->type() == QEvent::LocaleChange){
    const auto languageCode = LanguageCode::fromLanguageCountry( QLocale::system().name() );
    loadLanguage(languageCode);
  }
}

void LanguageSelection::onLanguageSelected(QAction* action)
{
  Q_ASSERT(action != nullptr);

  const auto languageCode = LanguageCode::fromString( action->data().toString() );
  loadLanguage(languageCode);
}

bool LanguageSelection::loadLanguage(const LanguageCode& languageCode)
{
  if(languageCode == mCurrentLanguage){
    return true;
  }

  const auto translations = mTranslations.getTranslationsForFileLanguageCode(languageCode);

  if(!loadTranslations(translations)){
    return false;
  }
  mCurrentLanguage = languageCode;

  return true;
}

bool LanguageSelection::loadTranslations(const TranslationInfoList& translations)
{
  unloadTranslations();
  resizeTranslatorsList(translations.count());
  for(int i=0; i < translations.count(); ++i){
    if( mTranslators[i]->load(translations.at(i).absoluteFilePath()) ){
      QCoreApplication::installTranslator(mTranslators[i].get());
    }else{
      const auto msg = tr("Could not load translation file '%1'.")
                       .arg(translations.at(i).fullFileName());
      auto error = mdtErrorNewQ(msg, Mdt::Error::Warning, this);
      setLastError(error);
      qWarning() << QLatin1String("Mdt::Translation::LanguageSelection::loadTranslations(): ") << msg;
      //error.commit(); Error logger will crash (because logging is not enabled, have to fix it in Mdt::Error)
    }
  }

  return true;
}

void LanguageSelection::unloadTranslations()
{
  for(const auto & translator : mTranslators){
    QCoreApplication::removeTranslator(translator.get());
  }
}

void LanguageSelection::resizeTranslatorsList(int size)
{
  if( (int)mTranslators.size() == size ){
    return;
  }
  mTranslators.clear();
  for(int i = 0; i < size; ++i){
    mTranslators.emplace_back( std::make_unique<QTranslator>() );
  }
}

void LanguageSelection::setLastError(const Mdt::Error& error)
{
  mLastError = error;
}

}} // namespace Mdt{ namespace Translation{
