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
#ifndef MDT_USBTMC_TRANSFER_HANDLER_STATE_MACHINE_H
#define MDT_USBTMC_TRANSFER_HANDLER_STATE_MACHINE_H

#include "mdtUsbtmcTransferHandler.h"
#include "mdtError.h"
#include <vector>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/euml/common.hpp>
#include <boost/msm/front/euml/operator.hpp>

#include <memory>

#include <mutex>

#include <QtGlobal>

#include <QDebug>

/*! \brief USBTMC transfer handler state machine namespace
 *
 * State machine specific stuff is defined in this namespace,
 *  such as events, states and state machine itself.
 */
namespace mdtUsbtmcTransferHandlerStateMachine
{
  using namespace boost::msm::front;

  /*! \brief Error occured event
   */
  struct ErrorOccuredEvent
  {
    ErrorOccuredEvent(const mdtError & e) : error(e){}
    mdtError error;
  };

  /*! \brief Device disconnected event
   */
  struct DeviceDisconnectedEvent {};

  /*! \brief Stop requested event
   */
  struct StopRequestedEvent {};

  /*! \brief Fake event
   */
  struct fakeEvent {};

  
  struct fakeRequest {};

  /*! \brief Running submachine
   */
  struct Running_ : public state_machine_def<Running_>
  {
    /*! \brief Running submachine entry/
     */
    template<typename Event, typename FSM>
    void on_entry(Event const &, FSM &)
    {
      qDebug() << "Entering Running ...";
    }

    /*! \brief Running submachine exit/
     */
    template<typename Event, typename FSM>
    void on_exit(Event const &, FSM &)
    {
      qDebug() << "Leaving Running ...";
    }

    /*! \brief Idle state
     */
    struct Idle : public boost::msm::front::state<>
    {
      /*! \brief Idle entry/
       */
      template<typename Event, typename FSM>
      void on_entry(Event const &, FSM & fsm)
      {
        qDebug() << "Entering Idle ...";
        ///fsm.process_event(fakeRequest());
        ///fsm.sendEvent();
      }
      /*! \brief Idle exit/
       */
      template<typename Event, typename FSM>
      void on_exit(Event const &, FSM &)
      {
        qDebug() << "Leaving Idle ...";
      }
    };

    /*! \brief Processing state
     */
    struct Processing : public boost::msm::front::state<>
    {
      /*! \brief Processing entry/
       */
      template<typename Event, typename FSM>
      void on_entry(Event const &, FSM &)
      {
        qDebug() << "Entering Processing ...";
      }
      /*! \brief Processing exit/
       */
      template<typename Event, typename FSM>
      void on_exit(Event const &, FSM &)
      {
        qDebug() << "Leaving Processing ...";
      }
    };

    /*! \brief Initial state of Running submachine
     */
    typedef Idle initial_state;

    /*! \brief Transition table
     */
    struct transition_table : boost::mpl::vector<
      //     Start state       Event            Next state        Action           Guard
      // +--------------------+----------------+----------------+----------------+---------------------+
      Row < Idle             , fakeRequest   , Processing          , none           , none               >
      // +--------------------+----------------+----------------+----------------+---------------------+
    >{};
  };

  /*! \brief Running submachine
    */
  typedef boost::msm::back::state_machine<Running_> Running;

  /*! \brief Main state machine definition class
   */
  struct StateMachine_ : public boost::msm::front::state_machine_def<StateMachine_>
  {
    StateMachine_()
     : transferHandler(0)
    {
    }

    /*! \brief Set transfer handler
     *
     * Must be called before starting the state machine
     */
    void setTh(mdtUsbtmcTransferHandler *th)
    {
      transferHandler = th;
    }

    void fakeAction(const fakeEvent &)
    {
      Q_ASSERT(transferHandler != 0);
      qDebug() << "** Fake action called !";
      transferHandler->someThFakeAction();
    }

    /*! \brief
     */
    template<typename Event, typename FSM>
    void on_entry(Event const &, FSM &)
    {
      qDebug() << "Entering StateMachine_ ...";
      ///pvTransferHandler.fakeFunction();
      ///transferHandler->someThFakeAction();
    }

