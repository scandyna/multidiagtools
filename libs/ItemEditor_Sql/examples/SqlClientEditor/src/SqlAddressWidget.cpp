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
#include "SqlAddressWidget.h"
#include "Address.h"
#include <QSqlDatabase>

using namespace Mdt::ItemEditor;

SqlAddressWidget::SqlAddressWidget(QWidget* parent)
 : SqlTableViewWidget(parent)
{
  addResizeToContentsActionToTopBar();
  addNavigationActionsToTopArea();
  addEditionActionsToBottomArea();
  addInsertActionToBottomArea();
  setInsertActionText(tr("Add address"));
  addRemoveActionToBottomArea();
  setRemoveActionText(tr("Remove addresses"));
}

// SqlAddressWidget::SqlAddressWidget(const QSqlDatabase & db)
//  : SqlTableViewWidget(/*db*/)
// {
//   auto ctrl = controller();
//   ctrl->setTable(Address());
//   ctrl->setMultiRowSelectionAllowed(true);
//   ctrl->setInsertLocation(SqlTableViewController::InsertAtEnd);
//   ctrl->select();
// 
//   addResizeToContentsActionToTopBar();
//   addNavigationActionsToTopArea();
//   addEditionActionsToBottomArea();
//   addInsertActionToBottomArea();
//   setInsertActionText(tr("Add address"));
//   addRemoveActionToBottomArea();
//   setRemoveActionText(tr("Remove addresses"));
// 
//   ///ctrl->setPrimaryKey({0});
// //   ctrl->setPrimaryKeyEditable(false);
// //   ctrl->setPrimaryKeyItemsEnabled(false);
// //   ctrl->setPrimaryKeyHidden(true);
//   ///ctrl->setForeignKey({1});
// //   ctrl->setForeignKeyEditable(false);
// //   ctrl->setForeignKeyItemsEnabled(false);
// //   ctrl->setForeignKeyHidden(true);
// }

bool SqlAddressWidget::setup(const QSqlDatabase & db)
{
  auto ctrl = controller();
  ctrl->setDefaultModel(db);
  ctrl->setMultiRowSelectionAllowed(true);
  ctrl->setInsertLocation(SqlTableViewController::InsertAtEnd);
  ctrl->setTable(Address());
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
