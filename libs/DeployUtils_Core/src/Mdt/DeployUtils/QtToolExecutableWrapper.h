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
#ifndef MDT_DEPLOY_UTILS_QT_TOOL_EXECUTABLE_WRAPPER_H
#define MDT_DEPLOY_UTILS_QT_TOOL_EXECUTABLE_WRAPPER_H

#include "ToolExecutableWrapper.h"
#include "Mdt/FileSystem/PathList.h"
#include "MdtDeployUtils_CoreExport.h"
#include <QString>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Common base class for command line tools provided by Qt (lconvert, ..)
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT QtToolExecutableWrapper : public ToolExecutableWrapper
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit QtToolExecutableWrapper(QObject* parent = nullptr);

    /*! \brief Find the root directory of Qt bin
     *
     * For a given path prefix, a subdirectory, named bin,
     *  will be located in known subdirectories,
     *  like bin or qt5/bin .
     *
     * Searching will be done in prefixes defined in \a pathPrefixList .
     *  If Qt bin could not be found in \a pathPrefixList ,
     *  or \a pathPrefixList is empty, false returned,
     *  and qtBinPath() returns a empty string.
     *
     * \sa qtBinPath()
     */
    bool findQtBinPath(const Mdt::FileSystem::PathList & pathPrefixList);

    /*! \brief Get Qt bin path
     *
     * Returns the path found by findQtBinPath() .
     *  Can also return a empty string, which means:
     *  - Maybe Qt bin existing in system PATH could be used
     *  - Qt bin does not exist
     *
     * \sa findQtBinPath()
     */
    QString qtBinPath() const
    {
      return mQtBinPath;
    }

   private:

    QString mQtBinPath;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_QT_TOOL_EXECUTABLE_WRAPPER_H
