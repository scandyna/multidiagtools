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
#ifndef MDT_ITEM_EDITOR_CONTROLLER_STATE_PERMISSION_H
#define MDT_ITEM_EDITOR_CONTROLLER_STATE_PERMISSION_H

#include "ControllerState.h"
#include <memory>
#include <type_traits>

namespace Mdt{ namespace ItemEditor{

  class AbstractControllerStatePermission;

  /*! \brief Check which actions are possible or not for a given state
   *
   * ControllerStatePermission is the central part that contains the logic to decide
   *  what is possible or not in a certain state.
   *
   * \sa AbstractControllerStatePermission
   */
  class ControllerStatePermission
  {
   public:

    /*! \brief Contruct a null ControllerStatePermission
     */
    ControllerStatePermission() = default;

    /*! \brief Destruct this ControllerStatePermission
     */
    ~ControllerStatePermission() = default;

    /*! \brief Copy construct a ControllerStatePermission from other
     */
    ControllerStatePermission(const ControllerStatePermission & other) = default;

    /*! \brief Assign other to this ControllerStatePermission
     */
    ControllerStatePermission & operator=(const ControllerStatePermission & other) = default;

    /*! \brief Move construct a ControllerStatePermission from other
     */
    ControllerStatePermission(ControllerStatePermission && other) = default;

    /*! \brief Assign other to this ControllerStatePermission
     */
    ControllerStatePermission & operator=(ControllerStatePermission && other) = default;

    /*! \brief Construct a ControllerStatePermission with a concrete implementation
     *
     * \pre Impl must be a subclass of AbstractControllerStatePermission
     */
    template<typename Impl>
    static ControllerStatePermission make()
    {
      static_assert( std::is_base_of<AbstractControllerStatePermission, Impl>::value, "Type Impl must be a subclass of Mdt::ItemEditor::AbstractControllerStatePermission" );
      return ControllerStatePermission( std::make_shared<Impl>() );
    }

    /*! \brief Check if it is allowed to change current row for state
     */
    bool canChangeCurrentRow(ControllerState state) const;

    /*! \brief Check if changing current row action is enabled for state
     */
    bool isChangeCurrentRowActionEnabled(ControllerState state) const;

    /*! \brief Check if it is allowed to insert for state
     */
    bool canInsert(ControllerState state) const;

    /*! \brief Check if insert action is enabled for state
     */
    bool isInsertActionEnabled(ControllerState state) const;

    /*! \brief Check if it is possible to submit for state
     */
    bool canSubmit(ControllerState state) const;

    /*! \brief Check if submit action is enabled for state
     */
    bool isSubmitActionEnabled(ControllerState state) const;

    /*! \brief Check if it is possible to revert for state
     */
    bool canRevert(ControllerState state) const;

    /*! \brief Check if revert action is enabled for state
     */
    bool isRevertActionEnabled(ControllerState state) const;

    /*! \brief Check if it is possible to remove for state
     */
    bool canRemove(ControllerState state) const;

    /*! \brief Check if remove action is enabled for state
     */
    bool isRemoveActionEnabled(ControllerState state) const;

    /*! \brief Check if it is possible to select data from database
     */
    bool canSelect(ControllerState state) const;

    /*! \brief Check if select action is enabled for state
     */
    bool isSelectActionEnabled(ControllerState state) const;

  private:

    template<typename Impl>
    ControllerStatePermission(const std::shared_ptr<Impl> & impl)
     : mImpl(impl)
    {
    }

    std::shared_ptr<const AbstractControllerStatePermission> mImpl;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_CONTROLLER_STATE_PERMISSION_H
