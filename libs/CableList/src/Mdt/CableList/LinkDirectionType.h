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
#ifndef MDT_CABLE_LIST_LINK_DIRECTION_TYPE_H
#define MDT_CABLE_LIST_LINK_DIRECTION_TYPE_H

namespace Mdt{ namespace CableList{

  /*! \brief Link direction type
   */
  enum class LinkDirectionType
  {
    Undefined,      /*!< Undefined direction (is the case for null data) */
    Bidirectional,  /*!< Bidirectional direction (is the case for null data) */
    StartToEnd,     /*!< Start -> end direction */
    EndToStart      /*!< End -> start direction */
  };


}} // namespace Mdt{ namespace CableList{

#endif // #ifndef MDT_CABLE_LIST_LINK_DIRECTION_TYPE_H
