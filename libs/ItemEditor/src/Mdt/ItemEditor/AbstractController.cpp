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
#include "AbstractController.h"
#include "RowChangeEventDispatcher.h"
#include "ControllerStatePermission.h"
#include <QAbstractItemModel>

// #include <QDebug>

namespace Mdt{ namespace ItemEditor{

AbstractController::AbstractController(QObject* parent)
 : QObject(parent),
   pvModel(nullptr),
   pvInsertLocation(InsertAtBeginning)
{
  pvRowChangeEventDispatcher = new RowChangeEventDispatcher(this);
  connect(pvRowChangeEventDispatcher, &RowChangeEventDispatcher::rowStateUpdated, this, &AbstractController::updateRowState);
}

void AbstractController::setInsertLocation(AbstractController::InsertLocation il)
{
  pvInsertLocation = il;
}

void AbstractController::setModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  referenceItemModel(model);
  registerItemModel();
}

int AbstractController::rowCount() const
{
  return pvRowChangeEventDispatcher->rowCount();
}

int AbstractController::currentRow() const
{
  return pvRowChangeEventDispatcher->currentRow();
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
  Q_ASSERT(!pvModel.isNull());

  if(!ControllerStatePermission::canInsert(pvControllerState)){
    return false;
  }
  switch(pvInsertLocation){
    case InsertAtBeginning:
      return pvModel->insertRow(0);
    case InsertAtEnd:
      /// \todo Fetch all
      return pvModel->insertRow( rowCount() );
  }

  return false;
}

bool AbstractController::remove()
{
  Q_ASSERT(!pvModel.isNull());

  if(!ControllerStatePermission::canRemove(pvControllerState)){
    return false;
  }
  int row = currentRow();
  if(row < 0){
    return false;
  }

  return pvModel->removeRow(row);
}

void AbstractController::referenceItemModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  if(model == pvModel){
    return;
  }
  pvModel = model;
}

void AbstractController::registerItemModel()
{
  Q_ASSERT(!pvModel.isNull());

  if(pvModel == pvRowChangeEventDispatcher->model()){
    return;
  }
  pvRowChangeEventDispatcher->setModel(pvModel);
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

}} // namespace Mdt{ namespace ItemEditor{
