/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_ERROR_QPROCESS_H
#define MDT_ERROR_QPROCESS_H

#include "MdtError_CoreExport.h"
#include <QString>
#include <QCoreApplication>

/*! \brief Get a Mdt::Error from last error in \a process
 *
 * \sa mdtErrorFromQProcessQ()
 */
#define mdtErrorFromQProcess(process, className) Mdt::ErrorQProcess::fromQProcess(process, __FILE__, __LINE__, className, __FUNCTION__)

/*! \brief Get a Mdt::Error from last error in \a process
 *
 * This version avoid giving class name,
 *  but only works for QObject subclass.
 *
 * \sa mdtErrorFromQProcess()
 */
#define mdtErrorFromQProcessQ(process, obj) Mdt::ErrorQProcess::fromQProcess(process, __FILE__, __LINE__, obj, __FUNCTION__)

class QProcess;
class QObject;

namespace Mdt{

  class Error;

  /*! \brief Translate QProcess errors to Mdt::Error
   *
   * \sa mdtErrorFromQProcess()
   * \sa mdtErrorFromQProcessQ()
   */
  class MDT_ERROR_CORE_EXPORT ErrorQProcess
  {
   Q_DECLARE_TR_FUNCTIONS(ErrorQProcess)

   public:

    /*! \brief Get a Mdt::Error from last error in \a process
     *
     * \sa mdtErrorFromQProcess()
     */
    static Mdt::Error fromQProcess(const QProcess & process, const QString & file, int line, const QString & className, const QString & functionName);

    /*! \brief Get a Mdt::Error from last error in \a process
     *
     * \sa mdtErrorFromQProcessQ()
     */
    static Mdt::Error fromQProcess(const QProcess & process, const QString & file, int line, const QObject * const obj, const QString & functionName);
  };

} // namespace Mdt{

#endif // #ifndef MDT_ERROR_QPROCESS_H
