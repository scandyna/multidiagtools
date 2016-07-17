/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "StandardWindow.h"
#include "NavigationActions.h"
#include "InsertAction.h"
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>

namespace Mdt{ namespace ItemEditor{

StandardWindow::StandardWindow(QWidget* parent)
 : AbstractWindow(parent)
{
  setupUi(this);
  setupNavigationElements();
}

void StandardWindow::setupNavigationElements()
{
  auto *actions = navigationActions();

  tlbMain->addAction(actions->toFirst());
  tlbMain->addAction(actions->toPrevious());
  tlbMain->addAction(actions->toNext());
  tlbMain->addAction(actions->toLast());
  tlbMain->addAction(editionActions()->submitAction());
  tlbMain->addAction(editionActions()->revertAction());
  tlbMain->addAction(insertAction()->insertAction());

  auto *navigationMenu = menuBar()->addMenu(tr("&Navigation"));
  navigationMenu->addAction(actions->toFirst());
  navigationMenu->addAction(actions->toPrevious());
  navigationMenu->addAction(actions->toNext());
  navigationMenu->addAction(actions->toLast());

  auto *editMenu = menuBar()->addMenu(tr("&Edit"));
  editMenu->addAction(editionActions()->submitAction());
  editMenu->addAction(editionActions()->revertAction());
  editMenu->addAction(insertAction()->insertAction());
}

}} // namespace Mdt{ namespace ItemEditor{
