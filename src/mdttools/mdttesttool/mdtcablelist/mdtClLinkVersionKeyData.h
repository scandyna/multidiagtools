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
#ifndef MDT_CL_LINK_VERSION_KEY_DATA_H
#define MDT_CL_LINK_VERSION_KEY_DATA_H

#include "mdtValue.h"

/*! \brief Link version primary key data
 *
 * Refers to LinkVersion_tbl
 */
struct mdtClLinkVersionPkData
{
  /*! \brief Version_PK
   */
  mdtValueInt versionPk;

  /*! \brief Check if PK is null
   */
  bool isNull() const
  {
    return versionPk.isNull();
  }

  /*! \brief Clear PK
   */
  void clear()
  {
    versionPk.clear();
  }
};

#endif // #ifndef MDT_CL_LINK_VERSION_KEY_DATA_H
