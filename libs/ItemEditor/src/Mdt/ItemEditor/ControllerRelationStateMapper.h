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
#ifndef MDT_ITEM_EDITOR_CONTROLLER_RELATION_STATE_MAPPER_H
#define MDT_ITEM_EDITOR_CONTROLLER_RELATION_STATE_MAPPER_H

#include "ControllerState.h"

namespace Mdt{ namespace ItemEditor{

  /*! \brief Mapping between state of a parent controller and a child controller
   */
  class ControllerRelationStateMapper
  {
   public:

    /*! \brief Get parent controller state for given child controller state
     */
    static ControllerState parentControllerState(ControllerState childControllerState);

    /*! \brief Get child controller state for given parent controller state
     */
    static ControllerState childControllerState(ControllerState parentControllerState);
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_CONTROLLER_RELATION_STATE_MAPPER_H
