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
#include "SqlClientEditorMainWidget.h"
#include "SqlClientWidget.h"
#include "SqlAddressWidget.h"

using namespace Mdt::ItemEditor;

SqlClientEditorMainWidget::SqlClientEditorMainWidget(QWidget* parent)
 : StandardEditorLayoutWidget(parent)
{
  mClientWidget = new SqlClientWidget;
  mAddressWidget = new SqlAddressWidget;
  setMainWidget(mClientWidget);
  addChildWidget(mAddressWidget, tr("Addresses"));
  mMainController = mClientWidget->controller();
}

bool SqlClientEditorMainWidget::setup(const QSqlDatabase& db)
{
  if(!mClientWidget->setup(db)){
    return false;
  }
  if(!mAddressWidget->setup(db)){
    return false;
  }
  // Setup relation

  return true;
}