    /*! \brief
     */
    template<typename Event, typename FSM>
    void on_exit(Event const &, FSM &)
    {
      qDebug() << "Leaving StateMachine_ ...";
    }

    /*! \brief AllOK state
     */
    struct AllOk : public boost::msm::front::state<>
    {
      /*! \brief AllOk entry/
       */
      template<typename Event, typename FSM>
      void on_entry(Event const &, FSM & fsm)
      {
        qDebug() << "Entering AllOk ...";
        ///fsm.transferHandler->someThFakeAction();
      }
      /*! \brief AllOk exit/
       */
      template<typename Event, typename FSM>
      void on_exit(Event const &, FSM &)
      {
        qDebug() << "Leaving AllOk ...";
      }
    };

    /*! \brief Error state
     */
    struct Error : public boost::msm::front::state<>
    {
      /*! \brief Error entry/
       */
      template<typename Event, typename FSM>
      void on_entry(Event const &, FSM & fsm)
      {
        Q_ASSERT(fsm.transferHandler != 0);
        qDebug() << "Entering Error ...";
      }
      /*! \brief Error exit/
       */
      template<typename Event, typename FSM>
      void on_exit(Event const &, FSM &)
      {
        qDebug() << "Leaving Error ...";
      }
    };

    /*! \brief Stopping state
     */
    struct Stopping : public boost::msm::front::state<>
    {
      /*! \brief Stopping entry/
       */
      template<typename Event, typename FSM>
      void on_entry(Event const &, FSM & fsm)
      {
        qDebug() << "Entering Stopping ...";
        ///fsm.process_event(ErrorOccuredEvent());
        ///fsm.transferHandler->someThFakeAction();
      }
      /*! \brief Stopping exit/
       */
      template<typename Event, typename FSM>
      void on_exit(Event const &, FSM &)
      {
        qDebug() << "Leaving Stopping ...";
      }
    };

    /*! \brief Stopped state
     */
    struct Stopped : public boost::msm::front::state<>
    {
      /*! \brief Stopped entry/
       */
      template<typename Event, typename FSM>
      void on_entry(Event const &, FSM &)
      {
        qDebug() << "Entering Stopped ...";
      }
      /*! \brief Stopped exit/
       */
      template<typename Event, typename FSM>
      void on_exit(Event const &, FSM &)
      {
        qDebug() << "Leaving Stopped ...";
      }
    };

    /*! \brief Initial state of main state machine
     */
    ///typedef boost::mpl::vector<Stopped, AllOk> initial_state;
    typedef Running initial_state;

    /*! \brief Main state machine transition table
     */
    struct transition_table : boost::mpl::vector<
      //     Start state       Event            Next state        Action           Guard
      // +--------------------+----------------+----------------+----------------+---------------------+
      Row < Stopping             , ErrorOccuredEvent   , Error          , none           , none               >,
      // +--------------------+----------------+----------------+----------------+---------------------+
      _row < Running         , StopRequestedEvent   , Stopping        > ,
      // +--------------------+----------------+----------------+----------------+---------------------+
      _row < Running         , ErrorOccuredEvent   , Error        >
      // +--------------------+----------------+----------------+----------------+---------------------+
    >{};

    /*! \brief Called when no transition exists
     */
    template <class FSM,class Event>
    void no_transition(Event const & e, FSM &, int state)
    {
        qDebug() << "No transition from state " << state << " on event " << typeid(e).name();
    }

    // StateMachine members
    mdtUsbtmcTransferHandler *transferHandler;
  };

  /*! \brief State machine class
   */
  struct StateMachine : public boost::msm::back::state_machine<StateMachine_>
  {
    template<class Event>
    void process_event(Event const& evt)
    {
      qDebug() << "* Processing event ...";
      std::lock_guard<std::recursive_mutex> lg(pvMutex);
      boost::msm::back::state_machine<StateMachine_>::process_event(evt);
      qDebug() << "* Processing event DONE";
    }

    std::recursive_mutex pvMutex;
  };

};


#endif
