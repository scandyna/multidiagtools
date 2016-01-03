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
#ifndef MDT_SQL_COPIER_TABLE_MAPPING_ITEM_STATE_H
#define MDT_SQL_COPIER_TABLE_MAPPING_ITEM_STATE_H

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief State of a TableMappingItem
   */
  enum class TableMappingItemState
  {
    MappingNotSet,      /*!< Source or destination was not set.
                              This is the default. */
    MappingComplete,    /*!< Field mapping was done automatically without any mismatch,
                              or some mapping was set and confirmed by the user as complete. */
    MappingPartial,     /*!< Field mapping was done automatically, but some mismatch
                              was detected, and a action is required from the user to fix it */
    MappingError        /*!< A error was detected table item mapping. */
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_TABLE_MAPPING_ITEM_STATE_H

