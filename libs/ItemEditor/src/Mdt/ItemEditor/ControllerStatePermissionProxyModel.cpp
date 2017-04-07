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
#include <QIdentityProxyModel>

// #include "Debug.h"

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
  mStateChangedConnection =
   connect( mStateMachine, &ControllerStateMachine::currentStateChanged, this, &ControllerStatePermissionProxyModel::flagsChanged );
  emit flagsChanged();
}

Qt::ItemFlags ControllerStatePermissionProxyModel::flags(const QModelIndex & index) const
{
  if(mStateMachine.isNull()){
    return unsetFlag( QIdentityProxyModel::flags(index), Qt::ItemIsEditable );
  }
  if(mStateMachine->canEdit()){
    return setFlag( QIdentityProxyModel::flags(index), Qt::ItemIsEditable );
  }
  return unsetFlag( QIdentityProxyModel::flags(index), Qt::ItemIsEditable );
}

Qt::ItemFlags ControllerStatePermissionProxyModel::setFlag(Qt::ItemFlags flags, Qt::ItemFlag flag)
{
  return flags | flag;
}

Qt::ItemFlags ControllerStatePermissionProxyModel::unsetFlag(Qt::ItemFlags flags, Qt::ItemFlag flag)
{
  return flags & Qt::ItemFlags(~flag);
}

}} // namespace Mdt{ namespace ItemEditor{
