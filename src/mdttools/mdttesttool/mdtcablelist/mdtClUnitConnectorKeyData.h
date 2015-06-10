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
#ifndef MDT_CL_UNIT_CONNECTOR_KEY_DATA_H
#define MDT_CL_UNIT_CONNECTOR_KEY_DATA_H

#include "mdtClConnectorKeyData.h"
#include "mdtClArticleConnectorKeyData.h"
#include <QVariant>

/*! \brief Data container for unit connector key data
 *
 * Refers to UnitConnector_tbl
 */
struct mdtClUnitConnectorKeyData
{
  /*! \brief Unit connector ID (Id_PK)
   */
  QVariant id;

  /*! \brief Unit ID (Unit_Id_FK)
   */
  QVariant unitId;

  /*! \brief Connector FK (Connector_Id_FK)
   */
  mdtClConnectorKeyData connectorFk;

  /*! \brief Article connector FK (ArticleConnector_Id_FK)
   */
  mdtClArticleConnectorKeyData articleConnectorFk;

  /*! \brief Check if key data is null
   *
   * Key data is null if id or unitId is null
   */
  bool isNull() const
  {
    return (id.isNull() || unitId.isNull());
  }

  /*! \brief Clear key data
   */
  void clear()
  {
    id.clear();
    unitId.clear();
    connectorFk.clear();
    articleConnectorFk.clear();
  }

  /*! \brief Check if unit connector is based on a connector (from Connector_tbl)
   */
  bool isBasedOnConnector() const
  {
    return (!connectorFk.isNull());
  }

  /*! \brief Check if unit connector is based on a article connector
   */
  bool isBasedOnArticleConnector() const
  {
    return (!articleConnectorFk.isNull());
  }
};

#endif // #ifndef MDT_CL_UNIT_CONNECTOR_KEY_DATA_H
