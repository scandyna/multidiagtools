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

#include "AbstractItemModelController.h"

#include <QObject>

#include <QDebug>

using Mdt::ItemModel::RelationFilterProxyModel;

namespace Mdt{ namespace ItemEditor{

ControllerRelation::ControllerRelation(AbstractItemModelController* parentController)
 : mParentController(parentController)
{
  Q_ASSERT(!mParentController.isNull());

  updateParentControllerModel();
  QObject::connect(mParentController, &AbstractItemModelController::currentRowChanged, &mProxyModel, &RelationFilterProxyModel::setParentModelMatchRow);
}

void ControllerRelation::updateParentControllerModel()
{
  Q_ASSERT(!mParentController.isNull());

  auto *model = mParentController->modelForView();
  if(model == nullptr){
    return;
  }
  mProxyModel.setParentModel(model);
}

void ControllerRelation::updateChildControllerModel()
{
 Q_ASSERT(!mChildController.isNull());

//  auto *model = mChildController->model();
//  if(model == nullptr){
//    return;
//  }
//  mProxyModel.setSourceModel(model);
//  mChildController->setModel(&mProxyModel);
}

}} // namespace Mdt{ namespace ItemEditor{
