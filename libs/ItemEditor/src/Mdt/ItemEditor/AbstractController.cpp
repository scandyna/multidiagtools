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
#include "Mdt/ItemModel/PrimaryKeyProxyModel.h"

// #include "Mdt/ItemModel/RelationFilterProxyModel.h"

#include <QAbstractItemModel>

#include <QDebug>

namespace ItemModel = Mdt::ItemModel;
using ItemModel::FilterProxyModel;
using ItemModel::PrimaryKey;
using ItemModel::PrimaryKeyProxyModel;
// using ItemModel::RelationFilterProxyModel;

namespace Mdt{ namespace ItemEditor{

AbstractController::AbstractController(QObject* parent)
 : QObject(parent),
   pvInsertLocation(InsertAtBeginning),
   mRelationList(this)
{
  mRowChangeEventDispatcher = new RowChangeEventDispatcher(this);
  connect(mRowChangeEventDispatcher, &RowChangeEventDispatcher::rowStateUpdated, this, &AbstractController::updateRowState);
//   connect(pvRowChangeEventDispatcher, &RowChangeEventDispatcher::rowsInserted, this, &AbstractController::onRowsInserted);
  connect(mRowChangeEventDispatcher, &RowChangeEventDispatcher::rowsRemoved, this, &AbstractController::onRowsRemoved);
}

void AbstractController::setInsertLocation(AbstractController::InsertLocation il)
{
  pvInsertLocation = il;
}

int AbstractController::rowCount() const
{
  return mRowChangeEventDispatcher->rowCount();
}

int AbstractController::currentRow() const
{
  return mRowChangeEventDispatcher->currentRow();
}

RowState AbstractController::rowState() const
{
  return mRowChangeEventDispatcher->currentRowState();
}

void AbstractController::prependProxyModel(QAbstractProxyModel* proxyModel)
{
  Q_ASSERT(proxyModel != nullptr);

  auto *oldModelForView = modelForView();
  mModelContainer.prependProxyModel(proxyModel);
  updateModelForViewIfChanged(oldModelForView);
}

void AbstractController::appendProxyModel(QAbstractProxyModel* proxyModel)
{
  Q_ASSERT(proxyModel != nullptr);

  auto *oldModelForView = modelForView();
  mModelContainer.appendProxyModel(proxyModel);
  updateModelForViewIfChanged(oldModelForView);
}

void AbstractController::removeProxyModel(QAbstractProxyModel* model)
{
  auto *oldModelForView = modelForView();
  mModelContainer.removeProxyModel(model);
  updateModelForViewIfChanged(oldModelForView);
}

// void AbstractController::setModel(QAbstractItemModel* model)
// {
//   Q_ASSERT(model != nullptr);
// 
//   mModelContainer.setSourceModel(model);
//   setModelToView(modelForView());
// }

void AbstractController::setPrimaryKeyEnabled(bool enable)
{
  if(enable == isPrimaryKeyEnabled()){
    return;
  }
  if(enable){
    appendProxyModel(new PrimaryKeyProxyModel(this));
  }else{
    deleteFirstProxyModelOfType<PrimaryKeyProxyModel>();
  }
}

PrimaryKeyProxyModel* AbstractController::getPrimaryKeyProxyModel() const
{
  auto *model = mModelContainer.firstProxyModelOfType<PrimaryKeyProxyModel>();
  return reinterpret_cast<PrimaryKeyProxyModel*>(model);
}

bool AbstractController::isPrimaryKeyEnabled() const
{
  return mModelContainer.containsProxyModelOfType<PrimaryKeyProxyModel>();
}

void AbstractController::setPrimaryKey(const ItemModel::PrimaryKey & pk)
{
  const auto oldPk = primaryKey();
  setPrimaryKeyEnabled(true);
  getPrimaryKeyProxyModel()->setPrimaryKey(pk);
  primaryKeyChangedEvent(oldPk, pk);
//   emit primaryKeyChanged(pk);
}

void AbstractController::setPrimaryKey(std::initializer_list<int> pk)
{
  setPrimaryKey( PrimaryKey(pk) );
}

ItemModel::PrimaryKey AbstractController::primaryKey() const
{
  const auto *model = getPrimaryKeyProxyModel();
  if(model == nullptr){
    return PrimaryKey();
  }
  return model->primaryKey();
}

void AbstractController::setPrimaryKeyEditable(bool editable)
{
  setPrimaryKeyEnabled(true);
  getPrimaryKeyProxyModel()->setPrimaryKeyEditable(editable);
}

void AbstractController::setPrimaryKeyItemsEnabled(bool enable)
{
  setPrimaryKeyEnabled(true);
  getPrimaryKeyProxyModel()->setPrimaryKeyItemsEnabled(enable);
}

void AbstractController::setForeignKey(const ItemModel::ForeignKey & fk)
{
//   Q_ASSERT(sourceModel() != nullptr);
//   Q_ASSERT(fk.greatestColumn() < sourceModel()->columnCount());
// 
//   mForeignKey = fk;
}

ItemModel::ForeignKey AbstractController::foreignKey() const
{
//   return mForeignKey;
}

void AbstractController::setFilterEnabled(bool enable)
{
  if(enable == isFilterEnabled()){
    return;
  }
  if(enable){
    prependProxyModel(new FilterProxyModel(this));
  }else{
    deleteFirstProxyModelOfType<FilterProxyModel>();
  }
  mRelationList.setParentControllerModelToChildControllers(); /// \todo Needed ?
}

bool AbstractController::isFilterEnabled() const
{
  return mModelContainer.containsProxyModelOfType<FilterProxyModel>();
}

void AbstractController::setDynamicFiltersEnabled(bool enable)
{
  if(isFilterEnabled()){
    filterModel()->setDynamicSortFilter(enable);
  }
}

void AbstractController::addChildController(AbstractController *controller, const ItemModel::RelationFilterExpression & conditions)
{
  Q_ASSERT(controller != nullptr);

  mRelationList.addChildController(controller, conditions);
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
   *       -> Or not ?
   */
  mRowChangeEventDispatcher->setCurrentRow(row);

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
    qDebug() << "AbstractController: connot submit in state " << (int)pvControllerState;
    return false;
  }
  if(!submitDataToModel()){
    qDebug() << "AbstractController: submit failed";
    return false;
  }
  setControllerState(ControllerState::Visualizing);
  ///enableDynamicFilters();

