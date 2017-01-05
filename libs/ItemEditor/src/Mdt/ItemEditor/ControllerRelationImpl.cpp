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
#include "ControllerRelationImpl.h"
#include "AbstractController.h"
#include "Mdt/ItemModel/RelationFilterProxyModel.h"
#include <QObject>

// #include <QDebug>

using Mdt::ItemModel::RelationFilterProxyModel;

namespace Mdt{ namespace ItemEditor{

ControllerRelationImpl::ControllerRelationImpl(AbstractController* parentController)
 : ControllerRelation(parentController)
{
}

void ControllerRelationImpl::registerChildController(const ItemModel::RelationFilterExpression & conditions)
{
  Q_ASSERT(parentController() != nullptr);
  Q_ASSERT(childController() != nullptr);

  childController()->setRelationFilter(conditions);
  QObject::connect(parentController(), &AbstractController::currentRowChanged, childController()->relationFilterModel(), &RelationFilterProxyModel::setParentModelMatchRow);
}

void ControllerRelationImpl::unregisterChildController()
{
  Q_ASSERT(parentController() != nullptr);
  Q_ASSERT(childController() != nullptr);

  QObject::disconnect(parentController(), &AbstractController::currentRowChanged, childController()->relationFilterModel(), &RelationFilterProxyModel::setParentModelMatchRow);
}

void ControllerRelationImpl::setParentControllerModelToChildController()
{
  Q_ASSERT(parentController() != nullptr);
  Q_ASSERT(childController() != nullptr);

  auto *model = parentController()->modelForView();
  if(model == nullptr){
    return;
  }
  childController()->setRelationFilterParentModel(model);
}

}} // namespace Mdt{ namespace ItemEditor{
