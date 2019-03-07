/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

#include "Mdt/Error.h"
#include <QObject>

void responseHandlerDebugError(const Mdt::Error & error);

/*! \brief Class template to handle a response in use case tests
 */
template<typename Request, typename Response>
class ResponseHandler : public QObject
{
 public:

  explicit ResponseHandler(QObject *parent = nullptr)
   : QObject(parent)
  {
  }

  void setResponse(const Response & response)
  {
    mResponse = response;
  }

  const Response & response() const
  {
    return mResponse;
  }

  void setError(const Request & request, const Mdt::Error & error)
  {
    Q_UNUSED(request);

    mError = error;
    responseHandlerDebugError(mError);
  }

  Mdt::Error error() const
  {
    return mError;
  }

  /*! \brief Will do signal/slot connection between \a useCase and this response handler
   *
   * To be able to use this method, the use case must have a signal that tells success:
   * \code
   * void succeeded(const Response & response);
   * \endcode
   *
   * and also a failed signal:
   * \code
   * void failed(const Request & request, const Mdt::Error & error) const;
   * \endcode
   */
  template<typename UseCase>
  void setUseCase(const UseCase & useCase)
  {
    connect(&useCase, &UseCase::succeeded, this, &ResponseHandler::setResponse);
    connect(&useCase, &UseCase::failed, this, &ResponseHandler::setError);
  }

 private:

  Response mResponse;
  Mdt::Error mError;
};

#endif // #ifndef RESPONSE_HANDLER_H
