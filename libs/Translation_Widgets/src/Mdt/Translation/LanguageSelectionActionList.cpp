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
#include "LanguageSelectionActionList.h"
#include "Mdt/Translation/LanguageCode.h"
#include <QAction>
#include <QActionGroup>
#include <QLocale>

#include <QDebug>

namespace Mdt{ namespace Translation{

void LanguageSelectionActionList::createActions(const LanguageCodeList& languages, QObject* parent)
{
  Q_ASSERT(mActions.isEmpty());

  const auto currentLanguageCode = LanguageCode::fromLanguageCountry( QLocale::system().name() );
  qDebug() << "Current lang: " << QLocale::system().name() << " , " << currentLanguageCode.toString();
  auto *actionGroup = new QActionGroup(parent);
  actionGroup->setExclusive(true);
  for(const auto & languageCode : languages){
    const auto locale = QLocale(languageCode.toString());
    const auto language = QLocale::languageToString( locale.language() );
    qDebug() << "Lang: " << languageCode.toString() << " , " << language;
    auto *action = new QAction(language, parent);
    action->setCheckable(true);
    if(languageCode == currentLanguageCode){
      action->setChecked(true);
    }
    mActions.append(action);
    actionGroup->addAction(action);
  }
}

}} // namespace Mdt{ namespace Translation{
