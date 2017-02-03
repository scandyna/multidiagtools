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
#include "ControllerRelation.h"
#include "AbstractController.h"
#include "Mdt/ItemModel/RelationFilterProxyModel.h"
#include <QAbstractItemModel>
#include <QObject>

#include <QDebug>

using Mdt::ItemModel::RelationFilterProxyModel;
using Mdt::ItemModel::RelationKeyCopier;

namespace Mdt{ namespace ItemEditor{

ControllerRelation::ControllerRelation(AbstractController* parentController)
 : ControllerRelationImplBase(parentController),
   mProxyModel(new RelationFilterProxyModel)
{
  Q_ASSERT(parentController != nullptr);

  if(parentController->modelForView() != nullptr){
    mProxyModel->setParentModel(parentController->modelForView());
  }
  QObject::connect(parentController, &AbstractController::modelForViewChanged, mProxyModel.get(), &RelationFilterProxyModel::setParentModel);
  QObject::connect(parentController, &AbstractController::currentRowChanged, mProxyModel.get(), &RelationFilterProxyModel::setParentModelMatchRow);
  mProxyModel->setParentModelMatchRow( parentController->currentRow() );
}

// unique_ptr needs complete definition of RelationFilterExpression
ControllerRelation::~ControllerRelation()
{
  if(childController() != nullptr){
    childController()->removeProxyModel(mProxyModel.get());
  }
}

void ControllerRelation::registerChildController(const ItemModel::RelationFilterExpression & conditions)
{
  Q_ASSERT(parentController() != nullptr);
  Q_ASSERT(childController() != nullptr);

  if(childController()->sourceModel() != nullptr){
    mProxyModel->setSourceModel( childController()->sourceModel() );
  }
  mChildSourceModelChangedConnection1 = 
    QObject::connect(childController(), &AbstractController::sourceModelChanged, mProxyModel.get(), &RelationFilterProxyModel::setSourceModel);
  childController()->prependProxyModel(mProxyModel.get());
  mProxyModel->setFilter(conditions);
}

void ControllerRelation::unregisterChildController()
{
  Q_ASSERT(parentController() != nullptr);
  Q_ASSERT(childController() != nullptr);

  QObject::disconnect(mChildSourceModelChangedConnection1);
  childController()->removeProxyModel(mProxyModel.get());
}

void ControllerRelation::onParentControllerStateChaged(ControllerState newState)
{

}

void ControllerRelation::onChildControllerStateChaged(ControllerState newState)
{

}

void ControllerRelation::setParentControllerModelToChildController()
{
  Q_ASSERT(parentController() != nullptr);
  Q_ASSERT(childController() != nullptr);

  auto *model = parentController()->modelForView();
  if(model == nullptr){
    return;
  }
//   childController()->setRelationFilterParentModel(model);
}

void ControllerRelation::onParentControllerModelChanged(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  mProxyModel->setParentModel(model);
}

}} // namespace Mdt{ namespace ItemEditor{
