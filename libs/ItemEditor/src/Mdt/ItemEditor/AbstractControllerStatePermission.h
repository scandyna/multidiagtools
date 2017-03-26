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

  class AbstractControllerStateTable;

  /*! \brief Base class to implement constroller state permission
   *
   * Implements permissions regarding controller state table for all actions.
   *  While implementing a concrete controller, which needs other permission for certain action,
   *  subclass AbstractControllerStatePermission.
   *
   * A ControllerStateMachine will check if it can do a action calling a can method
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

    /*! \brief Check if it is allowed to change current row
     */
    virtual bool canChangeCurrentRow(const AbstractControllerStateTable & st) const;

    /*! \brief Check if changing current row action is enabled
     */
    virtual bool isChangeCurrentRowActionEnabled(const AbstractControllerStateTable & st) const;

    /*! \brief Check if it is allowed to insert
     */
    virtual bool canInsert(const AbstractControllerStateTable & st) const;

    /*! \brief Check if insert action is enabled
     */
    virtual bool isInsertActionEnabled(const AbstractControllerStateTable & st) const;

    /*! \brief Check if it is possible to edit
     */
    virtual bool canEdit(const AbstractControllerStateTable & st) const;

    /*! \brief Check if it is possible to submit
     */
    virtual bool canSubmit(const AbstractControllerStateTable & st) const;

    /*! \brief Check if submit action is enabled
     */
    virtual bool isSubmitActionEnabled(const AbstractControllerStateTable & st) const;

    /*! \brief Check if it is possible to revert
     */
    virtual bool canRevert(const AbstractControllerStateTable & st) const;

    /*! \brief Check if revert action is enabled
     */
    virtual bool isRevertActionEnabled(const AbstractControllerStateTable & st) const;

    /*! \brief Check if it is possible to remove
     */
    virtual bool canRemove(const AbstractControllerStateTable & st) const;

    /*! \brief Check if remove action is enabled
     */
    virtual bool isRemoveActionEnabled(const AbstractControllerStateTable & st) const;

    /*! \brief Check if it is possible to select data from database
     */
    virtual bool canSelect(const AbstractControllerStateTable & st) const;

    /*! \brief Check if select action is enabled
     */
    virtual bool isSelectActionEnabled(const AbstractControllerStateTable & st) const;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif //#ifndef MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_STATE_PERMISSION_H
