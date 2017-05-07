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
#include "TableViewControllerImplementation.h"
#include "AbstractController.h"
#include "EventCatchItemDelegate.h"
#include "ItemSelectionModel.h"
#include <QObject>
#include <QTableView>

using namespace Mdt::ItemModel;

namespace Mdt{ namespace ItemEditor{

void TableViewControllerImplementation::setModel(QAbstractItemModel * const model)
{
  Q_ASSERT(model != nullptr);

  mContainer.setModel(model);
}

void TableViewControllerImplementation::setView(QTableView * const view)
{
  Q_ASSERT(view != nullptr);

  mContainer.setView(view);
}

QTableView* TableViewControllerImplementation::view() const
{
  return reinterpret_cast<QTableView*>(mContainer.view());
}

void TableViewControllerImplementation::setMultiRowSelectionAllowed(bool allow)
{
  Q_ASSERT(mContainer.selectionModel() != nullptr);

  mContainer.selectionModel()->setMultiRowSelectionAllowed(allow);
}

ItemModel::RowList TableViewControllerImplementation::getSelectedRows() const
{
  Q_ASSERT(mContainer.selectionModel() != nullptr);

  return RowList::fromModelIndexList( mContainer.selectionModel()->selectedIndexes() );
}

bool TableViewControllerImplementation::connectToController(AbstractController* controller)
{
  Q_ASSERT(controller != nullptr);
  /*
   * Order of signal/slot connections matters here.
   * We must be sure that model is set to the view
   * before it is registered (set to RowChangeEventDispatcher).
   * Not doing so will produces a problem when model resets:
   *  - Controller receives the event and updates current row to 0 (if model contains data)
   *  - Controller updates current index of view
   *  - View will reset (and current will also be lost!)
   */
  if( (model() == nullptr) || (view() == nullptr) ){
    return false;
  }
  Q_ASSERT(model() == controller->modelForView());
  QObject::disconnect(mContainer.selectionModel(), &ItemSelectionModel::currentRowChangeRequested, controller, &AbstractController::setCurrentRow);
  QObject::disconnect(controller, &AbstractController::currentRowToBeSet, mContainer.selectionModel(), &ItemSelectionModel::updateCurrentRow);
  QObject::connect(mContainer.selectionModel(), &ItemSelectionModel::currentRowChangeRequested, controller, &AbstractController::setCurrentRow);
  QObject::connect(controller, &AbstractController::currentRowToBeSet, mContainer.selectionModel(), &ItemSelectionModel::updateCurrentRow);

  return true;
}

bool TableViewControllerImplementation::setDataToModel()
{
  mContainer.proxyItemDelegate()->commitCurrentEditorData();
  return true;
}

void TableViewControllerImplementation::revertDataFromModel()
{
  mContainer.proxyItemDelegate()->closeCurrentEditor();
}

void TableViewControllerImplementation::setPrimaryKeyHidden(const ItemModel::PrimaryKey & pk, bool hide)
{
  mPrimaryKeyColumnsHidden = hide;
  updatePrimaryKeyColumnsVisibility(pk);
}

void TableViewControllerImplementation::setForeignKeyHidden(const QString& foreignEntityName, bool hide)
{
  Q_ASSERT(!foreignEntityName.trimmed().isEmpty());

  
}

void TableViewControllerImplementation::setAllForeignKeysHidden(bool hide)
{

}

void TableViewControllerImplementation::setForeignKeyHidden(const ItemModel::ForeignKey & fk, bool hide)
{
  mForeignKeyColumnsHidden = hide;
  updateForeignKeyColumnsVisibility(fk);
}

void TableViewControllerImplementation::primaryKeyChangedEvent(const ItemModel::PrimaryKey& oldPrimaryKey, const ItemModel::PrimaryKey& newPrimaryKey)
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

void TableViewControllerImplementation::foreignKeyChangedEvent(const ItemModel::ForeignKey& oldForeignKey, const ItemModel::ForeignKey& newForeignKey)
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

void TableViewControllerImplementation::updatePrimaryKeyColumnsVisibility(const ItemModel::PrimaryKey& pk)
{
  auto *v = view();
  if(v == nullptr){
    return;
  }
  for(int column : pk){
    v->setColumnHidden(column, mPrimaryKeyColumnsHidden);
  }
}

void TableViewControllerImplementation::updateForeignKeyColumnsVisibility(const ItemModel::ForeignKey& fk)
{
  auto *v = view();
  if(v == nullptr){
    return;
  }
  for(int column : fk){
    v->setColumnHidden(column, mForeignKeyColumnsHidden);
  }
}

}} // namespace Mdt{ namespace ItemEditor{
