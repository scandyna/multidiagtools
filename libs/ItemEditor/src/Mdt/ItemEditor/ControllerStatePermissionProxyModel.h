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
#include <QPointer>
#include <QMetaObject>
#include <Qt>

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

    /*! \brief Set controller state machine
     *
     * \pre \a stateMachine must be a valid pointer
     */
    void setStateMachine(const ControllerStateMachine * const stateMachine);

    /*! \brief Get flags
     */
    Qt::ItemFlags flags(const QModelIndex & index) const override;

   signals:

    /*! \brief Emitted whenever flags changed
     *
     * Can be used by views to update the editable state of their components.
     *  This signal is used by DataWidgetMapper and WidgetMapperController.
     *
     * \note Common views based on QAbstractItemView (such as QTableView)
     *        will check models's flags before editing, so they do not need this signal.
     */
    void flagsChanged();

   private:

    static Qt::ItemFlags setFlag(Qt::ItemFlags flags, Qt::ItemFlag flag);
    static Qt::ItemFlags unsetFlag(Qt::ItemFlags flags, Qt::ItemFlag flag);

    QPointer<const ControllerStateMachine> mStateMachine;
    QMetaObject::Connection mStateChangedConnection;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_CONTROLLER_STATE_PERMISSION_PROXY_MODEL_H
