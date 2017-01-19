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
#include "Mdt/ItemModel/RelationKeyCopier.h"

// #include <QDebug>

using Mdt::ItemModel::RelationFilterProxyModel;
using Mdt::ItemModel::RelationKeyCopier;

namespace Mdt{ namespace ItemEditor{

ControllerRelationImpl::ControllerRelationImpl(AbstractController* parentController)
 : ControllerRelation(parentController),
   mProxyModel(new RelationFilterProxyModel),
   mKeyCopier(new RelationKeyCopier)
{
  Q_ASSERT(parentController != nullptr);

  mKeyCopier->setCopyTriggers(RelationKeyCopier::ChildModelRowsInserted);
  if(parentController->modelForView() != nullptr){
    mProxyModel->setParentModel(parentController->modelForView());
    mKeyCopier->setParentModel(parentController->modelForView());
  }
  mKeyCopier->setChildModel(mProxyModel.get());
  QObject::connect(parentController, &AbstractController::modelForViewChanged, mProxyModel.get(), &RelationFilterProxyModel::setParentModel);
  QObject::connect(parentController, &AbstractController::modelForViewChanged, mKeyCopier.get(), &RelationKeyCopier::setParentModel);
}

// unique_ptr needs complete definition of RelationFilterExpression and RelationKeyCopier
ControllerRelationImpl::~ControllerRelationImpl()
{
  if(childController() != nullptr){
    childController()->removeProxyModel(mProxyModel.get());
  }
}

void ControllerRelationImpl::registerChildController(const ItemModel::RelationFilterExpression & conditions)
{
  Q_ASSERT(parentController() != nullptr);
  Q_ASSERT(childController() != nullptr);

  if(childController()->sourceModel() != nullptr){
    mProxyModel->setSourceModel( childController()->sourceModel() );
  }
  mChildSourceModelChangedConnection = QObject::connect(childController(), &AbstractController::sourceModelChanged, mProxyModel.get(), &RelationFilterProxyModel::setSourceModel);
  childController()->prependProxyModel(mProxyModel.get());
  mProxyModel->setFilter(conditions);
  mParentModelCurrentRowChangedConnection1 = QObject::connect(parentController(), &AbstractController::currentRowChanged, mProxyModel.get(), &RelationFilterProxyModel::setParentModelMatchRow);
  mParentModelCurrentRowChangedConnection2 = QObject::connect(parentController(), &AbstractController::currentRowChanged, mKeyCopier.get(), &RelationKeyCopier::setParentModelCurrentRow);
  mProxyModel->setParentModelMatchRow( parentController()->currentRow() );
}

void ControllerRelationImpl::unregisterChildController()
{
  Q_ASSERT(parentController() != nullptr);
  Q_ASSERT(childController() != nullptr);

  QObject::disconnect(mParentModelCurrentRowChangedConnection1);
  QObject::disconnect(mParentModelCurrentRowChangedConnection2);
  QObject::disconnect(mChildSourceModelChangedConnection);
  childController()->removeProxyModel(mProxyModel.get());
}

void ControllerRelationImpl::setParentControllerModelToChildController()
{
  Q_ASSERT(parentController() != nullptr);
  Q_ASSERT(childController() != nullptr);

  auto *model = parentController()->modelForView();
  if(model == nullptr){
    return;
  }
//   childController()->setRelationFilterParentModel(model);
}

}} // namespace Mdt{ namespace ItemEditor{
