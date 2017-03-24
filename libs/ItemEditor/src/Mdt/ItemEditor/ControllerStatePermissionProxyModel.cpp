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
#include "ControllerStatePermissionProxyModel.h"
#include "AbstractControllerStateChain.h"
#include <QIdentityProxyModel>

namespace Mdt{ namespace ItemEditor{

ControllerStatePermissionProxyModel::ControllerStatePermissionProxyModel(QObject* parent)
 : QIdentityProxyModel(parent)
{
}

void ControllerStatePermissionProxyModel::setStateMachine(const ControllerStateMachine * const stateMachine)
{
  Q_ASSERT(stateMachine != nullptr);

  mStateMachine = stateMachine;
  disconnect(mStateChangedConnection);
//   mStateChangedConnection = connect(mStateMachine.stateChain(), &AbstractControllerStateChain::currentStateChanged, this, &ControllerStatePermissionProxyModel::onCurrentStateChanged);
}

Qt::ItemFlags ControllerStatePermissionProxyModel::flags(const QModelIndex& index) const
{
  return QIdentityProxyModel::flags(index);
}

void ControllerStatePermissionProxyModel::onCurrentStateChanged()
{

}

}} // namespace Mdt{ namespace ItemEditor{
