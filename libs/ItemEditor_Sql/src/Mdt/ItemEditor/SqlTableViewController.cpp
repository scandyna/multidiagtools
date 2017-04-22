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
#include "SqlTableViewController.h"
#include "Mdt/ItemEditor/TableViewControllerImplementation.h"
#include "Mdt/ItemEditor/TableViewControllerStatePermission.h"
#include "Mdt/ItemEditor/TableViewControllerStateTable.h"
#include "Mdt/ItemEditor/ControllerStateMachine.h"
#include <QTableView>
#include <QSqlTableModel>

// #include <QDebug>

namespace Mdt{ namespace ItemEditor{

SqlTableViewController::SqlTableViewController(QObject* parent)
 : AbstractSqlTableModelController(parent),
   mImpl( std::make_unique<TableViewControllerImplementation>() )
{
  setControllerStateMachine( ControllerStateMachine::makeNew<TableViewControllerStateTable, TableViewControllerStatePermission>(this) );
  controllerStateMachine()->setModelHasCache(true);
  connect(mImpl->proxyItemDelegate(), &EventCatchItemDelegate::dataEditionStarted, this, &SqlTableViewController::onDataEditionStarted);
  connect(mImpl->proxyItemDelegate(), &EventCatchItemDelegate::dataEditionDone, this, &SqlTableViewController::onDataEditionDone);
}

SqlTableViewController::~SqlTableViewController()
{
}

void SqlTableViewController::setView(QTableView* view)
{
  Q_ASSERT(view != nullptr);

  mImpl->setView(view);
  if( mImpl->connectToController(this) ){
    modelSetToView();
  }
}

QTableView* SqlTableViewController::view() const
{
  return mImpl->view();
}

void SqlTableViewController::setMultiRowSelectionAllowed(bool allow)
{
  mImpl->setMultiRowSelectionAllowed(allow);
}

void SqlTableViewController::setModelToView(QAbstractItemModel* model)
{
  mImpl->setModel(model);
  if( mImpl->connectToController(this) ){
    modelSetToView();
  }
}

bool SqlTableViewController::setDataToModel()
{
  return mImpl->setDataToModel();
}

void SqlTableViewController::revertDataFromModel()
{
  mImpl->revertDataFromModel();
}

ItemModel::RowList SqlTableViewController::getSelectedRows() const
{
  return mImpl->getSelectedRows();
}

}} // namespace Mdt{ namespace ItemEditor{
