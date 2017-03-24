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
#include "ItemModelControllerTester.h"
#include "ControllerStateMachineTestClass.h"
#include "ControllerStateChainTestClass.h"
#include "Mdt/ItemEditor/ControllerStatePermission.h"
#include "Mdt/ItemEditor/AbstractControllerStatePermission.h"
#include <QAbstractItemModel>
#include <QModelIndex>

using namespace Mdt::ItemEditor;

ItemModelControllerTester::ItemModelControllerTester(QObject* parent)
 : AbstractItemModelController(parent)
{
  setControllerStateMachine( ControllerStateMachineTestClass::make<ControllerStateChainTestClass, AbstractControllerStatePermission>() );
//   setControllerStatePermission( ControllerStatePermission::make<AbstractControllerStatePermission>() );
}

int ItemModelControllerTester::primaryKeyChangedEventCount() const
{
  return mPrimaryKeyChangedEventCount;
}

void ItemModelControllerTester::clearPrimaryKeyChangedEventCount()
{
  mPrimaryKeyChangedEventCount = 0;
}

void ItemModelControllerTester::startEditing()
{
  onDataEditionStarted();
}

void ItemModelControllerTester::stopEditing()
{
  onDataEditionDone();
}

int ItemModelControllerTester::dataSubmitToModelCount() const
{
  return mDataSubmitToModelCount;
}

void ItemModelControllerTester::clearDataSubmitToModelCount()
{
  mDataSubmitToModelCount = 0;
}

int ItemModelControllerTester::dataRevertFromModelCount() const
{
  return mDataRevertFromModelCount;
}

void ItemModelControllerTester::clearDataRevertFromModelCount()
{
  mDataRevertFromModelCount = 0;
}

// bool ItemModelControllerTester::setModelData(int column, const QVariant& data)
// {
//   auto *model = sourceModel();
//   Q_ASSERT(model != nullptr);
//   const auto index = model->index(currentRow(), column);
//   Q_ASSERT(index.isValid());
//   return model->setData(index, data);
// }

void ItemModelControllerTester::setModelToView(QAbstractItemModel* /*model*/)
{
  modelSetToView();
}

bool ItemModelControllerTester::submitDataToModel()
{
  ++mDataSubmitToModelCount;
  return true;
}

void ItemModelControllerTester::revertDataFromModel()
{
  ++mDataRevertFromModelCount;
}


void ItemModelControllerTester::primaryKeyChangedEvent(const Mdt::ItemModel::PrimaryKey& /*oldPrimaryKey*/, const Mdt::ItemModel::PrimaryKey& /*newPrimaryKey*/)
{
  ++mPrimaryKeyChangedEventCount;
}
