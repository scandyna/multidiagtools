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
#ifndef MDT_CL_CONNECTOR_H
#define MDT_CL_CONNECTOR_H

#include "mdtTtBase.h"
#include "mdtError.h"
#include "mdtClConnectorData.h"

/*! \brief Helper class for connector manipulations
 *
 * Acts mainly on Connector_tbl and ConnectorContact_tbl
 */
class mdtClConnector : public mdtTtBase
{
 public:

  /*! \brief Constructor
   */
  mdtClConnector(QObject *parent, QSqlDatabase db);

  /*! \brief Constructor
   */
  mdtClConnector(QSqlDatabase db);

  /*! \brief Add a connector to database
   *
   * \param data Connector data to store
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By callsing this function with a allready open transaction,
   *             set this argument false.
   * \return Id_PK of added connector, or a null key on error
   */
  mdtClConnectorKeyData addConnector(const mdtClConnectorData & data, bool handleTransaction);

  /*! \brief Get connector data
   *
   * \return data for given key.
   *       A null data is returned if given key does not exist, or a error occured.
   *       Use ok parameter to diffrenciate both cases.
   */
  mdtClConnectorData getConnectorData(const mdtClConnectorKeyData & key, bool & ok);

  /*! \brief Remove connector
   */
  bool removeConnector(const mdtClConnectorKeyData & key);

 private:

  Q_DISABLE_COPY(mdtClConnector);
};

#endif // #ifndef MDT_CL_CONNECTOR_H
