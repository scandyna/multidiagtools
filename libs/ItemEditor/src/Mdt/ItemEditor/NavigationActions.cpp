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
#include "NavigationActions.h"
#include <QIcon>

namespace Mdt{ namespace ItemEditor{

NavigationActions::NavigationActions(QObject* parent)
 : QObject(parent),
   pvToFirst(new QAction(QIcon::fromTheme("go-first"), tr("To first"), this)),
   pvToPrevious(new QAction(QIcon::fromTheme("go-previous"), tr("To previous"), this)),
   pvToNext(new QAction(QIcon::fromTheme("go-next"), tr("To next"), this)),
   pvToLast(new QAction(QIcon::fromTheme("go-last"), tr("To last"), this))
{
  pvToFirst->setObjectName("ToFirst");
  pvToPrevious->setObjectName("ToPrevious");
  pvToNext->setObjectName("ToNext");
  pvToLast->setObjectName("ToLast");
  connect(pvToFirst, &QAction::triggered, this, &NavigationActions::toFirstTriggered);
  connect(pvToPrevious, &QAction::triggered, this, &NavigationActions::toPreviousTriggered);
  connect(pvToNext, &QAction::triggered, this, &NavigationActions::toNextTriggered);
  connect(pvToLast, &QAction::triggered, this, &NavigationActions::toLastTriggered);
  updateEnableStates();
}

void NavigationActions::setRowState(RowState rs)
{
  Q_ASSERT(rs.isValid());

  pvRowState = rs;
  updateEnableStates();
}

void NavigationActions::updateEnableStates()
{
  Q_ASSERT(pvRowState.isValid());

  int n = pvRowState.rowCount();
  int row = pvRowState.currentRow();

  pvToFirst->setEnabled( row > 0 );
  pvToPrevious->setEnabled( row > 0 );
  pvToNext->setEnabled( row < (n-1) );
  pvToLast->setEnabled( row < (n-1) );
}

}} // namespace Mdt{ namespace ItemEditor{

