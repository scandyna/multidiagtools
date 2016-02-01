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
#ifndef MDT_SQL_FIELD_SELECTION_MODE_H
#define MDT_SQL_FIELD_SELECTION_MODE_H

namespace mdt{ namespace sql{

  /*! \brief Field selection mode
   */
  enum class FieldSelectionMode
  {
    MultiSelection,   /*!< Its possible to select more that 1 field */
    SingleSelection   /*!< Only 1 field can be selected */
  };

}} // namespace mdt{ namespace sql{

#endif // #ifndef MDT_SQL_FIELD_SELECTION_MODE_H
