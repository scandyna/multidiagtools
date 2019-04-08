/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#ifndef MDT_ASYNC_WAIT_DONE_PREDICATE_H
#define MDT_ASYNC_WAIT_DONE_PREDICATE_H

#include "MdtAsyncExport.h"
#include <QObject>
#include <chrono>

namespace Mdt{ namespace Async{

  /*! \brief Done predicate used by Mdt::Async::wait()
   */
  class MDT_ASYNC_EXPORT WaitDonePredicate : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    WaitDonePredicate(QObject *parent = nullptr)
     : QObject(parent)
    {
    }

    /*! \brief Reset done state and start the timeout timer
     */
    void reset(std::chrono::milliseconds timeout)
    {
    }

    void start(std::chrono::milliseconds timeout)
    {
    }

    /*! \brief Start the timeout timer
     *
     * \todo should be start(), or restart(),
     *  and should reset states ( reset() ? )
     */
    void startTimeoutTimer(std::chrono::milliseconds t)
    {
    }

    /*! \brief Check if this wait predicate is finished
     *
     * Returns true if either the processing is done
     *  ( setDone() was called ),
     *  or after timeout .
     */
    bool isFinished() const noexcept
    {
      return mDone;
    }

    bool hasTimedOut() const noexcept
    {
      return mTimedOut;
    }

   public slots:

    /*! \brief Set this wait predicate done
     *
     * When this implementation directly sets the done flag.
     *
     * For other use cases, for example having to wait for multiple objects,
     *  this method can be reimplemented:
     * \code
     * void MyWaitPredicate::setDone()
     * {
     *   if(allMyObjectDones()){
     *     AbstractWaitDonePredicate::setDone();
     *   }
     * }
     * \endcode
     */
    virtual void setDone()
    {
    }

   private:

    void setTimedOut();

    bool mDone = false;
    bool mTimedOut = false;
  };

}} // namespace Mdt{ namespace Async{

#endif // MDT_ASYNC_WAIT_DONE_PREDICATE_H
