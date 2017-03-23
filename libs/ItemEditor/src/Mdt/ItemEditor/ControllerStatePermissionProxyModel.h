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
#ifndef MDT_ITEM_EDITOR_CONTROLLER_STATE_PERMISSION_PROXY_MODEL_H
#define MDT_ITEM_EDITOR_CONTROLLER_STATE_PERMISSION_PROXY_MODEL_H

#include "ControllerStateMachine.h"
#include <QIdentityProxyModel>

namespace Mdt{ namespace ItemEditor{

  /*! \brief Alow editing or not regarding controller state permission
   */
  class ControllerStatePermissionProxyModel : public QIdentityProxyModel
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit ControllerStatePermissionProxyModel(QObject* parent = nullptr);

    /*! \brief
     */
    Qt::ItemFlags flags(const QModelIndex & index) const override;

   private:

    ControllerStateMachine mStateMachine;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_CONTROLLER_STATE_PERMISSION_PROXY_MODEL_H
