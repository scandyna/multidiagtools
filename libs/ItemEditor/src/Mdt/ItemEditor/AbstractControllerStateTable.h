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
#ifndef MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_STATE_TABLE_H
#define MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_STATE_TABLE_H

#include "ControllerStateTableRow.h"
#include <vector>

namespace Mdt{ namespace ItemEditor{

  /*! \brief AbstractControllerStateTable is the transition table for ControllerStateMachine
   *
   * This is the implementation for AbstractController.
   */
  class AbstractControllerStateTable
  {
    using const_iterator = std::vector<ControllerStateTableRow>::const_iterator;

   public:

    /*! \brief Default constructor
     */
    AbstractControllerStateTable() = default;

    /*! \brief Destructor
     */
    virtual ~AbstractControllerStateTable() = default;

    // Copy disabled
    AbstractControllerStateTable(const AbstractControllerStateTable &) = delete;
    AbstractControllerStateTable & operator=(const AbstractControllerStateTable &) = delete;
    // Move disabled
    AbstractControllerStateTable(AbstractControllerStateTable &&) = delete;
    AbstractControllerStateTable & operator=(AbstractControllerStateTable &&) = delete;

    /*! \internal Force current state
     *
     * \note This is used for unit tests.
     */
    void forceCurrentState(ControllerState state);

    /*! \brief Get current state
     */
    ControllerState currentState() const noexcept
    {
      return mCurrentState;
    }

    /*! \brief Check if a event can be handled in current state
     */
    bool canHandleEvent(ControllerEvent event) const noexcept;

    /*! \brief Set event
     *
     * Current state will be updated regarding current state,
     *  event and the transition table.
     */
    void setEvent(ControllerEvent event) noexcept;

    /*! \brief Create transition table
     */
    virtual void createTable() = 0;

   protected:

    /*! \brief Add a transtion to the table
     */
    void addTransition(ControllerState state, ControllerEvent event, ControllerState targetState);

   private:

    const_iterator findTransitionFromCurrentState(ControllerEvent event) const;

    ControllerState mCurrentState = ControllerState::Visualizing;
    std::vector<ControllerStateTableRow> mTable;
  };


}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_STATE_TABLE_H
