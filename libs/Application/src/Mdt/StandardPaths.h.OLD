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
#ifndef MDT_STANDARD_PATHS_H
#define MDT_STANDARD_PATHS_H

#include <QStandardPaths>
#include <QString>

namespace Mdt{

  /*! \brief Provides methods for accessing standard paths
   *
   * This class uses QStandardPaths and adds some additionnal
   *  paths defined for the Mdt library.
   */
  class StandardPaths
  {
   public:

    /*! \brief Get path to data the directory of Mdt
     *
     * This returns a path to mdt directory
     *  located in QStandardPaths::GenericDataLocation
     *
     * This directory is used by Mdt libraries that
     *  needs access to some data.
     */
    static QString mdtDataDirectoryPath();
  };

} // namespace Mdt{

#endif // #ifndef MDT_STANDARD_PATHS_H
