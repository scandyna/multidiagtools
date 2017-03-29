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
#ifndef MDT_ITEM_EDITOR_TABLE_VIEW_CONTROLLER_STATE_PERMISSION_H
#define MDT_ITEM_EDITOR_TABLE_VIEW_CONTROLLER_STATE_PERMISSION_H

#include "AbstractControllerStatePermission.h"

namespace Mdt{ namespace ItemEditor{

  class AbstractControllerStateTable;

  /*! \brief Controller state permission implementation for TableViewController
   */
  class TableViewControllerStatePermission : public AbstractControllerStatePermission
  {
   public:

    /*! \brief Check if it is allowed to insert for state
     */
    bool canInsert(const AbstractControllerStateTable & st) const override;

    /*! \brief Check if it is allowed to change current row
     */
    bool canChangeCurrentRow(const AbstractControllerStateTable & st) const override;

    /*! \brief Check if submit action is enabled
     */
    bool isSubmitActionEnabled(const AbstractControllerStateTable & st) const override;

    /*! \brief Check if it is possible to revert
     */
    bool canRevert(const AbstractControllerStateTable & st) const override;

    /*! \brief Check if revert action is enabled
     */
    bool isRevertActionEnabled(const AbstractControllerStateTable & st) const override;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_TABLE_VIEW_CONTROLLER_STATE_PERMISSION_H
