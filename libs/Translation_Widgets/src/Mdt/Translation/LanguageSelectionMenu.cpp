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
#include "LanguageSelectionMenu.h"
#include "Mdt/Translation/LanguageCode.h"

#include <QAction>
#include <QActionGroup>

namespace Mdt{ namespace Translation{

LanguageSelectionMenu::LanguageSelectionMenu(QWidget* parent)
 : LanguageSelectionMenu(tr("&Language"), parent)
{
}

LanguageSelectionMenu::LanguageSelectionMenu(const QString& title, QWidget* parent)
 : QMenu(title, parent)
{
//   auto *frAct = new QAction(tr("Frensh"), this);
//   auto *deAct = new QAction(tr("German"), this);
//   auto *enAct = new QAction(tr("English"), this);
//   auto *actionGroup = new QActionGroup(this);
//   actionGroup->setExclusive(true);
//   frAct->setCheckable(true);
//   deAct->setCheckable(true);
//   enAct->setCheckable(true);
//   actionGroup->addAction(frAct);
//   actionGroup->addAction(deAct);
//   actionGroup->addAction(enAct);
//   addAction(frAct);
//   addAction(deAct);
//   addAction(enAct);
}

void LanguageSelectionMenu::setAvailableLanguages(const LanguageSelectionActionList & actionList)
{
  addActions(actionList.actions());
}

}} // namespace Mdt{ namespace Translation{
