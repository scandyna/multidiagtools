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
#include "mdtQActionEnableStateGuard.h"
#include <QAction>

mdtQActionEnableStateGuard::mdtQActionEnableStateGuard(QAction* action, bool initialState)
 : pvAction(action)
{
  Q_ASSERT(pvAction != nullptr);
  pvAction->setEnabled(initialState);
}

mdtQActionEnableStateGuard::~mdtQActionEnableStateGuard()
{
  Q_ASSERT(pvAction != nullptr);
  pvAction->setEnabled(!pvAction->isEnabled());
}


mdtQActionEnableStateGuardList::~mdtQActionEnableStateGuardList()
{
  for(auto & g : pvGuards){
    Q_ASSERT(g);
    delete g;
  }
  pvGuards.clear();
}

void mdtQActionEnableStateGuardList::append(QAction* action, bool initialState)
{
  mdtQActionEnableStateGuard *guard = new mdtQActionEnableStateGuard(action, initialState);
  pvGuards.push_back(guard);
}
