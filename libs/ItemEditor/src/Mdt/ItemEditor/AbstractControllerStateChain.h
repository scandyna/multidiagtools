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
#ifndef MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_STATE_CHAIN_H
#define MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_STATE_CHAIN_H

#include "ControllerState.h"

namespace Mdt{ namespace ItemEditor{

  /*! \brief Deduce next state regarding a event
   *
   * AbstractControllerStateChain is used by ControllerStateMachine
   *  to handle state change regarding current state and a event.
   *
   * Checking if a event can occure in current state
   *  is allready done by ControllerStateMachine.
   */
  class AbstractControllerStateChain
  {
   public:

    /*! \brief Constructor
     */
    AbstractControllerStateChain() = default;

    /*! \brief Destructor
     */
    virtual ~AbstractControllerStateChain() = default;

    // Copy disabled
    AbstractControllerStateChain(const AbstractControllerStateChain &) = delete;
    AbstractControllerStateChain & operator=(const AbstractControllerStateChain &) = delete;
    // Move disabled
    AbstractControllerStateChain(AbstractControllerStateChain &&) = delete;
    AbstractControllerStateChain & operator=(AbstractControllerStateChain &&) = delete;

    /*! \brief Get state when data edition started
     */
    virtual ControllerState dataEditionStartedState(ControllerState currentState) const;

    /*! \brief Get state when data edition is done
     */
    virtual ControllerState dataEditionDoneState(ControllerState currentState) const;

    /*! \brief Get state when submit is done
     */
    virtual ControllerState submitDoneState(ControllerState currentState) const;

    /*! \brief Get state when revert is done
     */
    virtual ControllerState revertDoneState(ControllerState currentState) const;

    /*! \brief Get state when insert started
     */
    virtual ControllerState insertStartedState(ControllerState currentState) const;

    /*! \brief Get state when remove is done
     */
    virtual ControllerState removeDoneState(ControllerState currentState) const;

    /*! \brief Get state when data edition started from parent controller
     */
    virtual ControllerState dataEditionStartedFromParentState(ControllerState currentState) const;

  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_STATE_CHAIN_H
