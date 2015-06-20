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
#ifndef MDT_CL_UNIT_CONNECTION_DATA_H
#define MDT_CL_UNIT_CONNECTION_DATA_H

#include "mdtClUnitConnectionKeyData.h"
#include "mdtClUnitConnectorKeyData.h"

#include "mdtSqlRecord.h"
#include "mdtClArticleConnectionData.h"

#include <QList>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlRecord>

/*! \brief Data container class for unit connection data
 *
 * Permit to echange data with UnitConnection_tbl.
 */
struct mdtClUnitConnectionData : public mdtSqlRecord /// \todo Remove this inheritance once all is done
{
 private:

  /*! \brief Key data
   */
  mdtClUnitConnectionKeyData pvKeyData;

 public:

  /*! \brief Get key data
   */
  inline mdtClUnitConnectionKeyData keyData() const
  {
    return pvKeyData;
  }

  /*! \brief Set key data
   */
  void setKeyData(const mdtClUnitConnectionKeyData & key);

  /*! \brief Set unit connector FK
   *
   * \sa mdtClUnitConnectionKeyData::setUnitConnectorFk()
   */
  void setUnitConnectorFk(const mdtClUnitConnectorKeyData & fk);

  /*! \brief Clear unit connector FK (UnitConnector_Id_FK)
   */
  void clearUnitConnectorFk();

  /*! \brief Set article connection FK (ArticleConnection_Id_FK)
   *
   * \sa mdtClUnitConnectionKeyData::setArticleConnectionFk()
   */
  void setArticleConnectionFk(const mdtClArticleConnectionKeyData & fk);

  /*! \brief Clear article connection FK (ArticleConnection_Id_FK)
   */
  void clearArticleConnectionFk();

  /*! \brief Check if unit connection is part of a unit connector
   */
  inline bool isPartOfUnitConnector() const
  {
    return pvKeyData.isPartOfUnitConnector();
  }

  /*! \brief Check if unit connection is based on a article connection
   */
  inline bool isBasedOnArticleConnection() const
  {
    return pvKeyData.isBasedOnArticleConnection();
  }

  /*! \brief Set connection type
   *
   * \pre Setting connection type is not allowed if unit connection is based on a article connection
   */
  void setConnectionType(mdtClConnectionType_t t);

  /*! \brief Get connection type
   */
  inline mdtClConnectionType_t connectionType() const
  {
    return pvKeyData.connectionTypeFk().type();
  }

  /*! \brief Check if data is null
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

  /*! \brief name (UnitContactName)
   */
  QVariant name;

  /*! \brief Resistance (Resistance)
   */
  QVariant resistance;

  /*! \brief Schema page (SchemaPage)
   */
  QVariant schemaPage;

  /*! \brief Signal name (SignalName)
   */
  QVariant signalName;

  /*! \brief SW address (SwAddress)
   */
  QVariant swAddress;

  /*! \brief Function EN (FunctionEN)
   */
  QVariant functionEN;

  /*! \brief Function FR (FunctionFR)
   */
  QVariant functionFR;

  /*! \brief Function DE (FunctionDE)
   */
  QVariant functionDE;

  /*! \brief Function IT (FunctionIT)
   */
  QVariant functionIT;

 public:

  /*! \brief Construct a empty mdtClUnitConnectionData
   */
  mdtClUnitConnectionData();

  /*! \brief Contruct a mdtClUnitConnectionData from a QSqlRecord
   *
   * Note: if this method is used, setup is not relevant.
   *
   * \pre All fields from UnitConnection_tbl must exist in record
   */
  mdtClUnitConnectionData(const QSqlRecord & record);

  /*! \brief Setup fields from UnitConnection_tbl
   *
   * \param setupAcd If true, fields from article connection part are also added.
   */
  bool setup(const QSqlDatabase & db, bool setupAcd);

  /*! \brief Set article connection data
   *
   * Will also update ArticleConnection_Id_FK
   */
  void setArticleConnectionData(const mdtClArticleConnectionData & data);

  /*! \brief Access article connection data (RD)
   */
  const mdtClArticleConnectionData & articleConnectionData() const;

  /*! \brief Access article connection data (WR)
   */
  mdtClArticleConnectionData & articleConnectionData();


 private:

  mdtClArticleConnectionData pvArticleConnectionData;
};

#endif // #ifndef MDT_CL_UNIT_CONNECTION_DATA_H
