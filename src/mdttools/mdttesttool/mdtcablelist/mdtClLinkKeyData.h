/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_CL_LINK_KEY_DATA_H
#define MDT_CL_LINK_KEY_DATA_H

#include <QVariant>

/*! \brief Link primary key data
 *
 * Refers to Link_tbl
 */
struct mdtClLinkPkData
{
  /*! \brief Start connection ID (UnitConnectionStart_Id_FK)
   */
  QVariant connectionStartId;

  /*! \brief End connection ID (UnitConnectionEnd_Id_FK)
   */
  QVariant connectionEndId;

  /*! \brief Check if primary key is null
   *
   * Primary key is null if one of the connection ID is null
   */
  bool isNull() const
  {
    return (connectionStartId.isNull() || connectionEndId.isNull());
  }

  /*! \brief Clear primary key
   */
  void clear()
  {
    connectionStartId.clear();
    connectionEndId.clear();
  }
};

struct mdtClLinkKeyData
{
};

#endif // #ifndef MDT_CL_LINK_KEY_DATA_H
