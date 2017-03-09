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
#ifndef MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_STATE_PERMISSION_H
#define MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_STATE_PERMISSION_H

#include "ControllerState.h"

namespace Mdt{ namespace ItemEditor{

  /*! \brief Base class to implement constroller state permission
   *
   * Implements permissions regarding controller state for all actions.
   *  While implementing a concrete controller, which needs other permission for certain action,
   *  subclass AbstractControllerStatePermission.
   */
  class AbstractControllerStatePermission
  {
   public:

    /*! \brief Constructor
     */
    AbstractControllerStatePermission();

    /*! \brief Destructor
     */
    virtual ~AbstractControllerStatePermission();

    // Copy disabled
    AbstractControllerStatePermission(const AbstractControllerStatePermission &) = delete;
    AbstractControllerStatePermission & operator=(const AbstractControllerStatePermission &) = delete;
    // Move disabled
    AbstractControllerStatePermission(AbstractControllerStatePermission &&) = delete;
    AbstractControllerStatePermission & operator=(AbstractControllerStatePermission &&) = delete;

    /*! \brief Check if it is allowed to change current row for state
     */
    virtual bool canChangeCurrentRow(ControllerState state) const;

    /*! \brief Check if it is allowed to insert for state
     */
    virtual bool canInsert(ControllerState state) const;

    /*! \brief Check if it is possible to submit for state
     */
    virtual bool canSubmit(ControllerState state) const;

    /*! \brief Check if it is possible to revert for state
     */
    virtual bool canRevert(ControllerState state) const;

    /*! \brief Check if it is possible to remove for state
     */
    virtual bool canRemove(ControllerState state) const;

    /*! \brief Check if it is possible to select data from database
     */
    virtual bool canSelect(ControllerState state) const;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif //#ifndef MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_STATE_PERMISSION_H
