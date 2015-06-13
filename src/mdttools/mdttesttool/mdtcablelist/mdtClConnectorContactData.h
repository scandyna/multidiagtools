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
#ifndef MDT_CL_CONNECTOR_CONTACT_DATA_H
#define MDT_CL_CONNECTOR_CONTACT_DATA_H

#include "mdtClConnectionTypeData.h"
#include "mdtClConnectorKeyData.h"
#include <QVariant>

/*! \brief Connector contact key data
 *
 * Refers to ConnectorContact_tbl
 */
struct mdtClConnectorContactKeyData
{
  /*! \brief Contact ID (Id_PK)
   */
  QVariant id;

 private:

  /*! \brief Connector ID (Connector_Id_FK)
   */
  mdtClConnectorKeyData pvConnectorFk;

  /*! \brief Connection type (ConnectionType_Code_FK)
   */
  mdtClConnectionTypeKeyData pvConnectionTypeFk;

 public:

  /*! \brief Set connector FK (Connector_Id_FK)
   *
   * \pre fk must not be null.
   */
  void setConnectorFk(const mdtClConnectorKeyData & fk)
  {
    Q_ASSERT(!fk.isNull());
    pvConnectorFk = fk;
  }

  /*! \brief Get connector FK (Connector_Id_FK)
   */
  inline mdtClConnectorKeyData connectorFk() const
  {
    return pvConnectorFk;
  }

  /*! \brief Set connection type
   */
  void setConnectionType(mdtClConnectionType_t t)
  {
    pvConnectionTypeFk.setType(t);
  }

  /*! \brief Set connection type code (ConnectionType_Code_FK)
   */
  void setConnectionTypeCode(const QVariant & c)
  {
    pvConnectionTypeFk.code = c;
  }

  /*! \brief Get connection type FK
   */
  inline mdtClConnectionTypeKeyData connectionTypeFk() const
  {
    return pvConnectionTypeFk;
  }

  /*! \brief Check if key is null
   *
   * Key is null if one of the 3 key is null
   */
  bool isNull() const
  {
    return (id.isNull() || pvConnectorFk.isNull() || pvConnectionTypeFk.isNull());
  }

  /*! \brief Clear key
   */
  void clear()
  {
    id.clear();
    pvConnectorFk.clear();
    pvConnectionTypeFk.clear();
  }
};

/*! \brief Connector contact data
 *
 * Refers to ConnectorContact_tbl
 */
struct mdtClConnectorContactData
{
 private:

  /*! \brief Contact key data
   */
  mdtClConnectorContactKeyData pvKeyData;

 public:

  /*! \brief Get key data
   */
  inline mdtClConnectorContactKeyData keyData() const
  {
    return pvKeyData;
  }

  /*! \brief Set key data
   */
  void setKeyData(const mdtClConnectorContactKeyData & key)
  {
    pvKeyData = key;
  }

  /*! \brief Set connector foreign key data
   */
  void setConnectorFk(const mdtClConnectorKeyData & fk)
  {
    pvKeyData.setConnectorFk(fk);
  }

  /*! \brief Set connection type
   */
  void setConnectionType(mdtClConnectionType_t t)
  {
    pvKeyData.setConnectionType(t);
  }

  /*! \brief Get connection type
   */
  mdtClConnectionType_t connectionType() const
  {
    return pvKeyData.connectionTypeFk().type();
  }

  /*! \brief Contact name (Name)
   */
  QVariant name;

  /*! \brief Check if contact data is null
   *
   * Data is null if key is null
   */
  bool isNull() const
  {
    return pvKeyData.isNull();
  }

  /*! \brief Clear data
   */
  void clear()
  {
    pvKeyData.clear();
    name.clear();
  }
};

#endif // #ifndef MDT_CL_CONNECTOR_CONTACT_DATA_H
