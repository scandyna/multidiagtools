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
#ifndef MDT_CL_CONNECTOR_KEY_DATA_H
#define MDT_CL_CONNECTOR_KEY_DATA_H

#include <QVariant>

/*! \brief Data container for connector key
 *
 * Refers to Connector_tbl
 */
struct mdtClConnectorPkData
{
  /*! \brief Connector ID (Id_PK)
   */
  QVariant id;

  /*! \brief Check if key data is null
   */
  bool isNull() const
  {
    return id.isNull();
  }

  /*! \brief Clear
   */
  void clear()
  {
    id.clear();
  }
};

#endif // MDT_CL_CONNECTOR_KEY_DATA_H

