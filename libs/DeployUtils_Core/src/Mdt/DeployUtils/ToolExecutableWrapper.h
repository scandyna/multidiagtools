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
#ifndef MDT_DEPLOY_UTILS_TOOL_EXECUTABLE_WRAPPER_H
#define MDT_DEPLOY_UTILS_TOOL_EXECUTABLE_WRAPPER_H

#include "Mdt/Error.h"
#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Common base class for command line tools (ldd, objdump, ...)
   */
  class ToolExecutableWrapper : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit ToolExecutableWrapper(QObject* parent = nullptr);

    /*! \brief Returns all data available from standard output of the channel as string
     */
    QString readAllStandardOutputString();

    /*! \brief Returns all data available from standard error of the channel as string
     */
    QString readAllStandardErrorString();

   protected:

    /*! \brief Execute a command
     */
    bool exec(const QString & exeName, const QStringList & arguments);

    /*! \brief Set last error
     */
    void setLastError(const Mdt::Error & error);

   private:

    QProcess mProcess;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_TOOL_EXECUTABLE_WRAPPER_H
