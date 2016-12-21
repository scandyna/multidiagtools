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
#include "TableViewController.h"
#include "ItemViewPrivateContainer.h"
#include <QAbstractTableModel>

// #include <QDebug>

namespace Mdt{ namespace ItemEditor{

TableViewController::TableViewController(QObject* parent)
 : AbstractController(parent),
   mContainer(new ItemViewPrivateContainer)
{
  connect(mContainer->proxyItemDelegate(), &EventCatchItemDelegate::dataEditionStarted, this, &TableViewController::onDataEditionStarted);
  connect(mContainer->proxyItemDelegate(), &EventCatchItemDelegate::dataEditionDone, this, &TableViewController::onDataEditionDone);
}

TableViewController::~TableViewController()
{
}

void TableViewController::setView(QAbstractItemView* view)
{
  Q_ASSERT(view != nullptr);

  mContainer->setView(view);
  registerModelAndSelectionModel();
}

QAbstractItemView* TableViewController::view() const
{
  return mContainer->view();
}

// void TableViewController::setModel(QAbstractItemModel* model)
// {
//   mContainer->setModel(model);
//   referenceItemModel(model);
//   registerModelAndSelectionModel();
// }

void TableViewController::setModelToView(QAbstractItemModel* model)
{
  mContainer->setModel(model);
  ///referenceItemModel(model);
  registerModelAndSelectionModel();
}

bool TableViewController::submitDataToModel()
{
  mContainer->proxyItemDelegate()->commitCurrentEditorData();
  return true;
}

void TableViewController::revertDataFromModel()
{
  mContainer->proxyItemDelegate()->closeCurrentEditor();
}

void TableViewController::registerModelAndSelectionModel()
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
  disconnect(mContainer->selectionModel(), &ItemSelectionModel::currentRowChangeRequested, this, &TableViewController::setCurrentRow);
  disconnect(this, &TableViewController::currentRowChanged, mContainer->selectionModel(), &ItemSelectionModel::updateCurrentRow);
  connect(mContainer->selectionModel(), &ItemSelectionModel::currentRowChangeRequested, this, &TableViewController::setCurrentRow);
  connect(this, &TableViewController::currentRowChanged, mContainer->selectionModel(), &ItemSelectionModel::updateCurrentRow);
  ///registerItemModel();
  modelSetToView();
}

}} // namespace Mdt{ namespace ItemEditor{
