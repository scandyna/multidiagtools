/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_CSV_RECORD_FORMATTER_H
#define MDT_CSV_RECORD_FORMATTER_H

#include "RecordFormat.h"
#include "mdtError.h"

namespace mdt{ namespace csv{

  /*! \brief Format record
   */
  class RecordFormatter
  {
   public:

    /*! \brief Set field type
     */
    

    /*! \brief Get last error
     */
    mdtError lastError() const
    {
      return pvLastError;
    }

   private:

    RecordFormat pvFormat;
    mdtError pvLastError;
  };

}} // namespace mdt{ namespace csv{

#endif // #ifndef MDT_CSV_RECORD_FORMATTER_H

