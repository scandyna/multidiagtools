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
#ifndef MDT_LIBRARY_INFO_IMPL_WINDOWS_H
#define MDT_LIBRARY_INFO_IMPL_WINDOWS_H

#include "Mdt/Expected.h"
#include "MdtLibraryInfo_CoreExport.h"
#include <QObject>
#include <QString>

namespace Mdt{

  /*! \internal
   */
  class MDT_LIBRARYINFO_CORE_EXPORT LibraryInfoImplWindows : public QObject
  {
   Q_OBJECT

   public:

    static Mdt::Expected<QString> getLibraryFilePath(const void * const address);
  };

} // namespace Mdt{

#endif // #ifndef MDT_LIBRARY_INFO_IMPL_WINDOWS_H
