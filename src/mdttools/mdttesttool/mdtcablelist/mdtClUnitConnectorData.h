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
#ifndef MDT_CL_UNIT_CONNECTOR_DATA_H
#define MDT_CL_UNIT_CONNECTOR_DATA_H

#include "mdtSqlRecord.h"

#include "mdtClUnitConnectorKeyData.h"
#include "mdtClUnitConnectionData.h"
#include "mdtClConnectorData.h"
#include "mdtClArticleConnectorData.h"
#include <QList>
#include <QVariant>
#include <QSqlDatabase>

/*! \brief Data container class for article connector data
 *
 * Permit to echange data with UnitConnector_tbl and UnitConnection_tbl.
 */
struct mdtClUnitConnectorData : public mdtSqlRecord  /// \todo remove this inheritance once all is done
{
 private:

  /*! \brief Key data
   */
  mdtClUnitConnectorKeyData pvKeyData;

 public:

  /*! \brief Get key data
   */
  inline mdtClUnitConnectorKeyData keyData() const
  {
    return pvKeyData;
  }

  /*! \brief Set key data
   *
   * \pre key must at least have its unitId set.
   * \pre keyData must not allready been set
   */
  void setKeyData(const mdtClUnitConnectorKeyData & key)
  {
    Q_ASSERT(!key.unitId().isNull());
    Q_ASSERT(pvKeyData.unitId().isNull());
    Q_ASSERT(!pvKeyData.isBasedOnConnector());
    Q_ASSERT(!pvKeyData.isBasedOnArticleConnector());
    pvKeyData = key;
  }

  /*! \brief Set/update PK
   *
   * If connector contains connections,
   *  they will be updated.
   */
  void setPk(const mdtClUnitConnectorPkData & pk);

  /*! \brief Set/update unit connector ID (Id_PK)
   *
   * If connector contains connections,
   *  they will be updated.
   */
  void setId(const QVariant & id);

  /*! \brief Check if unit connector data is null
   *
   * Data is null if key data is null
   */
  inline bool isNull() const
  {
    return pvKeyData.isNull();
  }

  /*! \brief Clear data
   */
  void clear();

  /*! \brief Check if unit connector is based on a connector (from Connector_tbl)
   */
  inline bool isBasedOnConnector() const
  {
    return pvKeyData.isBasedOnConnector();
  }

  /*! \brief Check if unit connector is based on a article connector
   */
  inline bool isBasedOnArticleConnector() const
  {
    return pvKeyData.isBasedOnArticleConnector();
  }

  /*! \brief Connector name (Name)
   */
  QVariant name;

  /*! \brief Add connection data
   *
   * In key that is contained in given data,
   *  if its id is null (that tells that connector is freshly created, not fetched from database),
   *  unitId and unitConnectorFk will be updated
   *  to match this unit connector befor adding connection.
   *
   * \pre In this unit connector's key, unitId must be set
   * \pre In data's key, pk must be null and unitConnectorFk must not allready been set,
   *      or pk is not null and unitConnectorFk must match this connector's key
   */
  void addConnectionData(mdtClUnitConnectionData data);

  /*! \brief Add a list of connection data
   *
   * \sa addConnectionData().
   */
  void setConnectionDataList(const QList<mdtClUnitConnectionData> & dataList);

  /*! \brief Access list of connection data
   */
  inline const QList<mdtClUnitConnectionData> & connectionDataList() const
  {
    return pvConnectionDataList;
  }

 public:

  /*! \brief Construct a empty mdtClUnitConnectorData
   */
  mdtClUnitConnectorData();

  /*! \brief Contruct a mdtClUnitConnectorData from a QSqlRecord
   *
   * Note: if this method is used, setup is not relevant.
   *
   * \pre All fields from UnitConnector_tbl must exist in record
   */
  mdtClUnitConnectorData(const QSqlRecord & record);

  /*! \brief Setup fields from UnitConnector_tbl
   *
   * \param setupCd If true, fields from connector part are also added.
   * \param setupAcd If true, fields from article connector part are also added.
   */
  bool setup(const QSqlDatabase & db, bool setupCd, bool setupAcd);

  /*! \brief Clear values
   *
   * Will clear values, including connection data.
   *  Fields are keeped.
   */
  void clearValues();

  /*! \brief Clear values and fields
   *
   * Will clear values, including connection data.
   *  Fields are also removed.
   */
//   void clear();

  /*! \brief Get list of connection data
   */
//   const QList<mdtClUnitConnectionData> & connectionDataList() const;

  /*! \brief Get list of connection data
   */
  QList<mdtClUnitConnectionData> & connectionDataList();

  /*! \brief Update existing connection data
   *
   * Will update connection data for witch Id_PK matches given connectionId.
   *
   * \return False if connectionId was not found.
   */
  bool setConnectionData(const QVariant & connectionId, const mdtClUnitConnectionData & data);

  /*! \brief Get connection data for given connection ID
   *
   * Return the connection data for witch Id_PK matches given connectionId.
   *  ok will be set to false if connectionId was not found.
   */
  mdtClUnitConnectionData connectionData(const QVariant & connectionId, bool *ok) const;

  /*! \brief Set connector data
   *
   * Will also update Connector_Id_FK and set ArticleConnector_Id_FK to Null.
   */
  void setConnectorData(const mdtClConnectorData & data);

  /*! \brief Get connector data (from Connector_tbl)
   */
  mdtClConnectorData connectorData() const;

  /*! \brief Set article connector data
   *
   * Will also update Connector_Id_FK and ArticleConnector_Id_FK
   */
  void setArticleConnectorData(const mdtClArticleConnectorData & data);

  /*! \brief Get article connector data
   */
  mdtClArticleConnectorData articleConnectorData() const;

  /*! \brief Check if unit connector is based on a article connector
   */
//   bool isBasedOnArticleConnector() const;

  /*! \brief Check if unit connector is based on a connector (from Connector_tbl)
   */
//   bool isBasedOnConnector() const;

 private:

  QList<mdtClUnitConnectionData> pvConnectionDataList;
  mdtClConnectorData pvConnectorData;
  mdtClArticleConnectorData pvArticleConnectorData;
};

#endif // #ifndef MDT_CL_UNIT_CONNECTOR_DATA_H
