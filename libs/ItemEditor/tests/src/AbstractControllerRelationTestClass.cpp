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
#include "AbstractControllerRelationTestClass.h"
#include "Mdt/ItemEditor/AbstractController.h"
#include <QAbstractItemModel>

using namespace Mdt::ItemEditor;

AbstractControllerRelationTestClass::AbstractControllerRelationTestClass(QObject* parent)
 : AbstractControllerRelation(parent)
{
}

AbstractController* AbstractControllerRelationTestClass::storedOldParentController() const
{
  return mOldParentController;
}

AbstractController* AbstractControllerRelationTestClass::storedOldChildController() const
{
  return mOldChildController;
}

AbstractController* AbstractControllerRelationTestClass::storedParentController() const
{
  return mParentController;
}

AbstractController* AbstractControllerRelationTestClass::storedChildController() const
{
  return mChildController;
}

QAbstractItemModel* AbstractControllerRelationTestClass::parentControllerStoredModel() const
{
  return mParentModel;
}

QAbstractItemModel* AbstractControllerRelationTestClass::childControllerStoredModel() const
{
  return mChildModel;
}

void AbstractControllerRelationTestClass::parentControllerAboutToChangeEvent(AbstractController* oldController)
{
  mOldParentController = oldController;
  Q_ASSERT(!mOldParentController.isNull());
}

void AbstractControllerRelationTestClass::parentControllerChangedEvent(AbstractController* controller)
{
  mParentController = controller;
  Q_ASSERT(!mParentController.isNull());
}

void AbstractControllerRelationTestClass::childControllerAboutToChangeEvent(AbstractController* oldController)
{
  mOldChildController = oldController;
  Q_ASSERT(!mOldChildController.isNull());
}

void AbstractControllerRelationTestClass::childControllerChangedEvent(AbstractController* controller)
{
  mChildController = controller;
  Q_ASSERT(!mChildController.isNull());
}

void AbstractControllerRelationTestClass::parentControllerModelChangedEvent(QAbstractItemModel* model)
{
  mParentModel = model;
  Q_ASSERT(!mParentModel.isNull());
}

void AbstractControllerRelationTestClass::childControllerModelChangedEvent(QAbstractItemModel* model)
{
  mChildModel = model;
  Q_ASSERT(!mChildModel.isNull());
}
