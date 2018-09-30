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
#ifndef MDT_LIBRARY_INFO_H
#define MDT_LIBRARY_INFO_H

#include "Mdt/Expected.h"
#include "MdtLibraryInfo_CoreExport.h"
#include <QString>

namespace Mdt{

  /*! \brief Provides informations about the Mdt library
   */
  class MDT_LIBRARYINFO_CORE_EXPORT LibraryInfo
  {
   public:

    /*! \brief Get path to the root of the Mdt libraries
     *
     * This method can handle those cases:
     *  - While building the Mdt libraries, it will return the same path as ${CMAKE_BINARY_DIR}
     *    (the case of putting the outputs to the same directory is also handled).
     *  - Developping a application that uses a installed distribution of Mdt libraries
     *  - A installed application, as long as it respect the common model
     *
     * What is called the common above, is a structure that is commonly seen in portable applications:
     * \code
     * AppRoot
     *  |
     *  --bin/
     *  --lib/
     *  --plugins/
     * \endcode
     * 
     */
    static Mdt::Expected<QString> getPrefixPath();

    /*! \brief Get path to the plugins directory
     */
    static Mdt::Expected<QString> getPluginsPath();

    /*! \brief Get path to the installed Mdt libraries
     *
     * \note While building the Mdt library, this path has no sense
     *   because all libraries are dispatched in the build tree (except for Windows build)
     */
    static Mdt::Expected<QString> getInstalledLibrariesPath();
  };

} // namespace Mdt{

#endif // #ifndef MDT_LIBRARY_INFO_H
