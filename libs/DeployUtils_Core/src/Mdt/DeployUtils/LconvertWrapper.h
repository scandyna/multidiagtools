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
#ifndef MDT_DEPLOY_UTILS_LCONVERT_WRAPPER_H
#define MDT_DEPLOY_UTILS_LCONVERT_WRAPPER_H

#include "QtToolExecutableWrapper.h"
#include "MdtDeployUtils_CoreExport.h"
#include <QString>
#include <QStringList>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Wraps a lconvert command line tool
   *
   * Example of usage:
   * \code
   * LconvertWrapper lconvert;
   *
   * // Maybe specify where expected Qt installation is located
   * PathList pathPrefixList{"/opt/Qt5/5.9.1/gcc_64"};
   * lconvert.findQtBinPath(pathPrefixList);
   * // Find and run lconvert
   * lconvert.execLconvert({"-h"});
   * \endcode
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT LconvertWrapper : public QtToolExecutableWrapper
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit LconvertWrapper(QObject* parent = nullptr);

    /*! \brief Execute lconvert
     */
    bool execLconvert(const QStringList & arguments);

    /*! \brief Execute joining QM files
     *
     * \pre \a inFilePathList must not be empty
     * \pre No file path in \a inFilePathList must be empty
     * \pre \a outFilePath must not be empty
     */
    bool executeJoinQmFiles(const QStringList & inFilePathList, const QString & outFilePath);

   private:

    bool checkStdError();
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_LCONVERT_WRAPPER_H
