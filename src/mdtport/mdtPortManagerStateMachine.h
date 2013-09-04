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
#ifndef MDT_PORT_MANAGER_STATE_MACHINE_H
#define MDT_PORT_MANAGER_STATE_MACHINE_H


#include "mdtStateMachine.h"
#include <QObject>

class mdtState;
class mdtPortManager;

/*! \brief State machine for port manager
 *
 * A port manager can receive events form different objects,
 *  like threads and main application.
 *
 * This class helps port manager to keep a coherent state
 *  and react only on allowed events regarding current state.
 */
class mdtPortManagerStateMachine : public mdtStateMachine
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   *
   * Will build the state machine by calling buildMainStateMachine()
   */
  mdtPortManagerStateMachine(QObject *parent = 0);

  /*! \brief Destructor
   */
  ~mdtPortManagerStateMachine();

  /*! \brief Build main state machine
   *
   * \image html mdtPortManagerStateMachineMain.png
   * \pre portManager must be a valid pointer
   */
  void buildMainStateMachine(mdtPortManager *portManager);

 protected:

  /*! \brief Build the Running sub machine
   *
   * \image html mdtPortManagerStateMachineRunning.png
   * \pre portManager must be a valid pointer
   * \pre parentState must be a valid pointer
   */
  void buildRunningSubMachine(mdtPortManager *portManager, mdtState *parentState);

  /*! \brief Build the Connected sub machine
   *
   * \image html mdtPortManagerStateMachineConnected.png
   * \pre portManager must be a valid pointer
   * \pre parentState must be a valid pointer
   */
  void buildConnectedSubMachine(mdtPortManager *portManager, mdtState *parentState);

 private:

  Q_DISABLE_COPY(mdtPortManagerStateMachine);

  mdtStateMachine *pvStateMachine;
  // Main states
  mdtState *pvPortClosed;
  mdtState *pvStarting;
  mdtState *pvRunning;
  mdtState *pvStopping;
  mdtState *pvStopped;
  mdtState *pvPortError;
  // Running sub states
  mdtState *pvPortReady;
  mdtState *pvConnecting;
  mdtState *pvConnected;
  mdtState *pvDisconnected;
  // Connected sub states
  mdtState *pvReady;
  mdtState *pvBusy;
};
#endif  // #ifndef MDT_PORT_MANAGER_STATE_MACHINE_H
