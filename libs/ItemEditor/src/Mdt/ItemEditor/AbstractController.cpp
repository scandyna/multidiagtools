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
#include "NavigationControllerRelation.h"
#include "FilterControllerRelation.h"
#include "Mdt/ItemModel/PrimaryKeyProxyModel.h"
#include "Mdt/ItemModel/ForeignKeyProxyModel.h"

// #include "Mdt/ItemModel/RelationFilterProxyModel.h"

#include <QAbstractItemModel>

#include "Debug.h"

namespace ItemModel = Mdt::ItemModel;
using ItemModel::FilterProxyModel;
using ItemModel::PrimaryKey;
using ItemModel::PrimaryKeyProxyModel;
using ItemModel::ForeignKey;
using ItemModel::ForeignKeyProxyModel;
// using ItemModel::RelationFilterProxyModel;

namespace Mdt{ namespace ItemEditor{

AbstractController::AbstractController(QObject* parent)
 : QObject(parent),
   mInsertLocation(InsertAtBeginning),
   mRelationList(this)
{
  mRowChangeEventDispatcher = new RowChangeEventDispatcher(this);
  connect(mRowChangeEventDispatcher, &RowChangeEventDispatcher::rowStateUpdated, this, &AbstractController::updateRowState);
//   connect(pvRowChangeEventDispatcher, &RowChangeEventDispatcher::rowsInserted, this, &AbstractController::onRowsInserted);
  connect(mRowChangeEventDispatcher, &RowChangeEventDispatcher::rowsRemoved, this, &AbstractController::onRowsRemoved);
}

void AbstractController::setInsertLocation(AbstractController::InsertLocation il)
{
  mInsertLocation = il;
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
//     connect(pkModel, &PrimaryKeyProxyModel::sourceModelChanged, this, &AbstractController::onPrimaryKeySourceModelChanged);
    appendProxyModel(new PrimaryKeyProxyModel(this));
  }else{
    primaryKeyChangedEvent(getPrimaryKey(), PrimaryKey());
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
  Q_ASSERT(!pk.isNull());

  const auto oldPk = getPrimaryKey();
  setPrimaryKeyEnabled(true);
  getPrimaryKeyProxyModel()->setPrimaryKey(pk);
  primaryKeyChangedEvent(oldPk, pk);
//   emit primaryKeyChanged(pk);
}

void AbstractController::setPrimaryKey(std::initializer_list<int> pk)
{
  setPrimaryKey( PrimaryKey(pk) );
}

ItemModel::PrimaryKey AbstractController::getPrimaryKey() const
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

void AbstractController::setForeignKeyEnabled(bool enable)
{
  if(enable == isForeignKeyEnabled()){
    return;
  }
  if(enable){
    appendProxyModel(new ForeignKeyProxyModel(this));
  }else{
    deleteFirstProxyModelOfType<ForeignKeyProxyModel>();
  }
}

ForeignKeyProxyModel* AbstractController::getForeignKeyProxyModel() const
{
  auto *model = mModelContainer.firstProxyModelOfType<ForeignKeyProxyModel>();
  return reinterpret_cast<ForeignKeyProxyModel*>(model);
}

bool AbstractController::isForeignKeyEnabled() const
{
  return mModelContainer.containsProxyModelOfType<ForeignKeyProxyModel>();
}

void AbstractController::setForeignKey(const ItemModel::ForeignKey & fk)
{
  Q_ASSERT(!fk.isNull());

  const auto oldFk = getForeignKey();
  setForeignKeyEnabled(true);
  getForeignKeyProxyModel()->setForeignKey(fk);
  foreignKeyChangedEvent(oldFk, fk);
}

void AbstractController::setForeignKey(std::initializer_list<int> fk)
{
  setForeignKey( ForeignKey(fk) );
}

ItemModel::ForeignKey AbstractController::getForeignKey() const
{
  const auto *model = getForeignKeyProxyModel();
  if(model == nullptr){
    return ForeignKey();
  }
  return model->foreignKey();
}

void AbstractController::setForeignKeyEditable(bool editable)
{
  setForeignKeyEnabled(true);
  getForeignKeyProxyModel()->setForeignKeyEditable(editable);
}

void AbstractController::setForeignKeyItemsEnabled(bool enable)
{
  setForeignKeyEnabled(true);
  getForeignKeyProxyModel()->setForeignKeyItemsEnabled(enable);
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
//   mRelationList.setParentControllerModelToChildControllers(); /// \todo Needed ?
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

  auto *relation = mRelationList.addChildController<FilterControllerRelation>(controller);
  relation->setRelationFilter(conditions);
}

void AbstractController::addChildController(AbstractController* controller)
{
  Q_ASSERT(controller != nullptr);
  Q_ASSERT(!getPrimaryKey().isNull());
  Q_ASSERT(!controller->getForeignKey().isNull());
  Q_ASSERT(getPrimaryKey().columnCount() == controller->getForeignKey().columnCount());

  auto *relation = mRelationList.addChildController<FilterControllerRelation>(controller);
  relation->setRelationFilterFromPkFk();
}

void AbstractController::setNavigationController(AbstractController* controller)
{
  Q_ASSERT(controller != nullptr);

  controller->mRelationList.addChildController<NavigationControllerRelation>(this);
}

bool AbstractController::setCurrentRow(int row)
{
  Q_ASSERT(row >= -1);

  if(row == currentRow()){
    return true;
  }
//   if(!ControllerStatePermission::canChangeCurrentRow(mControllerState)){
  if(!canChangeCurrentRow()){
//     qDebug() << "AC: cannot change current row in state " << (int)controllerState();
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
  if(!canSubmit()){
    qDebug() << "AbstractController: connot submit in state " << controllerStateText(mControllerState);
    return false;
  }
  if(!mRelationList.submitForEachChild()){
    qDebug() << "AbstractController: submit failed for a child controller";
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
  if(!canRevert()){
    return;
  }
  mRelationList.revertForEachChild();
  revertDataFromModel();
}

bool AbstractController::insert()
{
  auto *model = modelForView();
  Q_ASSERT(model != nullptr);

//   if(!ControllerStatePermission::canInsert(mControllerState)){
  if(!canInsert()){
    return false;
  }
  ///disableDynamicFilters();
  setControllerState(ControllerState::Inserting);
  qDebug() << "AC: insert into " << model;
  bool ok = false;
  switch(mInsertLocation){
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

//   if(!ControllerStatePermission::canRemove(mControllerState)){
  if(!canRemove()){
    return false;
  }
  int row = currentRow();
  if(row < 0){
    return false;
  }

  return model->removeRow(row);
}

void AbstractController::setControllerStateMachine(const ControllerStateMachine & stateMachine)
{
  Q_ASSERT(!stateMachine.isNull());

  mControllerStateMachine = stateMachine;
}

void AbstractController::setControllerStatePermission(const ControllerStatePermission& permission)
{
  mControllerStatePermission = permission;
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
  primaryKeyChangedEvent( PrimaryKey(), getPrimaryKey() );
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

void AbstractController::foreignKeyChangedEvent(const ForeignKey& , const ForeignKey&)
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
  qDebug() << "AC: rows removed , rows: " << rowCount();
  if(controllerState() == ControllerState::Inserting){
    setControllerState(ControllerState::Visualizing);
  }
}

void AbstractController::updateRowState(RowState rs)
{
//   qDebug() << "AC::updateRowState()";
  emit currentRowChanged(rs.currentRow());
  emit rowStateChanged(rs);
}

// void AbstractController::onPrimaryKeySourceModelChanged()
// {
//   primaryKeyChangedEvent( PrimaryKey(), getPrimaryKey() );
// }

void AbstractController::setControllerState(ControllerState state)
{
  if(state != mControllerState){
    mControllerState = state;
    qDebug() << "AC " << this << ": new state: " << mControllerState;
    emit controllerStateChanged(state);
  }else{ /// \todo ??
    mControllerState = state;
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
