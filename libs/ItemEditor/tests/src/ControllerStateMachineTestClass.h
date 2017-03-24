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
#ifndef MDT_ITEM_EDITOR_CONTROLLER_STATE_MACHINE_TEST_CLASS_H
#define MDT_ITEM_EDITOR_CONTROLLER_STATE_MACHINE_TEST_CLASS_H

#include "ControllerStateChainTestClass.h"
#include "Mdt/ItemEditor/ControllerStateMachine.h"

class ControllerStateMachineTestClass : public Mdt::ItemEditor::ControllerStateMachine
{
 public:

  void setCurrentState(Mdt::ItemEditor::ControllerState state);

  template<typename ChainImpl, typename PermissionImpl>
  static ControllerStateMachineTestClass make()
  {
    return static_cast<ControllerStateMachineTestClass>( ControllerStateMachine::make<ChainImpl, PermissionImpl>() );
  }

};

#endif // #ifndef MDT_ITEM_EDITOR_CONTROLLER_STATE_MACHINE_TEST_CLASS_H
