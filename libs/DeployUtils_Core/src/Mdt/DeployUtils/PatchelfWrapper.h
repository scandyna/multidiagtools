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
#ifndef MDT_DEPLOY_UTILS_PATCHELF_WRAPPER_H
#define MDT_DEPLOY_UTILS_PATCHELF_WRAPPER_H

#include "ToolExecutableWrapper.h"
#include "MdtDeployUtils_CoreExport.h"
#include <QString>
#include <QStringList>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Wraps a patchelf executable
   *
   * This is a low level wrapper around patchelf tool.
   *  Applications should use RPath .
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT PatchelfWrapper : public ToolExecutableWrapper
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit PatchelfWrapper(QObject* parent = nullptr);

    /*! \brief Execute the command to read RPATH
     *
     * \param binaryFilePath Path to a executable or a library
     */
    bool execReadRPath(const QString & binaryFilePath);

    /*! \brief Execute the command to write RPATH
     *
     * \param rpath RPATH string to pass to patchelf
     * \param binaryFilePath Path to a executable or a library
     */
    bool execWriteRPath(const QString & rpath, const QString & binaryFilePath);

   private:

    bool execPatchelf(const QStringList & arguments);
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_PATCHELF_WRAPPER_H
