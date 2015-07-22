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

/*! \brief Unit connector primary key data
 *
 * Refers to UnitConnector_tbl
 */
struct mdtClUnitConnectorPkData
{
  /*! \brief Unit connector ID (Id_PK)
   */
  QVariant id;

  /*! \brief Check if PK is null
   */
  bool isNull() const
  {
    return id.isNull();
  }

  /*! \brief Clear PK
   */
  void clear()
  {
    id.clear();
  }
};

/*! \brief Data container for unit connector key data
 *
 * Refers to UnitConnector_tbl
 */
struct mdtClUnitConnectorKeyData
{
  /*! \brief Primary key (Id_PK)
   */
  mdtClUnitConnectorPkData pk;

 private:

  /*! \brief Unit ID (Unit_Id_FK)
   */
  QVariant pvUnitId;

  /*! \brief Connector FK (Connector_Id_FK)
   */
  mdtClConnectorPkData pvConnectorFk;

  /*! \brief Article connector FK (ArticleConnector_Id_FK)
   */
  mdtClArticleConnectorKeyData pvArticleConnectorFk;

 public:

  /*! \brief Set unit ID (Unit_Id_FK)
   */
  void setUnitId(const QVariant & uid)
  {
    pvUnitId = uid;
  }

  /*! \brief Get unit ID (Unit_Id_FK)
   */
  inline QVariant unitId() const
  {
    return pvUnitId;
  }

  /*! \brief Set connector FK (Connector_Id_FK)
   *
   * \pre fk must not be null
   * \pre articleConnectorFk must not allready been set
   */
  void setConnectorFk(const mdtClConnectorPkData & fk)
  {
    Q_ASSERT(!fk.isNull());
    /*
     * When constructing new data for future storage into databse,
     * article connector's key ID is not set, in witch case it will be null.
     * To check if article connector key was set, we use the articleId, witch is mandatory
     */
    Q_ASSERT(pvArticleConnectorFk.articleId().isNull());
    pvConnectorFk = fk;
  }

  /*! \brief Get connector FK (Connector_Id_FK)
   */
  inline mdtClConnectorPkData connectorFk() const
  {
    return pvConnectorFk;
  }

  /*! \brief Set article connector FK (ArticleConnector_Id_FK)
   *
   * Will also update connectorFk with those contained in fk.
   *
   * \pre fk must not be totally null, at least its articleId mus be set
   * \pre connectorFk must not allready been set
   */
  void setArticleConnectorFk(const mdtClArticleConnectorKeyData & fk)
  {
    Q_ASSERT(!fk.articleId().isNull());
    Q_ASSERT(pvConnectorFk.isNull());
    pvArticleConnectorFk = fk;
    pvConnectorFk = fk.connectorFk();
  }

  /*! \brief Get article connector FK (ArticleConnector_Id_FK)
   */
  inline mdtClArticleConnectorKeyData articleConnectorFk() const
  {
    return pvArticleConnectorFk;
  }

  /*! \brief Check if key data is null
   *
   * Key data is null if PK or unitId is null
   */
  bool isNull() const
  {
    return (pk.isNull() || pvUnitId.isNull());
  }

  /*! \brief Clear key data
   */
  void clear()
  {
    pk.clear();
    pvUnitId.clear();
    pvConnectorFk.clear();
    pvArticleConnectorFk.clear();
  }

  /*! \brief Check if unit connector is based on a connector (from Connector_tbl)
   */
  bool isBasedOnConnector() const
  {
    return (!pvConnectorFk.isNull());
  }

  /*! \brief Check if unit connector is based on a article connector
   */
  bool isBasedOnArticleConnector() const
  {
    return (!pvArticleConnectorFk.articleId().isNull());
  }
};

#endif // #ifndef MDT_CL_UNIT_CONNECTOR_KEY_DATA_H
