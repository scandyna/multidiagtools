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

 private:

  /*! \brief Unit ID (Unit_Id_FK)
   */
  QVariant pvUnitId;

  /*! \brief Unit connector ID (UnitConnector_Id_FK)
   */
  mdtClUnitConnectorKeyData pvUnitConnectorFk;

  /*! \brief Article connection FK (ArticleConnection_Id_FK)
   */
  mdtClArticleConnectionKeyData pvArticleConnectionFk;

  /*! \brief Connection type (ConnectionType_Code_FK)
   */
  mdtClConnectionTypeKeyData pvConnectionTypeFk;

 public:

  /*! \brief Set unit ID (Unit_Id_FK)
   *
   * \pre uid must not be null
   * \pre unitConnectorFk must not allready been set
   */
  void setUnitId(const QVariant & uid)
  {
    Q_ASSERT(!uid.isNull());
    /*
     * By constructing data for insertion into database,
     * unitConnectorFk's id is not allready set, so unitConnectorFk will also be null.
     * To check if it was allready set, we check its unitId, witch is mandatory
     */
    Q_ASSERT(pvUnitConnectorFk.unitId().isNull());
    pvUnitId = uid;
  }

  /*! \brief Get unit ID (Unit_Id_FK)
   */
  inline QVariant unitId() const
  {
    return pvUnitId;
  }

  /*! \brief Set unit connector FK (UnitConnector_Id_FK)
   *
   * \pre fk must not be totally null, at least its unitId must be set
   * \pre fk's unitId must match unitId
   * \pre If fk is based on a article connector and articleConnectionFk was allready set,
   *      fk's article connector must match articleConnectionFk's article connector
   */
  void setUnitConnectorFk(const mdtClUnitConnectorKeyData & fk)
  {
    /*
     * By constructing data for insertion into database,
     * fk's id is not allready set, so fk will also be null.
     * To check if it was allready set, we check its unitId, witch is mandatory
     */
    Q_ASSERT(!fk.unitId().isNull());
    Q_ASSERT(fk.unitId() == pvUnitId);
    Q_ASSERT( ( !fk.isBasedOnArticleConnector() ) || ( pvArticleConnectionFk.articleId().isNull() ) ||
              ( ( (fk.articleConnectorFk().id == pvArticleConnectionFk.articleConnectorFk().id) ) &&
                ( (fk.articleConnectorFk().articleId() == pvArticleConnectionFk.articleConnectorFk().articleId()) ) ) );
    pvUnitConnectorFk = fk;
  }

  /*! \brief Get unit connector FK (UnitConnector_Id_FK)
   */
  inline mdtClUnitConnectorKeyData unitConnectorFk() const
  {
    return pvUnitConnectorFk;
  }

  /*! \brief Set article connection FK (ArticleConnection_Id_FK)
   *
   * connectionType will be updated with fk's connectionType.
   *
   * \pre fk must not be totally null, its articleId must be set
   * \pre If fk is part of a article connector and unit connector was allready set,
   *      fk's article connector must match unitConnectorFk's article connector
   */
  void setArticleConnectionFk(const mdtClArticleConnectionKeyData & fk)
  {
    /*
     * By constructing data for insertion into database,
     * fk's id is not allready set, so fk will also be null.
     * To check if it was allready set, we check its unitId, witch is mandatory
     */
    Q_ASSERT(!fk.articleId().isNull());
    Q_ASSERT( ( !fk.isPartOfArticleConnector() ) || ( pvUnitConnectorFk.unitId().isNull() ) ||
              ( ( fk.articleConnectorFk().id == pvUnitConnectorFk.articleConnectorFk().id ) &&
                ( fk.articleConnectorFk().articleId() == pvUnitConnectorFk.articleConnectorFk().articleId() ) ) );
    pvArticleConnectionFk = fk;
    pvConnectionTypeFk.setType(fk.connectionTypeFk().type());
  }

  /*! \brief Get article connection FK (ArticleConnection_Id_FK)
   */
  inline mdtClArticleConnectionKeyData articleConnectionFk() const
  {
    return pvArticleConnectionFk;
  }

  /*! \brief Set connection type
   *
   * \pre articleConnectionFk must not allready been set
   */
  void setConnectionType(mdtClConnectionType_t t)
  {
    /*
     * By constructing data for insertion into database,
     * articleConnectionFk's id is not allready set, so articleConnectionFk will also be null.
     * To check if it was allready set, we check its articleId, witch is mandatory
     */
    Q_ASSERT(pvArticleConnectionFk.articleId().isNull());
    pvConnectionTypeFk.setType(t);
  }

  /*! \brief Set connection type code (ConnectionType_Code_FK)
   *
   * \pre articleConnectionFk must not allready been set
   */
  void setConnectionTypeCode(const QVariant & c)
  {
    /*
     * By constructing data for insertion into database,
     * articleConnectionFk's id is not allready set, so articleConnectionFk will also be null.
     * To check if it was allready set, we check its articleId, witch is mandatory
     */
    Q_ASSERT(pvArticleConnectionFk.articleId().isNull());
    pvConnectionTypeFk.code = c;
  }

  /*! \brief Get connection type FK (ConnectionType_Code_FK)
   */
  inline mdtClConnectionTypeKeyData connectionTypeFk() const
  {
    return pvConnectionTypeFk;
  }

  /*! \brief Check if key data is null
   *
   * Key data is null if id, unitId or connectionTypeFk is null
   */
  bool isNull() const
  {
    return (id.isNull() || pvUnitId.isNull() || pvConnectionTypeFk.isNull());
  }

  /*! \brief Clear
   */
  void clear()
  {
    id.clear();
    pvUnitId.clear();
    pvUnitConnectorFk.clear();
    pvArticleConnectionFk.clear();
    pvConnectionTypeFk.clear();
  }

  /*! \brief Check if unit connection is part of a unit connector
   */
  bool isPartOfUnitConnector() const
  {
    /*
     * By constructing data for insertion into database,
     * unitConnectorFk's id is not allready set, so unitConnectorFk will also be null.
     * To check if it was allready set, we check its unitId, witch is mandatory
     */
    return (!pvUnitConnectorFk.unitId().isNull());
  }

  /*! \brief Check if unit connection is based on a article connection
   */
  bool isBasedOnArticleConnection() const
  {
    /*
     * By constructing data for insertion into database,
     * articleConnectionFk's id is not allready set, so articleConnectionFk will also be null.
     * To check if it was allready set, we check its articleId, witch is mandatory
     */
    return (!pvArticleConnectionFk.articleId().isNull());
  }
};

#endif // #ifndef MDT_CL_UNIT_CONNECTION_KEY_DATA_H
