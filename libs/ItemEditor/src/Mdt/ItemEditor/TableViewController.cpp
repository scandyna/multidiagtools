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
#include "TableViewController.h"
#include "ItemViewPrivateContainer.h"
#include "TableViewControllerStateTable.h"
#include "TableViewControllerStatePermission.h"
#include "ControllerStateMachine.h"
#include "Mdt/ItemModel/PrimaryKey.h"
#include <QTableView>

// #include <QDebug>

using namespace Mdt::ItemModel;

namespace Mdt{ namespace ItemEditor{

TableViewController::TableViewController(QObject* parent)
 : AbstractItemModelController(parent),
   mContainer(new ItemViewPrivateContainer)
{
  setControllerStateMachine( ControllerStateMachine::makeNew<TableViewControllerStateTable, TableViewControllerStatePermission>(this) );
  connect(mContainer->proxyItemDelegate(), &EventCatchItemDelegate::dataEditionStarted, this, &TableViewController::onDataEditionStarted);
  connect(mContainer->proxyItemDelegate(), &EventCatchItemDelegate::dataEditionDone, this, &TableViewController::onDataEditionDone);
//   connect(this, &TableViewController::primaryKeyChanged, this, &TableViewController::onPrimaryKeyChanged);
}

TableViewController::~TableViewController()
{
}

void TableViewController::setView(QTableView* view)
{
  Q_ASSERT(view != nullptr);

  mContainer->setView(view);
  registerModelAndSelectionModel();
}

QTableView* TableViewController::view() const
{
  return reinterpret_cast<QTableView*>(mContainer->view());
}

void TableViewController::setPrimaryKeyHidden(bool hide)
{
  mPrimaryKeyColumnsHidden = hide;
  updatePrimaryKeyColumnsVisibility();
}

void TableViewController::setForeignKeyHidden(bool hide)
{
  mForeignKeyColumnsHidden = hide;
  updateForeignKeyColumnsVisibility();
}

void TableViewController::setModelToView(QAbstractItemModel* model)
{
  mContainer->setModel(model);
  registerModelAndSelectionModel();
}

bool TableViewController::setDataToModel()
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
  modelSetToView();
}

void TableViewController::primaryKeyChangedEvent(const PrimaryKey & oldPrimaryKey, const PrimaryKey & newPrimaryKey)
{
  auto *v = view();
  if(v == nullptr){
    return;
  }
  for(int column : oldPrimaryKey){
    v->setColumnHidden(column, false);
  }
  for(int column : newPrimaryKey){
    v->setColumnHidden(column, mPrimaryKeyColumnsHidden);
  }
}

void TableViewController::updatePrimaryKeyColumnsVisibility()
{
  auto *v = view();
  if(v == nullptr){
    return;
  }
  const auto pk = getPrimaryKey();
  for(int column : pk){
    v->setColumnHidden(column, mPrimaryKeyColumnsHidden);
  }
}

void TableViewController::foreignKeyChangedEvent(const ForeignKey& oldForeignKey, const ForeignKey& newForeignKey)
{
  auto *v = view();
  if(v == nullptr){
    return;
  }
  for(int column : oldForeignKey){
    v->setColumnHidden(column, false);
  }
  for(int column : newForeignKey){
    v->setColumnHidden(column, mForeignKeyColumnsHidden);
  }
}

void TableViewController::updateForeignKeyColumnsVisibility()
{
  auto *v = view();
  if(v == nullptr){
    return;
  }
  const auto fk = getForeignKey();
  for(int column : fk){
    v->setColumnHidden(column, mForeignKeyColumnsHidden);
  }
}

}} // namespace Mdt{ namespace ItemEditor{
