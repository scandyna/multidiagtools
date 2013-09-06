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
#include "mdtPortManagerStateMachine.h"
#include "mdtState.h"
#include "mdtPortManager.h"
#include "mdtLed.h"

mdtPortManagerStateMachine::mdtPortManagerStateMachine(QObject *parent)
 : mdtStateMachine(parent)
{
  pvStateMachine = new mdtStateMachine(this);
  // Main states
  pvPortClosed = 0;
  pvStarting = 0;
  pvRunning = 0;
  pvStopping = 0;
  pvStopped = 0;
  pvPortError = 0;
  // Running sub states
  pvPortReady = 0;
  pvConnecting = 0;
  pvConnected = 0;
  pvDisconnected = 0;
  // Connected sub states
  pvReady = 0;
  pvBusy = 0;
}

mdtPortManagerStateMachine::~mdtPortManagerStateMachine()
{
}

void mdtPortManagerStateMachine::buildMainStateMachine(mdtPortManager *portManager)
{
  Q_ASSERT(pvStateMachine != 0);
  Q_ASSERT(portManager != 0);

  // Create states
  pvPortClosed = new mdtState(mdtPortManager::PortClosed);
  pvPortClosed->setText(tr("Port closed"));
  pvPortClosed->setLedColorId(mdtLed::Green);
  pvPortClosed->setLedOn(false);
  pvStarting = new mdtState(mdtPortManager::Starting);
  pvRunning = new mdtState(mdtPortManager::Running);
  pvStopping = new mdtState(mdtPortManager::Stopping);
  pvStopped = new mdtState(mdtPortManager::Stopped);
  pvPortError = new mdtState(mdtPortManager::PortError);
  pvPortError->setText(tr("Fatal error"));
  pvPortError->setLedColorId(mdtLed::Red);
  pvPortError->setLedOn(true);
  // Add sates to machine
  pvStateMachine->addState(pvPortClosed);
  pvStateMachine->addState(pvStarting);
  pvStateMachine->addState(pvRunning);
  pvStateMachine->addState(pvStopping);
  pvStateMachine->addState(pvStopped);
  pvStateMachine->addState(pvPortError);
  // Add pvPortClosed transitions ->
  pvPortClosed->addTransition(portManager, SIGNAL(pmStartThreadsEvent()), pvStarting);
  // Add pvStarting transitions ->
  pvStarting->addTransition(portManager, SIGNAL(pmUnhandledErrorEvent()), pvPortError);
  pvStarting->addTransition(portManager, SIGNAL(pmAllThreadsReadyEvent()), pvRunning);
  // Add pvRunning transitions ->
  pvRunning->addTransition(portManager, SIGNAL(pmStopThreadsEvent()), pvStopping);
  pvRunning->addTransition(portManager, SIGNAL(pmUnhandledErrorEvent()), pvPortError);
  // Add pvStopping transitions ->
  pvStopping->addTransition(portManager, SIGNAL(pmAllThreadsStoppedEvent()), pvStopped);
  // Add pvStopped transitions ->
  pvStopped->addTransition(portManager, SIGNAL(pmPortClosedEvent()), pvPortClosed);
  // Add pvPortError transitions ->
  pvPortError->addTransition(portManager, SIGNAL(pmAllThreadsStoppedEvent()), pvStopped);
  pvPortError->addTransition(portManager, SIGNAL(pmStopThreadsEvent()), pvStopping);
  // Build Running sub machine
  buildRunningSubMachine(portManager, pvRunning);
  // Start
  pvStateMachine->setInitialState(pvPortClosed);
  pvStateMachine->start();
  pvStateMachine->waitOnState(mdtPortManager::PortClosed);
}

void mdtPortManagerStateMachine::buildRunningSubMachine(mdtPortManager *portManager, mdtState *parentState)
{
  Q_ASSERT(pvStateMachine != 0);
  Q_ASSERT(portManager != 0);
  Q_ASSERT(parentState != 0);

  // Create states
  pvPortReady = new mdtState(mdtPortManager::PortReady, parentState);
  pvPortReady->setText(tr("Port ready"));
  pvPortReady->setLedColorId(mdtLed::Green);
  pvPortReady->setLedOn(true);
  pvConnecting = new mdtState(mdtPortManager::Connecting, parentState);
  pvConnecting->setText(tr("Connecting ..."));
  pvConnecting->setLedColorId(mdtLed::Orange);
  pvConnecting->setLedOn(true);
  pvConnected = new mdtState(mdtPortManager::Connected, parentState);
  pvDisconnected = new mdtState(mdtPortManager::Disconnected, parentState);
  pvDisconnected->setText(tr("Disconnected"));
  pvDisconnected->setLedColorId(mdtLed::Green);
  pvDisconnected->setLedOn(false);
  // Add pvPortReady transitions ->
  pvPortReady->addTransition(portManager, SIGNAL(pmConnectingEvent()), pvConnecting);
  // Add pvConnecting transitions ->
  pvConnecting->addTransition(portManager, SIGNAL(pmDisconnectedEvent()), pvDisconnected);
  pvConnecting->addTransition(portManager, SIGNAL(pmConnectedEvent()), pvConnected);
  // Add pvDisconnected transitions ->
  pvDisconnected->addTransition(portManager, SIGNAL(pmConnectingEvent()), pvConnecting);
  // Add pvConnected transitions ->
  pvConnected->addTransition(portManager, SIGNAL(pmDisconnectedEvent()), pvDisconnected);
  // Add Global sate transitions and set initial state
  parentState->addTransition(portManager, SIGNAL(pmConnectingEvent()), pvConnecting);
  parentState->addTransition(portManager, SIGNAL(pmConnectedEvent()), pvConnected);
  parentState->setInitialState(pvPortReady);
  // Build connected sub machine
  buildConnectedSubMachine(portManager, pvConnected);
}

void mdtPortManagerStateMachine::buildConnectedSubMachine(mdtPortManager *portManager, mdtState *parentState)
{
  Q_ASSERT(pvStateMachine != 0);
  Q_ASSERT(portManager != 0);
  Q_ASSERT(parentState != 0);

  // Create states
  pvReady = new mdtState(mdtPortManager::Ready , parentState);
  pvReady->setText(tr("Ready"));
  pvReady->setLedColorId(mdtLed::Green);
  pvReady->setLedOn(true);
  pvBusy = new mdtState(mdtPortManager::Busy , parentState);
  pvBusy->setText(tr("Busy"));
  pvBusy->setLedColorId(mdtLed::Orange);
  pvBusy->setLedOn(true);
  // Add pvReady transitions ->
  pvReady->addTransition(portManager, SIGNAL(pmBusyEvent()), pvBusy);
  // Add pvBusy transitions ->
  pvBusy->addTransition(portManager, SIGNAL(pmReadyEvent()), pvReady);
  // Set initial state
  parentState->setInitialState(pvReady);
}
