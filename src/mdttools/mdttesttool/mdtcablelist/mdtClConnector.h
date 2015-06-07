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
#include "mdtClConnectorContactData.h"
#include "mdtClConnectionTypeData.h"
#include <QList>

/*! \brief Helper class for connector manipulations
 *
 * Acts mainly on Connector_tbl and ConnectorContact_tbl
 *
 * Acting with ConnectionType_tbl is also provided.
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

  /*! \brief Get connection type data for given code
   *
   * \return Data for given code.
   *       A null data is returned if given code does not exist, or a error occured.
   *       Use ok parameter to diffrenciate both cases.
   */
  mdtClConnectionTypeData getConnectionTypeData(const QString & code, bool & ok);

  /*! \brief Add a connector contact to database
   *
   * \param data Contact data
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   * \return Id_PK of added contact, or a null key on error
   */
  mdtClConnectorContactKeyData addContact(const mdtClConnectorContactData & data, bool handleTransaction);

  /*! \brief Get connector contact data from database
   *
   * \param key Data that contains contact ID (Id_PK).
   *     Note: only id is used, other key memebers are ignored.
   * \return data for given key.
   *       A null data is returned if given key does not exist, or a error occured.
   *       Use ok parameter to diffrenciate both cases.
   */
  mdtClConnectorContactData getContactData(const mdtClConnectorContactKeyData & key, bool & ok);

  /*! \brief Remove a contact
   */
  bool removeContact(const mdtClConnectorContactKeyData & key);

  /*! \brief Add a list of connector contacts to database
   *
   * \param dataList List of contact data
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   */
  bool addContactList(const QList<mdtClConnectorContactData> & dataList, bool handleTransaction);

  /*! \brief Get a list of contact data for given connector
   *
   * \param key Key of connector that contains contacts
   * \return List of connector contacts.
   *        A empty list is returned if connector contains no contact, or a error occured.
   *        Use ok parameter to diffrenciate both cases.
   */
  QList<mdtClConnectorContactData> getContactDataList(const mdtClConnectorKeyData & key, bool & ok);

  /*! \brief Remove contact list of given connector
   *
   * \param key Key of connector that contains contacts to remove
   */
  bool removeContactList(const mdtClConnectorKeyData & key);

  /*! \brief Add a connector to database
   *
   * \param data Connector data to store
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   * \return Id_PK of added connector, or a null key on error
   */
  mdtClConnectorKeyData addConnector(mdtClConnectorData data, bool handleTransaction);

  /*! \brief Get connector data from database
   *
   * \param includeContactData If true, related contacts are also included in data
   * \param ok Is set false on error
   * \return data for given key.
   *       A null data is returned if given key does not exist, or a error occured.
   *       Use ok parameter to diffrenciate both cases.
   */
  mdtClConnectorData getConnectorData(const mdtClConnectorKeyData & key, bool includeContactData, bool & ok);

  /*! \brief Remove connector
   *
   * \param key Key of connector to remove
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   */
  bool removeConnector(const mdtClConnectorKeyData & key, bool handleTransaction);

 private:

  /*! \brief Fill given record with given connector contact data
   *
   * \pre record must be setup
   */
  void fillRecord(mdtSqlRecord & record, const mdtClConnectorContactData & data);

  Q_DISABLE_COPY(mdtClConnector);
};

#endif // #ifndef MDT_CL_CONNECTOR_H
