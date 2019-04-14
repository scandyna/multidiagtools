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
#ifndef MDT_ASYNC_WAIT_DONE_PREDICATE_WITH_ERROR_H
#define MDT_ASYNC_WAIT_DONE_PREDICATE_WITH_ERROR_H

#include "WaitDonePredicate.h"
#include "MdtAsyncExport.h"

namespace Mdt{ namespace Async{

  /*! \brief Done predicate used by Mdt::Async::wait()
   */
  class MDT_ASYNC_EXPORT WaitDonePredicateWithError : public WaitDonePredicate
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    WaitDonePredicateWithError(QObject *parent = nullptr)
     : WaitDonePredicate(parent)
    {
    }

    /*! \brief Check if a error was set
     */
    bool hasError() const noexcept
    {
      return mHasError;
    }

   public Q_SLOTS:

    /*! \brief Set a error
     *
     * This will also set this predicate finished
     */
    void setErrorOccured()
    {
      mHasError = true;
      setDone();
    }

   private:

    void resetPredicateStates() override
    {
      mHasError = false;
    }

    bool mHasError = false;
  };

}} // namespace Mdt{ namespace Async{

#endif // #ifndef MDT_ASYNC_WAIT_DONE_PREDICATE_WITH_ERROR_H
