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
#ifndef MDT_ITEM_MODEL_STRING_NUMERIC_MODE_H
#define MDT_ITEM_MODEL_STRING_NUMERIC_MODE_H

namespace Mdt{ namespace ItemModel{

  /*! \brief Numeric mode for strings
   */
  enum class StringNumericMode
  {
    Alphabetical, /*!< In sort context, series of strings like 1,2,11,12 will be sorted 1,11,12,2 */
    Natural       /*!< In sort context, series of strings like 1,2,11,12 will be sorted 1,2,11,12*/
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_STRING_NUMERIC_MODE_H
