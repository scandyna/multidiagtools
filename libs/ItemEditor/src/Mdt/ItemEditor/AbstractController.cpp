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
#include "ControllerStateMachine.h"
#include "RowChangeEventDispatcher.h"
#include "NavigationControllerRelation.h"
#include "FilterControllerRelation.h"
#include "ControllerStatePermissionProxyModel.h"
#include "Mdt/ItemModel/PrimaryKeyProxyModel.h"
#include "Mdt/ItemModel/ForeignKeyProxyModel.h"
#include "Mdt/ItemModel/SortProxyModel.h"
#include <QAbstractItemModel>
#include <QModelIndex>
#include <algorithm>

// #include "Mdt/ItemModel/RelationFilterProxyModel.h"

#include "Debug.h"

namespace ItemModel = Mdt::ItemModel;
using ItemModel::SortProxyModel;
using ItemModel::FilterProxyModel;
using ItemModel::PrimaryKey;
using ItemModel::PrimaryKeyProxyModel;
using ItemModel::ForeignKey;
using ItemModel::ForeignKeyProxyModel;
using ItemModel::RowList;
using ItemModel::ColumnList;
// using ItemModel::RelationFilterProxyModel;

namespace Mdt{ namespace ItemEditor{

AbstractController::AbstractController(QObject* parent)
 : QObject(parent),
   mInsertLocation(InsertAtBeginning),
   mRelationList(this)
{
  mModelContainer.appendProxyModel( new ControllerStatePermissionProxyModel(this) );
  mRowChangeEventDispatcher = new RowChangeEventDispatcher(this);
  connect(mRowChangeEventDispatcher, &RowChangeEventDispatcher::rowStateChanged, this, &AbstractController::rowStateChanged);
  connect(mRowChangeEventDispatcher, &RowChangeEventDispatcher::currentRowToBeSet, this, &AbstractController::currentRowToBeSet);
//   connect(pvRowChangeEventDispatcher, &RowChangeEventDispatcher::rowsInserted, this, &AbstractController::onRowsInserted);
//   connect(mRowChangeEventDispatcher, &RowChangeEventDispatcher::rowsRemoved, this, &AbstractController::onRowsRemoved);
}

ControllerState AbstractController::controllerState() const
{
  if(mControllerStateMachine.isNull()){
    return ControllerState::Visualizing;  /// \todo Fix default state
  }
  return mControllerStateMachine->currentState();
}

ControllerStateMachine* AbstractController::controllerStateMachine() const
{
  return mControllerStateMachine;
}

bool AbstractController::canChangeCurrentRow() const
{
  if(mControllerStateMachine.isNull()){
    return false;
  }
  return mControllerStateMachine->canChangeCurrentRow();
}

bool AbstractController::canInsert() const
{
  if(mControllerStateMachine.isNull()){
    return false;
  }
  return mControllerStateMachine->canInsert();
}

bool AbstractController::canSubmit() const
{
  if(mControllerStateMachine.isNull()){
    return false;
  }
  return mControllerStateMachine->canSubmit();
}

bool AbstractController::canRevert() const
{
  if(mControllerStateMachine.isNull()){
    return false;
  }
  return mControllerStateMachine->canRevert();
}

bool AbstractController::canRemove() const
{
  if(mControllerStateMachine.isNull()){
    return false;
  }
  return mControllerStateMachine->canRemove();
}

void AbstractController::setInsertLocation(AbstractController::InsertLocation il)
{
  mInsertLocation = il;
}

void AbstractController::setModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

//   mPrimaryKey.clear();
//   mForeignKey.clear();
  if(model == mModelContainer.sourceModel()){
    return;
  }
  mModelContainer.setSourceModel(model);
  emit sourceModelChanged(model);
  model = modelForView();
  emit modelForViewChanged(model);
  setModelToView(model);
  primaryKeyChangedEvent( PrimaryKey(), getPrimaryKey() );
  const auto *fkModel = getForeignKeysProxyModel();
  if(fkModel != nullptr){
    columnsPartOfForeignKeyChangedEvent( ColumnList(), fkModel->getColumnsPartOfForeignKey() );
  }
//   foreignKeyChangedEvent( ForeignKey(), getForeignKey() );
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

int AbstractController::columnCount() const
{
  const auto *model = modelForView();

  if(model == nullptr){
    return 0;
  }
  return model->columnCount();
}

QVariant AbstractController::currentData(int column) const
{
  Q_ASSERT(sourceModel() != nullptr);
  Q_ASSERT(currentRow() >= 0);
  Q_ASSERT(currentRow() < rowCount());
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < columnCount());

