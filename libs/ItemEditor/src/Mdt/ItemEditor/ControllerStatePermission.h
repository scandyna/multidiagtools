/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_ITEM_EDITOR_CONTROLLER_STATE_PERMISSION_H
#define MDT_ITEM_EDITOR_CONTROLLER_STATE_PERMISSION_H

#include "ControllerState.h"

namespace Mdt{ namespace ItemEditor{

  /*! \brief Check which actions are possible or not for a given state
   *
   * ControllerStatePermission is the central part that contains the logic to decide
   *  what is possible or not in a certain state.
   */
  class ControllerStatePermission
  {
   public:

    /*! \brief Check if it is allowed to change current row for state
     */
    static bool canChangeCurrentRow(ControllerState state);

    /*! \brief Check if it is allowed to insert for state
     */
    static bool canInsert(ControllerState state);

    /*! \brief Check if it is possible to submit for state
     */
    static bool canSubmit(ControllerState state);

    /*! \brief Check if it is possible to revert for state
     */
    static bool canRevert(ControllerState state);

    /*! \brief Check if it is possible to remove for state
     */
    static bool canRemove(ControllerState state);

    /*! \brief Check if it is possible to select data from database
     */
    static bool canSelect(ControllerState state);
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_CONTROLLER_STATE_PERMISSION_H
