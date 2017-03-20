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
#include "NavigationActions.h"
#include <QIcon>

namespace Mdt{ namespace ItemEditor{

NavigationActions::NavigationActions(QObject* parent)
 : AbstractActionContainer(parent),
   mToFirst(new QAction(QIcon::fromTheme("go-first"), tr("To first"), this)),
   mToPrevious(new QAction(QIcon::fromTheme("go-previous"), tr("To previous"), this)),
   mToNext(new QAction(QIcon::fromTheme("go-next"), tr("To next"), this)),
   mToLast(new QAction(QIcon::fromTheme("go-last"), tr("To last"), this))
{
  mToFirst->setObjectName("ToFirst");
  mToPrevious->setObjectName("ToPrevious");
  mToNext->setObjectName("ToNext");
  mToLast->setObjectName("ToLast");
  connect(mToFirst, &QAction::triggered, this, &NavigationActions::toFirstTriggered);
  connect(mToPrevious, &QAction::triggered, this, &NavigationActions::toPreviousTriggered);
  connect(mToNext, &QAction::triggered, this, &NavigationActions::toNextTriggered);
  connect(mToLast, &QAction::triggered, this, &NavigationActions::toLastTriggered);
  mToFirst->setEnabled(false);
  mToPrevious->setEnabled(false);
  mToNext->setEnabled(false);
  mToLast->setEnabled(false);
}

void NavigationActions::updateEnableState()
{
  int n = rowCount();
  int row = currentRow();

  if(!isChangeCurrentRowActionEnabled()){
    disableAllActions();
    return;
  }
  if(rowStateIsValid()){
    mToFirst->setEnabled( row > 0 );
    mToPrevious->setEnabled( row > 0 );
    mToNext->setEnabled( row < (n-1) );
    mToLast->setEnabled( row < (n-1) );
  }else if(row < n){
    Q_ASSERT(row == -1);
    mToFirst->setEnabled(true);
    mToPrevious->setEnabled(false);
    mToNext->setEnabled(true);
    mToLast->setEnabled(true);
  }
}

void NavigationActions::disableAllActions()
{
  mToFirst->setEnabled(false);
  mToPrevious->setEnabled(false);
  mToNext->setEnabled(false);
  mToLast->setEnabled(false);
}

}} // namespace Mdt{ namespace ItemEditor{

