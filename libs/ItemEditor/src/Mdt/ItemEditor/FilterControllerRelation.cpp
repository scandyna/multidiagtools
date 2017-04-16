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
#include "FilterControllerRelation.h"
#include "AbstractController.h"
#include "ControllerStateMachine.h"
#include "ControllerEvent.h"
#include "Mdt/ItemModel/RelationFilterProxyModel.h"
#include "Mdt/ItemModel/RelationKey.h"

// #include "Debug.h"

using namespace Mdt::ItemModel;

namespace Mdt{ namespace ItemEditor{

FilterControllerRelation::FilterControllerRelation(QObject* parent)
 : AbstractControllerRelation(parent),
   mProxyModel(new RelationFilterProxyModel)
{
}

FilterControllerRelation::~FilterControllerRelation()
{
  if(childController() != nullptr){
    childController()->removeProxyModel(mProxyModel.get());
  }
}

void FilterControllerRelation::setRelationFilter(const ItemModel::RelationFilterExpression& conditions)
{
  Q_ASSERT(!conditions.isNull());

  mProxyModel->setFilter(conditions);
}

void FilterControllerRelation::setRelationFilterFromPkFk()
{
  Q_ASSERT(childController() != nullptr);
  Q_ASSERT(!parentController()->getPrimaryKey().isNull());
  Q_ASSERT(!childController()->getForeignKey().isNull());
  Q_ASSERT(parentController()->getPrimaryKey().columnCount() == childController()->getForeignKey().columnCount());

  Mdt::ItemModel::RelationKey key;
  key.setKey( parentController()->getPrimaryKey(), childController()->getForeignKey() );
  setFilterFromRelationKey(key);
}

void FilterControllerRelation::setParentModelMatchRow(int row)
{
  if(childController() != nullptr){
    Q_ASSERT(childController()->controllerStateMachine() != nullptr);
    if(row < 0){
      childController()->controllerStateMachine()->setEvent(ControllerEvent::DisableController);
    }else{
      childController()->controllerStateMachine()->setEvent(ControllerEvent::EnableController);
    }
  }
  mProxyModel->setParentModelMatchRow(row);
}

void FilterControllerRelation::setFilterFromRelationKey(const ItemModel::RelationKey& key)
{
  Q_ASSERT(!key.isNull());

  mProxyModel->setFilter(key);
}

void FilterControllerRelation::parentControllerChangedEvent(AbstractController* controller)
{
  Q_ASSERT(controller != nullptr);

  disconnect(mSetParentCurrentRowConnection);
  mSetParentCurrentRowConnection =
    connect(controller, &AbstractController::currentRowToBeSet, this, &FilterControllerRelation::setParentModelMatchRow);
  setParentModelMatchRow( controller->currentRow() );
//     connect(controller, &AbstractController::currentRowToBeSet, mProxyModel.get(), &RelationFilterProxyModel::setParentModelMatchRow);
//     connect(controller, &AbstractController::currentRowChanged, mProxyModel.get(), &RelationFilterProxyModel::setParentModelMatchRow);
//   mProxyModel->setParentModelMatchRow( controller->currentRow() );
}

void FilterControllerRelation::childControllerAboutToChangeEvent(AbstractController* oldController)
{
  Q_ASSERT(oldController != nullptr);

  oldController->removeProxyModel(mProxyModel.get());
}

void FilterControllerRelation::childControllerChangedEvent(AbstractController* controller)
{
  Q_ASSERT(controller != nullptr);

  controller->prependProxyModel(mProxyModel.get());
  mProxyModel->setSourceModel(controller->sourceModel());
  setParentModelMatchRow( controller->currentRow() );
}

void FilterControllerRelation::parentControllerModelChangedEvent(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  mProxyModel->setParentModel(model);
}

// void FilterControllerRelation::childControllerModelChangedEvent(QAbstractItemModel* model)
// {
//   Q_ASSERT(model != nullptr);
// 
//   qDebug() << "FCR: new child model: " << model;
//   mProxyModel->setSourceModel(model);
// }

}} // namespace Mdt{ namespace ItemEditor{