  return true;
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
  ///disableDynamicFilters();
  setControllerState(ControllerState::Inserting);
  qDebug() << "AC: insert into " << model;
  bool ok = false;
  switch(pvInsertLocation){
    case InsertAtBeginning:
      ok = mRowChangeEventDispatcher->insertRowAtBeginning();
      break;
    case InsertAtEnd:
      /// \todo Fetch all (or not?)
      ok = mRowChangeEventDispatcher->insertRowAtEnd();
      break;
  }
  if(!ok){
    return false;
  }
  ///setControllerState(ControllerState::Editing);
//   /// \todo check if ok
//   setControllerState(ControllerState::Inserting);

  return true;
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

void AbstractController::registerModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

//   mPrimaryKey.clear();
//   mForeignKey.clear();
  mModelContainer.setSourceModel(model);
  emit sourceModelChanged(model);
  model = modelForView();
  emit modelForViewChanged(model);
  setModelToView(model);
}

void AbstractController::modelSetToView()
{
  auto *model = modelForView();
  if(model == mRowChangeEventDispatcher->model()){
    return;
  }
  mRowChangeEventDispatcher->setModel(model);
}

void AbstractController::primaryKeyChangedEvent(const PrimaryKey& , const PrimaryKey&)
{
}

void AbstractController::onDataEditionStarted()
{
  setControllerState(ControllerState::Editing);
}

void AbstractController::onDataEditionDone()
{
  setControllerState(ControllerState::Visualizing);
}

// void AbstractController::onRowsInserted()
// {
//   qDebug() << "AbstractController: rows inserted , rows: " << rowCount();
//   setControllerState(ControllerState::Inserting);
// }

void AbstractController::onRowsRemoved()
{
  qDebug() << "AbstractController: rows removed , rows: " << rowCount();
  if(controllerState() == ControllerState::Inserting){
    setControllerState(ControllerState::Visualizing);
  }
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
  }else{ /// \todo ??
    pvControllerState = state;
  }
}

FilterProxyModel* AbstractController::filterModel() const
{
  auto *model = mModelContainer.firstProxyModelOfType<FilterProxyModel>();
  Q_ASSERT(model != nullptr);
  return reinterpret_cast<FilterProxyModel*>(model);
}

void AbstractController::updateModelForViewIfChanged(QAbstractItemModel* oldModelForView)
{
  auto *newModelForView = modelForView();
  if(newModelForView != oldModelForView){
    setModelToView(newModelForView);
    emit modelForViewChanged(newModelForView);
  }
}

}} // namespace Mdt{ namespace ItemEditor{
