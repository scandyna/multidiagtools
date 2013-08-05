/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtSqlFormWindow.h"
#include "mdtAbstractSqlWidget.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlWindow.h"

mdtSqlFormWindow::mdtSqlFormWindow(QObject *parent)
 : mdtSqlForm(parent)
{
  pvSqlWindow = new mdtSqlWindow;
  pvSqlWindow->setSqlWidget(mainSqlWidget());
}

mdtSqlFormWindow::~mdtSqlFormWindow()
{
  delete pvSqlWindow;
}

mdtSqlWindow *mdtSqlFormWindow::sqlWindow()
{
  Q_ASSERT(pvSqlWindow != 0);

  return pvSqlWindow;
}

void mdtSqlFormWindow::show()
{
  ///pvSqlWindow->setWindowTitle(mainSqlWidget()->userFriendlyTableName());
  pvSqlWindow->show();
}

void mdtSqlFormWindow::addChildWidget(mdtAbstractSqlWidget *widget)
{
  Q_ASSERT(widget != 0);

  pvSqlWindow->addChildWidget(widget, widget->userFriendlyTableName());
}
