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
#ifndef MDT_CL_UNIT_CONNECTION_KEY_DATA_H
#define MDT_CL_UNIT_CONNECTION_KEY_DATA_H

#include "mdtClUnitConnectorKeyData.h"
#include "mdtClArticleConnectionKeyData.h"
#include "mdtClConnectionTypeData.h"
#include <QVariant>

/*! \brief Data container for unit connection key data
 *
 * Refers to UnitConnection_tbl
 */
struct mdtClUnitConnectionKeyData
{
  /*! \brief Unit connection ID (Id_PK)
   */
  QVariant id;

  /*! \brief Unit ID (Unit_Id_FK)
   */
  QVariant unitId;

  /*! \brief Unit connector ID (UnitConnector_Id_FK)
   */
  mdtClUnitConnectorKeyData unitConnectorFk;

  /*! \brief Article connection FK (ArticleConnection_Id_FK)
   */
  mdtClArticleConnectionKeyData articleConnectionFk;

  /*! \brief Connection type
   */
  mdtClConnectionTypeKeyData connectionTypeFk;

  /*! \brief Check if key data is null
   *
   * Key data is null if id, unitId or connectionTypeFk is null
   */
  bool isNull() const
  {
    return (id.isNull() || unitId.isNull() || connectionTypeFk.isNull());
  }

  /*! \brief Clear
   */
  void clear()
  {
    id.clear();
    unitId.clear();
    unitConnectorFk.clear();
    articleConnectionFk.clear();
    connectionTypeFk.clear();
  }

  /*! \brief Check if unit connection is part of a unit connector
   */
  bool isPartOfUnitConnector() const
  {
    return (!unitConnectorFk.isNull());
  }

  /*! \brief Check if unit connection is based on a article connection
   */
  bool isBasedOnArticleConnection() const
  {
    return (!articleConnectionFk.isNull());
  }
};

#endif // #ifndef MDT_CL_UNIT_CONNECTION_KEY_DATA_H
