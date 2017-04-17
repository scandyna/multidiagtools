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
#include "SqlTableModelControllerTester.h"
#include "Mdt/ItemEditor/AbstractControllerStateTable.h"
#include "Mdt/ItemEditor/AbstractControllerStatePermission.h"

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
    addTransition(ControllerState::Visualizing, ControllerEvent::DisableController, ControllerState::Disabled);
    addTransition(ControllerState::Editing, ControllerEvent::SubmitDone, ControllerState::Visualizing);
    addTransition(ControllerState::Editing, ControllerEvent::RevertDone, ControllerState::Visualizing);
    addTransition(ControllerState::Inserting, ControllerEvent::SubmitDone, ControllerState::Visualizing);
    addTransition(ControllerState::Inserting, ControllerEvent::RevertDone, ControllerState::Visualizing);
    addTransition(ControllerState::ParentEditing, ControllerEvent::EditionDoneFromParent, ControllerState::Visualizing);
    addTransition(ControllerState::ChildEditing, ControllerEvent::EditionDoneFromChild, ControllerState::Visualizing);
    addTransition(ControllerState::Disabled, ControllerEvent::EnableController, ControllerState::Visualizing);
  }
};

SqlTableModelControllerTester::SqlTableModelControllerTester(QObject* parent, Mdt::ItemEditor::ControllerStateMachine* stateMachine)
 : AbstractSqlTableModelController(parent)
{
  if(stateMachine == nullptr){
    setControllerStateMachine( ControllerStateMachine::makeNew<StateTableTestClass, AbstractControllerStatePermission>(this) );
  }else{
    setControllerStateMachine( stateMachine );
  }
}

SqlTableModelControllerTester::SqlTableModelControllerTester(Mdt::ItemEditor::ControllerStateMachine* stateMachine)
 : SqlTableModelControllerTester(nullptr, stateMachine)
{
}

void SqlTableModelControllerTester::setModelToView(QAbstractItemModel* /*model*/)
{
  modelSetToView();
}

bool SqlTableModelControllerTester::setDataToModel()
{
  return true;
}

void SqlTableModelControllerTester::revertDataFromModel()
{
}
