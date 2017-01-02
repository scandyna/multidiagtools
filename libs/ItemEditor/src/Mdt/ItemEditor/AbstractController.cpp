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
#include "AbstractController.h"
#include "RowChangeEventDispatcher.h"
#include "ControllerStatePermission.h"
#include "Mdt/ItemModel/RelationFilterProxyModel.h"
#include <QAbstractItemModel>

// #include <QDebug>

namespace ItemModel = Mdt::ItemModel;
using ItemModel::FilterProxyModel;
using ItemModel::RelationFilterProxyModel;

namespace Mdt{ namespace ItemEditor{

AbstractController::AbstractController(QObject* parent)
 : QObject(parent),
   pvInsertLocation(InsertAtBeginning)
{
  pvRowChangeEventDispatcher = new RowChangeEventDispatcher(this);
  connect(pvRowChangeEventDispatcher, &RowChangeEventDispatcher::rowStateUpdated, this, &AbstractController::updateRowState);
}

void AbstractController::setInsertLocation(AbstractController::InsertLocation il)
{
  pvInsertLocation = il;
}

// void AbstractController::setModel(QAbstractItemModel* model)
// {
//   Q_ASSERT(model != nullptr);
// 
//   mModelContainer.setSourceModel(model);
//   setModelToView(modelForView());
// }

void AbstractController::registerModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  mModelContainer.setSourceModel(model);
  setModelToView(modelForView());
}

void AbstractController::modelSetToView()
{
  auto *model = modelForView();
  Q_ASSERT(model != nullptr);

  if(model == pvRowChangeEventDispatcher->model()){
    return;
  }
  pvRowChangeEventDispatcher->setModel(model);
}

int AbstractController::rowCount() const
{
  return pvRowChangeEventDispatcher->rowCount();
}

int AbstractController::currentRow() const
{
  return pvRowChangeEventDispatcher->currentRow();
}

void AbstractController::setFilterEnabled(bool enable)
{
  if(enable == isFilterEnabled()){
    return;
  }
  if(enable){
    mModelContainer.prependProxyModel(new FilterProxyModel(this));
  }else{
    mModelContainer.deleteFirstProxyModelOfType<FilterProxyModel>();
  }
  setModelToView(modelForView());
}

bool AbstractController::isFilterEnabled() const
{
  return mModelContainer.containsProxyModelOfType<FilterProxyModel>();
}

void AbstractController::setRelationFilterEnabled(bool enable)
{
  if(enable == isRelationFilterEnabled()){
    return;
  }
  if(enable){
    mModelContainer.prependProxyModel(new RelationFilterProxyModel(this));
  }else{
    mModelContainer.deleteFirstProxyModelOfType<RelationFilterProxyModel>();
  }
  setModelToView(modelForView());
}

bool AbstractController::isRelationFilterEnabled() const
{
  return mModelContainer.containsProxyModelOfType<RelationFilterProxyModel>();
}

void AbstractController::setRelationFilterParentModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  setRelationFilterEnabled(true);
  relationFilterModel()->setParentModel(model);
}

void AbstractController::setRelationFilter(const ItemModel::RelationFilterExpression & expression)
{
  setRelationFilterEnabled(true);
  relationFilterModel()->setFilter(expression);
}

RelationFilterProxyModel* AbstractController::relationFilterModel() const
{
  auto *model = mModelContainer.firstProxyModelOfType<RelationFilterProxyModel>();
  Q_ASSERT(model != nullptr);
  return reinterpret_cast<RelationFilterProxyModel*>(model);
}

bool AbstractController::setCurrentRow(int row)
{
  Q_ASSERT(row >= -1);

  if(row == currentRow()){
    return true;
  }
  if(!ControllerStatePermission::canChangeCurrentRow(pvControllerState)){
    return false;
  }
  /**
   * \todo If row >= rowCount()
   *       we must try to fetch more data
   *       until we found row, or no more data is available,
   *       and consider this value as current row.
   */
  pvRowChangeEventDispatcher->setCurrentRow(row);

  return true;
}

void AbstractController::toFirst()
{
  if(rowCount() > 0){
    setCurrentRow(0);
  }else{
    setCurrentRow(-1);
  }
}

void AbstractController::toPrevious()
{
  /// \todo checks..
  setCurrentRow(currentRow()-1);
}

void AbstractController::toNext()
{
  /// \todo checks..
  setCurrentRow(currentRow()+1);
}

void AbstractController::toLast()
{
  setCurrentRow(rowCount()-1);
}

bool AbstractController::submit()
{
  if(!ControllerStatePermission::canSubmit(pvControllerState)){
    return false;
  }
  return submitDataToModel();
}

void AbstractController::revert()
{
  if(!ControllerStatePermission::canRevert(pvControllerState)){
    return;
  }
  revertDataFromModel();
}

bool AbstractController::insert()
{
  auto *model = modelForView();
  Q_ASSERT(model != nullptr);

  if(!ControllerStatePermission::canInsert(pvControllerState)){
    return false;
  }
  switch(pvInsertLocation){
    case InsertAtBeginning:
      return model->insertRow(0);
    case InsertAtEnd:
      /// \todo Fetch all
      return model->insertRow( rowCount() );
  }

  return false;
}

bool AbstractController::remove()
{
  auto *model = modelForView();
  Q_ASSERT(model != nullptr);

  if(!ControllerStatePermission::canRemove(pvControllerState)){
    return false;
  }
  int row = currentRow();
  if(row < 0){
    return false;
  }

  return model->removeRow(row);
}

void AbstractController::onDataEditionStarted()
{
  setControllerState(ControllerState::Editing);
}

void AbstractController::onDataEditionDone()
{
  setControllerState(ControllerState::Visualizing);
}

void AbstractController::updateRowState(RowState rs)
{
  emit currentRowChanged(rs.currentRow());
  emit rowStateChanged(rs);
}

void AbstractController::setControllerState(ControllerState state)
{
  if(state != pvControllerState){
    pvControllerState = state;
    emit controllerStateChanged(state);
  }else{
    pvControllerState = state;
  }
}

FilterProxyModel* AbstractController::filterModel() const
{
  auto *model = mModelContainer.firstProxyModelOfType<FilterProxyModel>();
  Q_ASSERT(model != nullptr);
  return reinterpret_cast<FilterProxyModel*>(model);
}

}} // namespace Mdt{ namespace ItemEditor{
