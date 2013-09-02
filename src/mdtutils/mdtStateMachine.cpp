/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtStateMachine.h"
#include "mdtState.h"
#include <QTimer>
#include <QCoreApplication>
#include <QEventLoop>

mdtStateMachine::mdtStateMachine(QObject *parent)
 : QStateMachine(parent)
{
  pvTimeoutTimer = new QTimer(this);
  pvTimeoutTimer->setSingleShot(true);
  pvTimeoutOccured = false;
  connect(pvTimeoutTimer, SIGNAL(timeout()), this, SLOT(onWaitTimeout()));
  pvCurrentState = -1;
}

mdtStateMachine::~mdtStateMachine()
{
}

void mdtStateMachine::addState(mdtState *state)
{
  Q_ASSERT(state != 0);

  connect(state, SIGNAL(entered(mdtState*)), this, SLOT(onStateEntered(mdtState*)));
  QStateMachine::addState(state);
}

int mdtStateMachine::currentState() const
{
  return pvCurrentState;
}

bool mdtStateMachine::waitOnState(int state, int timeout)
{
  pvTimeoutOccured = false;
  if(pvCurrentState == state){
    return true;
  }
  if(timeout > 0){
    pvTimeoutTimer->start(timeout);
  }
  while(pvCurrentState != state){
    if(pvTimeoutOccured){
      return false;
    }
    QCoreApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);
  }
  pvTimeoutTimer->stop();

  return true;
}

void mdtStateMachine::onStateEntered(mdtState *state)
{
  Q_ASSERT(state != 0);

  pvCurrentState = state->id();
  emit stateChanged(pvCurrentState);
  if(state->notifyEnteredToUi()){
    emit stateChangedForUi(pvCurrentState, state->text());
  }
}

void mdtStateMachine::onWaitTimeout()
{
  pvTimeoutOccured = true;
}

