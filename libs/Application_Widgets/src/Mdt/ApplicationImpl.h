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
#ifndef MDT_APPLICATION_IMPL_H
#define MDT_APPLICATION_IMPL_H

#include "Mdt/CoreApplicationImpl.h"

namespace Mdt{

  /*! \brief Implementation for Application and related classes
   */
  class ApplicationImpl : public CoreApplicationImpl
  {
   Q_DECLARE_TR_FUNCTIONS(CoreApplicationImpl)

   public:

    /*! \brief Constructor
     */
    ApplicationImpl() = default;

    /*! \brief Destructor
     */
    ~ApplicationImpl() = default;

    // Copy disabled
    ApplicationImpl(const ApplicationImpl &) = delete;
    ApplicationImpl & operator=(const ApplicationImpl &) = delete;
    // Move disabled
    ApplicationImpl(ApplicationImpl &&) = delete;
    ApplicationImpl & operator=(ApplicationImpl &&) = delete;

    /*! \brief Get environment entries
     */
    QStringList environmentEntries();
  };

} // namespace Mdt{

#endif // #ifndef MDT_APPLICATION_IMPL_H
