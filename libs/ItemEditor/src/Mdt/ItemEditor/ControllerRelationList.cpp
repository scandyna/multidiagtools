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
#include "ControllerRelationList.h"
#include "AbstractController.h"

// #include "AbstractItemModelController.h"

#include <QtGlobal>

namespace Mdt{ namespace ItemEditor{

ControllerRelationList::ControllerRelationList(AbstractController* parentController)
 : mParentController(parentController)
{
  Q_ASSERT(!mParentController.isNull());
}

ControllerRelationList::~ControllerRelationList()
{
  clearRelations();
}

void ControllerRelationList::addChildController(AbstractController* controller, const ItemModel::RelationFilterExpression& conditions)
{
  Q_ASSERT(controller != nullptr);

  auto *relation = new ControllerRelation(mParentController);
  relation->setChildController(controller, conditions);
  mList.push_back(relation);
}

void ControllerRelationList::clearRelations()
{
  for(auto relation : mList){
    delete relation;
  }
  mList.clear();
}


}} // namespace Mdt{ namespace ItemEditor{
