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
#include "Mdt/ItemEditor/AbstractControllerStateTable.h"
#include "Mdt/ItemEditor/AbstractControllerStatePermission.h"
#include "Mdt/ItemEditor/ControllerStateMachine.h"
#include <QAbstractItemModel>
#include <QModelIndex>

using namespace Mdt::ItemEditor;

class StateTableTestClass : public AbstractControllerStateTable
{
 public:

  void createTable() override
  {
    addTransition(ControllerState::Visualizing, ControllerEvent::DataEditionStarted, ControllerState::Editing);
    addTransition(ControllerState::Visualizing, ControllerEvent::InsertStarted, ControllerState::Inserting);
    addTransition(ControllerState::Visualizing, ControllerEvent::EditionStartedFromParent, ControllerState::ParentEditing);
    addTransition(ControllerState::Visualizing, ControllerEvent::EditionStartedFromChild, ControllerState::ChildEditing);
    addTransition(ControllerState::Editing, ControllerEvent::SubmitDone, ControllerState::Visualizing);
    addTransition(ControllerState::Editing, ControllerEvent::RevertDone, ControllerState::Visualizing);
    addTransition(ControllerState::Inserting, ControllerEvent::SubmitDone, ControllerState::Visualizing);
    addTransition(ControllerState::Inserting, ControllerEvent::RevertDone, ControllerState::Visualizing);
    addTransition(ControllerState::ParentEditing, ControllerEvent::EditionDoneFromParent, ControllerState::Visualizing);
    addTransition(ControllerState::ChildEditing, ControllerEvent::EditionDoneFromChild, ControllerState::Visualizing);
  }
};


ItemModelControllerTester::ItemModelControllerTester(QObject* parent)
 : AbstractItemModelController(parent)
{
  setControllerStateMachine( ControllerStateMachine::makeNew<StateTableTestClass, AbstractControllerStatePermission>(this) );
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

bool ItemModelControllerTester::setDataToModel()
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
