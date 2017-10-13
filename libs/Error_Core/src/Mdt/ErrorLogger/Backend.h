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
#ifndef MDT_ERROR_LOGGER_BACKEND_H
#define MDT_ERROR_LOGGER_BACKEND_H

#include "MdtError_CoreExport.h"
#include <QObject>
#include <QString>
#include <memory>

namespace Mdt{

 class Error;

 namespace ErrorLogger {

  class FormatEngine;

  /*! \brief Error Logger backend
   *
   * This class is a interface to create a error logger backend
   *  that will be used by error Logger to output errors.
   *
   * Notice that Logger can be executed from a non main thread,
   *  also take care that some functions must at least be reentrant.
   */
  class MDT_ERROR_CORE_EXPORT Backend : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    Backend(QObject *parent = nullptr);

    /*! \brief Destructor
     */
    ~Backend();

    // Disable copy and move
    Backend(const Backend &) = delete;
    Backend(Backend &&) = delete;
    Backend & operator=(const Backend &) = delete;
    Backend & operator=(Backend &&) = delete;

    /*! \brief Set the format engine
     *
     * A format engine is a subclass of FormatEngine
     *  that outputs a Mdt::Error to its string representation.
     *
     * \note Concrete subclass of Backend should set its format engine
     *
     * \warning Setting the format engine is not thread safe.
     *          It should be done before any error is committed
     *          to a backend.
     */
    template<typename T>
    void setFormatEngine()
    {
      mFormatEngine = std::make_unique<T>();
    }

    /*! \brief Called during end cleanup
     *
     * This method is called from Logger::cleanup().
     *
     * Backend can implement this method if requierd.
     *
     * This default implementation does nothing.
     */
    virtual void cleanup()
    {
    }

   public slots:

    /*! \brief Log given error
     *
     * This method must be reentrant, because it can be called from Logger thread
     *  (which is not the main thread).
     */
    virtual void logError(const Error & error) = 0;

   protected:

    /*! \brief Format \a error to its string representation
     *
     * Will use the format engine set using setFormatEngine().
     *
     * \pre A format engine must have been set before calling this method.
     */
    QString formatError(const Mdt::Error & error) const;

   private:

    std::unique_ptr<FormatEngine> mFormatEngine;
  };

}}  // namespace Mdt{ namespace ErrorLogger {

#endif // #ifndef MDT_ERROR_LOGGER_BACKEND_H
