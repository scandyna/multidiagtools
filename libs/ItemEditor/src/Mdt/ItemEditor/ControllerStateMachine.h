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
#ifndef MDT_ITEM_EDITOR_CONTROLLER_STATE_MACHINE_H
#define MDT_ITEM_EDITOR_CONTROLLER_STATE_MACHINE_H

#include "ControllerState.h"
#include "ControllerStatePermission.h"
#include <memory>
#include <type_traits>

namespace Mdt{ namespace ItemEditor{

  class AbstractControllerStateChain;

  /*! \brief Synchronous state machine for a item editor controller
   *
   * ControllerStateMachine is used by AbstractController.
   *  AbstractController receives events from the GUI,
   *  but can also be driven by code.
   *  Experience showed that the latter does not work well with
   *  a asynchronous state machine, this why a existing framework
   *  (like QStateMachine) is not used here.
   *
   * ControllerStateMachine is divided in 2 parts:
   *  - Check if this state machine can accept a event,
   *     which is driven by ControllerStatePermission
   *  - Deducing next state regarding current state and the event,
   *     which is driven by AbstractControllerStateChain (or a subclass of it).
   */
  class ControllerStateMachine
  {
   public:

    /*! \brief Contruct a null ControllerStateMachine
     */
    ControllerStateMachine() = default;

    /*! \brief Destruct this ControllerStateMachine
     */
    ~ControllerStateMachine() = default;

    /*! \brief Copy construct a ControllerStateMachine from other
     */
    ControllerStateMachine(const ControllerStateMachine & other) = default;

    /*! \brief Assign other to this ControllerStateMachine
     */
    ControllerStateMachine & operator=(const ControllerStateMachine & other) = default;

    /*! \brief Move construct a ControllerStateMachine from other
     */
    ControllerStateMachine(ControllerStateMachine && other) = default;

    /*! \brief Assign other to this ControllerStateMachine
     */
    ControllerStateMachine & operator=(ControllerStateMachine && other) = default;

    /*! \brief Check if it is allowed to change current row for current state
     */
    bool canChangeCurrentRow() const;

    /*! \brief Check if it is allowed to insert for current state
     */
    bool canInsert() const;

    /*! \brief Check if it is possible to edit for current state
     */
    bool canEdit() const;

    /*! \brief Check if it is possible to submit for current state
     */
    bool canSubmit() const;

    /*! \brief Check if it is possible to revert for current state
     */
    bool canRevert() const;

    /*! \brief Check if it is possible to remove for current state
     */
    bool canRemove() const;

    /*! \brief Check if it is possible to select data from database for current state
     */
    bool canSelect() const;

    /*! \brief Data edition started event
     */
    void onDataEditionStarted();

    /*! \brief Construct a ControllerStateMachine with a concrete state chain and permission implementations
     *
     * \code
     * auto stateMachine = ControllerStateMachine::make<MyStateChain, MyPermission>();
     * \endcode
     *
     * \pre ChainImpl must be a subclass of AbstractControllerStateChain
     * \pre PermissionImpl must be a subclass of AbstractControllerStatePermission
     */
    template<typename ChainImpl, typename PermissionImpl>
    static ControllerStateMachine make()
    {
      static_assert( std::is_base_of<AbstractControllerStateChain, ChainImpl>::value, "Type ChainImpl must be a subclass of Mdt::ItemEditor::AbstractControllerStateChain" );
      static_assert( std::is_base_of<AbstractControllerStatePermission, PermissionImpl>::value, "Type PermissionImpl must be a subclass of Mdt::ItemEditor::AbstractControllerStatePermission" );
      return ControllerStateMachine( std::make_shared<ChainImpl>() , ControllerStatePermission::make<PermissionImpl>() );
    }

  private:

    ControllerState currentState() const;

    template<typename ChainImpl>
    ControllerStateMachine(const std::shared_ptr<ChainImpl> & chainImpl, const ControllerStatePermission & permission)
     : mChainImpl(chainImpl),
       mPermission(permission)
    {
    }

    std::shared_ptr<const AbstractControllerStateChain> mChainImpl;
    ControllerStatePermission mPermission;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_CONTROLLER_STATE_MACHINE_H
