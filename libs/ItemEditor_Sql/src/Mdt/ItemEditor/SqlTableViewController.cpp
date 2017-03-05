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
#include "Mdt/ItemEditor/ItemViewPrivateContainer.h"
#include <QSqlTableModel>

namespace Mdt{ namespace ItemEditor{

SqlTableViewController::SqlTableViewController(QObject* parent)
 : AbstractSqlController(parent),
   mContainer(new ItemViewPrivateContainer)
{
}

// SqlTableViewController::SqlTableViewController(QObject* parent, const QSqlDatabase& db)
//  : AbstractSqlController(parent, db),
//    mContainer(new ItemViewPrivateContainer)
// {
// }

// SqlTableViewController::SqlTableViewController(const QSqlDatabase& db)
//  : SqlTableViewController(nullptr, db)
// {
// }

SqlTableViewController::~SqlTableViewController()
{
}

void SqlTableViewController::setView(QAbstractItemView* view)
{
  Q_ASSERT(view != nullptr);

  mContainer->setView(view);
  registerModelAndSelectionModel();
}

QAbstractItemView* SqlTableViewController::view() const
{
  return mContainer->view();
}

void SqlTableViewController::setModelToView(QAbstractItemModel* model)
{
  mContainer->setModel(model);
  registerModelAndSelectionModel();
}

bool SqlTableViewController::submitDataToModel()
{

}

void SqlTableViewController::revertDataFromModel()
{

}

void SqlTableViewController::registerModelAndSelectionModel()
{
  /*
   * Order of signal/slot connections matters here.
   * We must be sure that model is set to the view
   * before it is registered (set to RowChangeEventDispatcher).
   * Not doing so will produces a problem when model resets:
   *  - Controller receives the event and updates current row to 0 (if model contains data)
   *  - Controller updates current index of view
   *  - View will reset (and current will also be lost!)
   */
  if( (mContainer->model() == nullptr) || (mContainer->view() == nullptr) ){
    return;
  }
  disconnect(mContainer->selectionModel(), &ItemSelectionModel::currentRowChangeRequested, this, &SqlTableViewController::setCurrentRow);
  disconnect(this, &SqlTableViewController::currentRowChanged, mContainer->selectionModel(), &ItemSelectionModel::updateCurrentRow);
  connect(mContainer->selectionModel(), &ItemSelectionModel::currentRowChangeRequested, this, &SqlTableViewController::setCurrentRow);
  connect(this, &SqlTableViewController::currentRowChanged, mContainer->selectionModel(), &ItemSelectionModel::updateCurrentRow);
  modelSetToView();
}

}} // namespace Mdt{ namespace ItemEditor{
