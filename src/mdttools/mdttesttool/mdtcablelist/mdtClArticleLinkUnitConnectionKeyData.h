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
#ifndef MDT_CL_ARTICLE_LINK_UNIT_CONNECTION_KEY_DATA_H
#define MDT_CL_ARTICLE_LINK_UNIT_CONNECTION_KEY_DATA_H

#include "mdtClArticleLinkKeyData.h"
#include "mdtClUnitConnectionKeyData.h"

/*! \brief Container for article link and unit connection PKs
 *
 * The primary use of this data container is for link creation
 *  based on a article link.
 *
 * \sa mdtClUnitConnection
 */
struct mdtClArticleLinkUnitConnectionKeyData
{
  /*! \brief Article link PK
   */
  mdtClArticleLinkPkData articleLinkPk;

  /*! \brief Unit connection start PK
   */
  mdtClUnitConnectionPkData unitConnectionStartPk;

  /*! \brief Unit connection end PK
   */
  mdtClUnitConnectionPkData unitConnectionEndPk;

  /*! \brief Check if PK is null
   *
   * PK is null if one of the memebre is null
   */
  bool isNull() const
  {
    return (articleLinkPk.isNull() || unitConnectionStartPk.isNull() || unitConnectionEndPk.isNull());
  }

  /*! \brief Clear
   */
  void clear()
  {
    articleLinkPk.clear();
    unitConnectionStartPk.clear();
    unitConnectionEndPk.clear();
  }
};

#endif // #ifndef MDT_CL_ARTICLE_LINK_UNIT_CONNECTION_KEY_DATA_H
