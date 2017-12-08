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
#ifndef MDT_TEST_LIB_CREATE_FILE_H
#define MDT_TEST_LIB_CREATE_FILE_H

#include "MdtTestLib_CoreExport.h"
#include <QString>

namespace Mdt{ namespace TestLib{

  /*! \brief Create a file to the location \a filePath
   *
   * Will create missing directories if needed
   */
  bool MDT_TESTLIB_CORE_EXPORT createFile(const QString & filePath);

  /*! \brief Create a file named \a fileName in the location \a directoryPath
   *
   * Will create missing directories if needed
   */
  bool MDT_TESTLIB_CORE_EXPORT createFileInDirectory(const QString & directoryPath, const QString & fileName);

}} // namespace Mdt{ namespace TestLib{

#endif // #ifndef MDT_TEST_LIB_CREATE_FILE_H
