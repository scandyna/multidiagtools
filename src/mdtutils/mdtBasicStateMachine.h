/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#ifndef MDT_BASIC_STATE_MACHINE_H
#define MDT_BASIC_STATE_MACHINE_H

#include <atomic>
#include <type_traits>

/*! \brief Basic state machine
 *
 * This is a basic state holder,
 *  that only supports enum types.
 *
 * For complex state machines, see mdtStateMachine,
 *  wich is based on QStateMachine.
 */
template<typename S>
class mdtBasicStateMachine
{
 public:

  /*! \brief Constructor
   */
  mdtBasicStateMachine(S initialState)
   : pvCurrentState(initialState)
  {
    static_assert(std::is_enum<S>::value, "mdtBasicStateMachine::mdtBasicStateMachine(): S must be a enum type");
  }

  /*! \brief Set current state
   */
  inline void setCurrentState(S state)
  {
    static_assert(std::is_enum<S>::value, "mdtBasicStateMachine::mdtBasicStateMachine(): S must be a enum type");
    pvCurrentState.store(state);
  }

  /*! \brief Get current state
   */
  inline S currentState() const
  {
    static_assert(std::is_enum<S>::value, "mdtBasicStateMachine::mdtBasicStateMachine(): S must be a enum type");
    return pvCurrentState.load();
  }

 private:

  std::atomic<S> pvCurrentState;
};

#endif // #ifndef MDT_BASIC_STATE_MACHINE_H
