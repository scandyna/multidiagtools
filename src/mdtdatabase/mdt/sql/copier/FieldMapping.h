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
#ifndef MDT_SQL_COPIER_FIELD_MAPPING_H
#define MDT_SQL_COPIER_FIELD_MAPPING_H

#include "AbstractTableMappingItem.h"

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Table mapping item implementation for field mapping
   */
  class FieldMapping : public AbstractTableMappingItem
  {
   public:

    /*! \brief Create a invalid field mapping
     */
    FieldMapping()
     : sourceFieldIndex(-1),
       destinationFieldIndex(-1)
    {
    }

    /*! \brief Destructor
     */
    ~FieldMapping()
    {
    }

    /*! \brief Check if field mapping is null
     *
     * Fiel mapping is null if at least source or destination field index is < 0
     */
    bool isNull() const override
    {
      return ( (sourceFieldIndex < 0) || (destinationFieldIndex < 0) );
    }

   private:

    int sourceFieldIndex;
    int destinationFieldIndex;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef
