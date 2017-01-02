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
#include <QObject>

// #include <QDebug>

using Mdt::ItemModel::RelationFilterProxyModel;

namespace Mdt{ namespace ItemEditor{

ControllerRelation::ControllerRelation(AbstractController* parentController)
 : mParentController(parentController)
{
  Q_ASSERT(!mParentController.isNull());
}

void ControllerRelation::setChildController(AbstractController* controller, const ItemModel::RelationFilterExpression& conditions)
{
  Q_ASSERT(controller != nullptr);

  if(!mChildController.isNull()){
    QObject::disconnect(mParentController, &AbstractController::currentRowChanged, mChildController->relationFilterModel(), &RelationFilterProxyModel::setParentModelMatchRow);
  }
  mChildController = controller;
  mChildController->setRelationFilter(conditions);
  setParentControllerModelToChildController();
  QObject::connect(mParentController, &AbstractController::currentRowChanged, mChildController->relationFilterModel(), &RelationFilterProxyModel::setParentModelMatchRow);
}

void ControllerRelation::setParentControllerModelToChildController()
{
  Q_ASSERT(!mParentController.isNull());
  Q_ASSERT(!mChildController.isNull());

  auto *model = mParentController->modelForView();
  if(model == nullptr){
    return;
  }
  mChildController->setRelationFilterParentModel(model);
}

}} // namespace Mdt{ namespace ItemEditor{
