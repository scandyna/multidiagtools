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
#ifndef MDT_ITEM_EDITOR_CONTROLLER_STATE_TABLE_ROW_H
#define MDT_ITEM_EDITOR_CONTROLLER_STATE_TABLE_ROW_H

#include "ControllerState.h"
#include "ControllerEvent.h"

namespace Mdt{ namespace ItemEditor{

  /*! \brief Row of a ControllerStateTable
   */
  class ControllerStateTableRow
  {
   public:

    /*! \brief Constructor
     */
    explicit constexpr ControllerStateTableRow(ControllerState state, ControllerEvent event, ControllerState targetState) noexcept
     : mState(state),
       mEvent(event),
       mTargetState(targetState)
    {
    }

    /*! \brief Get state
     */
    constexpr ControllerState state() const noexcept
    {
      return mState;
    }

    /*! \brief Get event
     */
    constexpr ControllerEvent event() const noexcept
    {
      return mEvent;
    }

    /*! \brief Get target state
     */
    constexpr ControllerState targetState() const noexcept
    {
      return mTargetState;
    }

    /*! \brief Check if this row can handle given event
     */
    constexpr bool canHandleEvent(ControllerEvent event) const noexcept
    {
      return (event == mEvent);
    }

   private:

    ControllerState mState;
    ControllerEvent mEvent;
    ControllerState mTargetState;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_CONTROLLER_STATE_TABLE_ROW_H
