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
#ifndef MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_STATE_CHAIN_H
#define MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_STATE_CHAIN_H

#include "ControllerState.h"
#include <QObject>

namespace Mdt{ namespace ItemEditor{

  /*! \brief Deduce next state regarding a event
   *
   * AbstractControllerStateChain is used by ControllerStateMachine
   *  to handle state change regarding current state and a event.
   *
   * Checking if a event can occure in current state
   *  is allready done by ControllerStateMachine.
   */
  class AbstractControllerStateChain : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit AbstractControllerStateChain(QObject *parent = nullptr);

    /*! \brief Destructor
     */
    virtual ~AbstractControllerStateChain() = default;

    // Copy disabled
    AbstractControllerStateChain(const AbstractControllerStateChain &) = delete;
    AbstractControllerStateChain & operator=(const AbstractControllerStateChain &) = delete;
    // Move disabled
    AbstractControllerStateChain(AbstractControllerStateChain &&) = delete;
    AbstractControllerStateChain & operator=(AbstractControllerStateChain &&) = delete;

    ControllerState currentState() const
    {
      return mCurrentState;
    }

    /*! \brief Get state after data edition started
     */
    virtual ControllerState dataEditionStartedState(ControllerState currentState) const;

   signals:

    /*! \brief Emitted whenever current state changed
     */
    void currentStateChanged();

   protected:

    /*! \internal Force current state
     *
     * This method is used for unit tests.
     */
    void forceCurrentState(ControllerState state);

   private:

    ControllerState mCurrentState;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_STATE_CHAIN_H
