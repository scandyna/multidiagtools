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
   *
   * A controller will check if it can do a action calling a can method
   *  (for example canChangeCurrentRow() ).
   *
   * Actions (see AbstractActionContainer) will set their enable state calling a is..ActionEnabled method
   *  (for example isChangeCurrentRowActionEnabled() ).
   *
   * For common cases, a can method and a is..ActionEnabled method will return the same result.
   *  When many controllers are related (parent/child relations), this can change.
   *  For example, if we start editing in a parent controller,
   *  we not want the submit action be enabled in the child controller (this is not intuitive),
   *  but submit must be possible in the child controller.
   *
   * \sa ControllerStatePermission
   */
  class AbstractControllerStatePermission
  {
   public:

    /*! \brief Constructor
     */
    AbstractControllerStatePermission() = default;

    /*! \brief Destructor
     */
    virtual ~AbstractControllerStatePermission() = default;

    // Copy disabled
    AbstractControllerStatePermission(const AbstractControllerStatePermission &) = delete;
    AbstractControllerStatePermission & operator=(const AbstractControllerStatePermission &) = delete;
    // Move disabled
    AbstractControllerStatePermission(AbstractControllerStatePermission &&) = delete;
    AbstractControllerStatePermission & operator=(AbstractControllerStatePermission &&) = delete;

    /*! \brief Check if it is allowed to change current row for state
     */
    virtual bool canChangeCurrentRow(ControllerState state) const;

    /*! \brief Check if changing current row action is enabled for state
     */
    virtual bool isChangeCurrentRowActionEnabled(ControllerState state) const;

    /*! \brief Check if it is allowed to insert for state
     */
    virtual bool canInsert(ControllerState state) const;

    /*! \brief Check if insert action is enabled for state
     */
    virtual bool isInsertActionEnabled(ControllerState state) const;

    /*! \brief Check if it is possible to edit for state
     */
    virtual bool canEdit(ControllerState state) const;

    /*! \brief Check if it is possible to submit for state
     */
    virtual bool canSubmit(ControllerState state) const;

    /*! \brief Check if submit action is enabled for state
     */
    virtual bool isSubmitActionEnabled(ControllerState state) const;

    /*! \brief Check if it is possible to revert for state
     */
    virtual bool canRevert(ControllerState state) const;

    /*! \brief Check if revert action is enabled for state
     */
    virtual bool isRevertActionEnabled(ControllerState state) const;

    /*! \brief Check if it is possible to remove for state
     */
    virtual bool canRemove(ControllerState state) const;

    /*! \brief Check if remove action is enabled for state
     */
    virtual bool isRemoveActionEnabled(ControllerState state) const;

    /*! \brief Check if it is possible to select data from database
     */
    virtual bool canSelect(ControllerState state) const;

    /*! \brief Check if select action is enabled for state
     */
    virtual bool isSelectActionEnabled(ControllerState state) const;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif //#ifndef MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_STATE_PERMISSION_H
