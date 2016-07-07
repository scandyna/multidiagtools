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
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>

namespace Mdt{ namespace ItemEditor{

StandardWindow::StandardWindow(QWidget* parent)
 : QMainWindow(parent),
   pvNavigationActions(new NavigationActions(this))
{
  setupUi(this);
  setupNavigationElements();
}

void StandardWindow::setupNavigationElements()
{
  tlbMain->addAction(pvNavigationActions->toFirst());
  tlbMain->addAction(pvNavigationActions->toPrevious());
  tlbMain->addAction(pvNavigationActions->toNext());
  tlbMain->addAction(pvNavigationActions->toLast());

  auto *navigationMenu = menuBar()->addMenu(tr("&Navigation"));
  navigationMenu->addAction(pvNavigationActions->toFirst());
  navigationMenu->addAction(pvNavigationActions->toPrevious());
  navigationMenu->addAction(pvNavigationActions->toNext());
  navigationMenu->addAction(pvNavigationActions->toLast());
}

}} // namespace Mdt{ namespace ItemEditor{
