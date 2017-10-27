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
#ifndef MDT_DEPLOY_UTILS_BINARY_FORMAT_H
#define MDT_DEPLOY_UTILS_BINARY_FORMAT_H

#include "OperatingSystem.h"
#include "Processor.h"
#include "MdtDeployUtils_CoreExport.h"
#include "Mdt/Error.h"
#include <QObject>
#include <QString>

class QFileInfo;

namespace Mdt{ namespace DeployUtils{

  /*! \brief Read the format of a executable or a library
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT BinaryFormat : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit BinaryFormat(QObject* parent = nullptr);

    /*! \brief Read the format of a executable or a library
     */
    bool readFormat(const QString & binaryFilePath);

    /*! \brief Get operating system
     *
     * Return only a valid value after readFormat() succeded
     */
    OperatingSystem operatindSystem() const
    {
      return mOperatingSystem;
    }

    /*! \brief Get processor
     *
     * Return only a valid value after readFormat() succeded
     */
    Processor processor() const
    {
      return mProcessor;
    }

    /*! \brief Check if a file is a executable
     */
    static bool isFileAnExecutableByExtension(const QFileInfo & fileInfo);

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

   private:

    static bool compareExtension(const QString & extention, const char * const match);
    void setLastError(const Mdt::Error & error);

    OperatingSystem mOperatingSystem;
    Processor mProcessor;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_BINARY_FORMAT_H