  const auto *model = modelForView();
  Q_ASSERT(model != nullptr);
  const auto index = model->index(currentRow(), column);
  Q_ASSERT(index.isValid());

  return model->data(index);
}

void AbstractController::setEntityName(const QString& name)
{
  mEntityName = name;
}

void AbstractController::setUserFriendlyEntityName(const QString& name)
{
  mUserFriendlyEntityName = name;
}

QString AbstractController::userFriendlyEntityName() const
{
  if(mUserFriendlyEntityName.isEmpty()){
    return entityName();
  }
  return mUserFriendlyEntityName;
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

void AbstractController::setForeignKeysEnabled(bool enable)
{
  if(enable == isForeignKeysEnabled()){
    return;
  }
  if(enable){
    appendProxyModel(new ForeignKeyProxyModel(this));
  }else{
//     foreignKeyChangedEvent( getForeignKey(), ForeignKey() );
    const auto *fkModel = getForeignKeysProxyModel();
    if(fkModel != nullptr){
      columnsPartOfForeignKeyChangedEvent( fkModel->getColumnsPartOfForeignKey(), ColumnList() );
      deleteFirstProxyModelOfType<ForeignKeyProxyModel>();
    }
  }
}

ForeignKeyProxyModel* AbstractController::getForeignKeysProxyModel() const
{
  auto *model = mModelContainer.firstProxyModelOfType<ForeignKeyProxyModel>();
  return reinterpret_cast<ForeignKeyProxyModel*>(model);
}

bool AbstractController::isForeignKeysEnabled() const
{
  return mModelContainer.containsProxyModelOfType<ForeignKeyProxyModel>();
}

void AbstractController::addForeignKey(const QString & foreignEntityName, const ForeignKey & fk)
{
  Q_ASSERT(!foreignEntityName.trimmed().isEmpty());
  Q_ASSERT(!fk.isNull());

  setForeignKeysEnabled(true);
  auto *model = getForeignKeysProxyModel();
  Q_ASSERT(model != nullptr);
  const auto oldFkColumnList = model->getColumnsPartOfForeignKey();
  model->addForeignKey(foreignEntityName, fk);
  const auto newFkColumnList = model->getColumnsPartOfForeignKey();
  columnsPartOfForeignKeyChangedEvent(oldFkColumnList, newFkColumnList);
}

void AbstractController::addForeignKey(const QString& foreignEntityName, std::initializer_list< int > fk)
{
  Q_ASSERT(!foreignEntityName.trimmed().isEmpty());

  addForeignKey( foreignEntityName, ForeignKey(fk) );
}

ForeignKey AbstractController::getForeignKeyReferencing(const QString& entityName) const
{
  Q_ASSERT(!entityName.trimmed().isEmpty());

  const auto *model = getForeignKeysProxyModel();
  if(model == nullptr){
    return ForeignKey();
  }
  return model->getForeignKeyReferencing(entityName);
}

void AbstractController::setForeignKeyEditable(const QString& foreignEntityName, bool editable)
{
  setForeignKeysEnabled(true);
  getForeignKeysProxyModel()->setForeignKeyEditable(foreignEntityName, editable);
}

void AbstractController::setAllForeignKeysEditable(bool editable)
{
  setForeignKeysEnabled(true);
  getForeignKeysProxyModel()->setAllForeignKeysEditable(editable);
}

void AbstractController::setForeignKeyItemsEnabled(const QString& foreignEntityName, bool enable)
{
  setForeignKeysEnabled(true);
  getForeignKeysProxyModel()->setForeignKeyItemsEnabled(foreignEntityName, enable);
}

void AbstractController::setAllForeignKeysItemsEnabled(bool enable)
{
  setForeignKeysEnabled(true);
  getForeignKeysProxyModel()->setAllForeignKeysItemsEnabled(enable);
}


void AbstractController::setForeignKey(const ItemModel::ForeignKey & fk)
{
  Q_ASSERT(!fk.isNull());

  const auto oldFk = getForeignKey();
  setForeignKeysEnabled(true);
  getForeignKeysProxyModel()->setForeignKey(fk);
  foreignKeyChangedEvent(oldFk, fk);
}

void AbstractController::setForeignKey(std::initializer_list<int> fk)
{
  setForeignKey( ForeignKey(fk) );
}

ItemModel::ForeignKey AbstractController::getForeignKey() const
{
  const auto *model = getForeignKeysProxyModel();
  if(model == nullptr){
    return ForeignKey();
  }
  return model->foreignKey();
}

void AbstractController::setForeignKeyEditable(bool editable)
{
  setForeignKeysEnabled(true);
  getForeignKeysProxyModel()->setForeignKeyEditable(editable);
}

void AbstractController::setForeignKeyItemsEnabled(bool enable)
{
  setForeignKeysEnabled(true);
  getForeignKeysProxyModel()->setForeignKeyItemsEnabled(enable);
}

void AbstractController::setSortEnabled(bool enable)
{
  if(enable == isSortEnabled()){
    return;
  }
  if(enable){
    appendProxyModel(new SortProxyModel(this));
  }else{
    deleteFirstProxyModelOfType<SortProxyModel>();
  }
}

bool AbstractController::isSortEnabled() const
{
  return mModelContainer.containsProxyModelOfType<SortProxyModel>();
}

ItemModel::SortProxyModel* AbstractController::getSortProxyModel() const
{
  auto *model = mModelContainer.firstProxyModelOfType<SortProxyModel>();
  return reinterpret_cast<SortProxyModel*>(model);
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
  Q_ASSERT(!mControllerStateMachine.isNull());

  auto *relation = mRelationList.addChildController<FilterControllerRelation>(controller);
  relation->setRelationFilter(conditions);
  mControllerStateMachine->setHasChildController(true);
}

void AbstractController::addChildController(AbstractController* controller)
{
  Q_ASSERT(controller != nullptr);
  Q_ASSERT(!mControllerStateMachine.isNull());
  Q_ASSERT(!getPrimaryKey().isNull());
  Q_ASSERT(!controller->getForeignKey().isNull());
  Q_ASSERT(getPrimaryKey().columnCount() == controller->getForeignKey().columnCount());

  auto *relation = mRelationList.addChildController<FilterControllerRelation>(controller);
  relation->setRelationFilterFromPkFk();
  mControllerStateMachine->setHasChildController(true);
}

void AbstractController::setNavigationController(AbstractController* controller)
{
  Q_ASSERT(controller != nullptr);
  Q_ASSERT(!mControllerStateMachine.isNull());

  controller->mRelationList.addChildController<NavigationControllerRelation>(this);
  controller->mControllerStateMachine->setHasChildController(true);
}

bool AbstractController::setCurrentRow(int row)
{
  Q_ASSERT(row >= -1);

  if(row == currentRow()){
    return true;
  }
  if(row >= rowCount()){
    return false;
  }
  if(!canChangeCurrentRow()){
    qDebug() << "AC: cannot change current row in state " << controllerState();
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
  const int row = std::max( currentRow()-1 , -1 );
  setCurrentRow(row);
}

void AbstractController::toNext()
{
  setCurrentRow(currentRow()+1);
}

void AbstractController::toLast()
{
  setCurrentRow(rowCount()-1);
}

/** \todo Avoid double setDataToModel()
 *
 * - If submit() is called while item view is editing a item (has a editor opern),
 *   first tell it to commit
 * - For item view, submit() can be called after data has been set to model
 * - Widget mapper: allways tell it to commit (or?)
 */
bool AbstractController::submit()
{
//   qDebug() << "AC:submit - set data to model";
  if(!setDataToModel()){
    qDebug() << "AbstractController: set data to model failed";
    return false;
  }
  if(!canSubmit()){
    qDebug() << "AbstractController: connot submit in state " << controllerStateText(controllerState());
    return false;
  }
//   qDebug() << "AC:submit - submit for each child";
  if(!mRelationList.submitForEachChild()){
    qDebug() << "AbstractController: submit failed for a child controller";
    return false;
  }
  if(!submitChangesToStorage()){
    qDebug() << "AbstractController: submit data to storage failed";
    return false;
  }
  Q_ASSERT(!mControllerStateMachine.isNull());
//   qDebug() << "AC:submit - DONE";
  mControllerStateMachine->submitDone();
//   setControllerState(ControllerState::Visualizing);
  ///enableDynamicFilters();

  return true;
}

void AbstractController::revert()
{
  Q_ASSERT(!mControllerStateMachine.isNull());

  revertDataFromModel();
  if(!canRevert()){
    return;
  }
  mRelationList.revertForEachChild();
  if(controllerState() == ControllerState::Inserting){
    if(!removeCurrentRow()){
      return;
    }
  }else{
    revertChangesFromStorage();
  }
  mControllerStateMachine->revertDone();
}

bool AbstractController::insert()
{
  auto *model = modelForView();
  Q_ASSERT(model != nullptr);

  if(!canInsert()){
    return false;
  }
  //disableDynamicFilters();
  Q_ASSERT(!mControllerStateMachine.isNull());
  mControllerStateMachine->insertStarted();
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

  return true;
}

bool AbstractController::remove()
{
//   auto *model = modelForView();
//   Q_ASSERT(model != nullptr);

  if(!canRemove()){
    qDebug() << "AC: cannot remove in state " << controllerState();
    return false;
  }
  if(!removeSelectedRows()){
    return false;
  }
  if(!submitChangesToStorage()){
    return false;
  }
//   if(!removeCurrentRow()){
//     return false;
//   }
//   int row = currentRow();
//   if(row < 0){
//     return false;
//   }
//   if(!model->removeRow(row)){
//     return false;
//   }
  Q_ASSERT(!mControllerStateMachine.isNull());
  mControllerStateMachine->removeDone();

  return true;
}

void AbstractController::setControllerStateMachine(ControllerStateMachine *stateMachine)
{
  Q_ASSERT(stateMachine != nullptr);
  Q_ASSERT(stateMachine->parent() == this);

  mControllerStateMachine = stateMachine;
  auto *statePermissionModel = reinterpret_cast<ControllerStatePermissionProxyModel*>( mModelContainer.firstProxyModelOfType<ControllerStatePermissionProxyModel>() );
  Q_ASSERT(statePermissionModel != nullptr);
  statePermissionModel->setStateMachine(stateMachine);
}

ControllerStatePermissionProxyModel* AbstractController::getControllerStatePermissionProxyModel() const
{
  Q_ASSERT( mModelContainer.firstProxyModelOfType<ControllerStatePermissionProxyModel>() != nullptr );
  return reinterpret_cast<ControllerStatePermissionProxyModel*>( mModelContainer.firstProxyModelOfType<ControllerStatePermissionProxyModel>() );
}

// void AbstractController::registerModel(QAbstractItemModel* model)
// {
//   Q_ASSERT(model != nullptr);
// 
// //   mPrimaryKey.clear();
// //   mForeignKey.clear();
//   if(model == mModelContainer.sourceModel()){
//     return;
//   }
//   mModelContainer.setSourceModel(model);
//   emit sourceModelChanged(model);
//   model = modelForView();
//   emit modelForViewChanged(model);
//   setModelToView(model);
//   primaryKeyChangedEvent( PrimaryKey(), getPrimaryKey() );
//   foreignKeyChangedEvent( ForeignKey(), getForeignKey() );
// }

void AbstractController::modelSetToView()
{
  auto *model = modelForView();
  if(model == mRowChangeEventDispatcher->model()){
    return;
  }
  mRowChangeEventDispatcher->setModel(model);
}

bool AbstractController::submitChangesToStorage()
{
  return true;
}

void AbstractController::revertChangesFromStorage()
{
}

void AbstractController::primaryKeyChangedEvent(const PrimaryKey& , const PrimaryKey&)
{
}

void AbstractController::columnsPartOfForeignKeyChangedEvent(const ItemModel::ColumnList&, const ItemModel::ColumnList&)
{
}

void AbstractController::foreignKeyChangedEvent(const ForeignKey& , const ForeignKey&)
{
}

ItemModel::RowList AbstractController::getSelectedRows() const
{
  RowList list;

  const int row = currentRow();
  if(row >= 0){
    list.append(row);
  }

  return list;
}

bool AbstractController::removeSelectedRows()
{
  auto *model = modelForView();
  Q_ASSERT(model != nullptr);
  const auto rowList = getSelectedRows();

  int offset = 0;
  for(int row : rowList){
    Q_ASSERT(row >= 0);
    row = row - offset;
    ++offset;
    Q_ASSERT(row >= 0);
    qDebug() << "AC: remove row " << row;
    if(!model->removeRow(row)){
      return false;
    }
  }

  return true;
}

void AbstractController::onDataEditionStarted()
{
//   qDebug() << "AC: onDataEditionStarted() ..";
  if(mControllerStateMachine.isNull()){
    return;
  }
  mControllerStateMachine->dataEditionStarted();
}

void AbstractController::onDataEditionDone()
{
//   qDebug() << "AC: onDataEditionDone()";
  if(mControllerStateMachine.isNull()){
    return;
  }
  mControllerStateMachine->dataEditionDone();
}

// void AbstractController::onRowsInserted()
// {
//   qDebug() << "AbstractController: rows inserted , rows: " << rowCount();
//   setControllerState(ControllerState::Inserting);
// }

// void AbstractController::onRowsRemoved()
// {
//   qDebug() << "AC: rows removed , rows: " << rowCount();
// //   if(controllerState() == ControllerState::Inserting){
// //     setControllerState(ControllerState::Visualizing);
// //   }
// }

// void AbstractController::onPrimaryKeySourceModelChanged()
// {
//   primaryKeyChangedEvent( PrimaryKey(), getPrimaryKey() );
// }

// void AbstractController::setControllerState(ControllerState state)
// {
//   Q_ASSERT(!mControllerStateMachine.isNull());
//   qDebug() << "AC: Warning! setControllerState() is deprecated ! - state: " << state;
// //   const bool changed = ( controllerState() != state );
//   mControllerStateMachine->forceCurrentState(state);
// //   if(changed){
// //     emit controllerStateChanged(state);
// //   }
// 
// //   if(state != mControllerState){
// //     mControllerState = state;
// //     qDebug() << "AC " << this << ": new state: " << mControllerState;
// //     emit controllerStateChanged(state);
// //   }else{ /// \todo ??
// //     mControllerState = state;
// //   }
// }

bool AbstractController::removeCurrentRow()
{
  auto *model = modelForView();
  Q_ASSERT(model != nullptr);

  int row = currentRow();
  if(row < 0){
    return false;
  }
  if(!model->removeRow(row)){
    return false;
  }

  return true;
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
