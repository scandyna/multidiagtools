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
#include "SqlClientWidget.h"
#include "Client.h"
#include "Mdt/ItemEditor/SqlTableWidgetMapperController.h"

using namespace Mdt::ItemEditor;

SqlClientWidget::SqlClientWidget(QWidget* parent)
 : AbstractSqlTableFormWidget(parent)
{
  auto *centralWidget = new QWidget;
  setupUi(centralWidget);
  setCentralWidget(centralWidget);
}

bool SqlClientWidget::setup(const QSqlDatabase & db)
{
  auto *ctrl = controller();
  ctrl->clearMapping();
  ctrl->setDefaultModel(db);
  ctrl->addMapping(fld_Id, 0);
  ctrl->addMapping(fld_Name, 1);
  ctrl->setInsertLocation(SqlTableWidgetMapperController::InsertAtEnd);
  ctrl->setTable(Client());
  if(!ctrl->select()){
    return false;
  }

  ///ctrl->setPrimaryKey({0});
//   ctrl->setPrimaryKeyEditable(false);
//   ctrl->setPrimaryKeyItemsEnabled(false);
//   ctrl->setPrimaryKeyHidden(true);
  ///ctrl->setForeignKey({1});
//   ctrl->setForeignKeyEditable(false);
//   ctrl->setForeignKeyItemsEnabled(false);
//   ctrl->setForeignKeyHidden(true);

  return true;
}
