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
#ifndef MDT_CL_LINK_DATA_H
#define MDT_CL_LINK_DATA_H

#include "mdtSqlRecord.h"
#include "mdtClUnitConnectionData.h"
#include "mdtClVehicleTypeLinkData.h"

#include "mdtClLinkKeyData.h"
#include "mdtValue.h"
#include <QVariant>

#include <QList>
#include <QSqlDatabase>
#include <QSqlRecord>

/*! \brief Data container class for link data
 *
 * Refers to Link_tbl.
 */
struct mdtClLinkData : public mdtSqlRecord /// \todo remove this inheritance once all is updated
{
 private:

  mdtClLinkKeyData pvKeyData;

 public:

  /*! \brief Link identification (Identification)
   */
  QVariant identification;

  /*! \brief Link resistance (Resistance)
   */
  mdtValueDouble resistance;

  /*! \brief Link legth (Length)
   */
  mdtValueDouble length;

  /*! \brief Set PK (UnitConnectionStart_Id_FK and UnitConnectionEnd_Id_FK)
   */
  void setPk(const mdtClLinkPkData & pk)
  {
    pvKeyData.setPk(pk);
  }

  /*! \brief Set link type (LinkType_Code_FK)
   */
  void setLinkType(mdtClLinkType_t t)
  {
    pvKeyData.setLinkType(t);
  }

  /*! \brief Set link type code (LinkType_Code_FK)
   */
  void setLinkTypeCode(const QVariant & c)
  {
    pvKeyData.setLinkTypeCode(c);
  }

  /*! \brief Set link direction (LinkDirection_Code_FK)
   */
  void setLinkDirection(mdtClLinkDirection_t d)
  {
    pvKeyData.setLinkDirection(d);
  }

  /*! \brief Set link direction code (LinkDirection_Code_FK)
   */
  void setLinkDirectionCode(const QVariant & c)
  {
    pvKeyData.setLinkDirectionCode(c);
  }

  /*! \brief Set article link FK (ArticleConnectionStart_Id_FK and ArticleConnectionEnd_Id_FK)
   */
  void setArticleLinkFk(const mdtClArticleLinkPkData & fk)
  {
    pvKeyData.setArticleLinkFk(fk);
  }

  /*! \brief Set wire ID (Wire_Id_FK)
   */
  void setWireId(const QVariant & id)
  {
    pvKeyData.setWireId(id);
  }

  /*! \brief Get key data
   */
  mdtClLinkKeyData keyData() const
  {
    return pvKeyData;
  }

  /*! \brief Get PK data
   */
  mdtClLinkPkData pk() const
  {
    return pvKeyData.pk();
  }

  /*! \brief Check if link data is null
   *
   * \sa mdtClLinkKeyData::isNull()
   */
  bool isNull() const
  {
    return pvKeyData.isNull();
  }

  /*! \brief Clear data
   */
  void clear();

 public:

  /*! \brief Construct a empty mdtClLinkData
   */
  mdtClLinkData();

  /*! \brief Contruct a mdtClLinkData from a QSqlRecord
   *
   * Note: if this method is used, setup is not relevant.
   *
   * \pre All fields from Link_tbl must exist in record
   */
  mdtClLinkData(const QSqlRecord & record);

  /*! \brief Setup fields from Link_tbl
   */
  bool setup(const QSqlDatabase & db);

  /*! \brief Clear values
   */
  void clearValues();

  /*! \brief Set start connection data
   *
   * Will also update following data:
   *  - UnitConnectionStart_Id_FK
   *  - ArticleConnectionStart_Id_FK
   *
   * Internal vehicle type link data list is also updated with new start unit connection IDs
   */
  void setStartConnectionData(const mdtClUnitConnectionData & cnnData);

  /*! \brief Set end connection data
   *
   * Will also update following data:
   *  - UnitConnectionEnd_Id_FK
   *  - ArticleConnectionEnd_Id_FK
   *
   * Internal vehicle type link data list is also updated with new end unit connection IDs
   */
  void setEndConnectionData(const mdtClUnitConnectionData & cnnData);

  /*! \brief Set start and end connection data
   *
   * Will also update following data:
   *  - UnitConnectionStart_Id_FK
   *  - UnitConnectionEnd_Id_FK
   *  - ArticleConnectionStart_Id_FK
   *  - ArticleConnectionEnd_Id_FK
   *
   * Internal vehicle type link data list is also updated with new start and end unit connection IDs
   */
  void setConnectionData(const mdtClUnitConnectionData & startCnnData, const mdtClUnitConnectionData & endCnnData);

  /*! \brief Get start connection data (RD)
   */
  inline const mdtClUnitConnectionData & startConnectionData() const { return pvStartConnectionData; }

  /*! \brief Get end connection data (RD)
   */
  inline const mdtClUnitConnectionData & endConnectionData() const { return pvEndConnectionData; }

  /*! \brief Check if a vehicle type link was edited
   */
  inline bool vehicleTypeLinksEdited() const { return pvVehicleTypeLinksEdited; }

  /*! \brief Add vehicle type link data
   *
   * Note: in given data, unitConnectionStartId and unitConnectionEndId are not relevant,
   *  because they are set internally.
   */
  void addVehicleTypeLinkData(const mdtClVehicleTypeLinkData & data, bool updateVehicleLinksEditedFlags = true);

  /*! \brief Get list of vehicle type link data
   */
  inline const QList<mdtClVehicleTypeLinkData> & vehicleTypeLinkDataList() const { return pvVehicleTypeLinkDataList; }

  /*! \brief Clear only vehicle type link data list
   */
  void clearVehicleTypeLinkDataList();

 private:
  
  mdtClUnitConnectionData pvStartConnectionData;
  mdtClUnitConnectionData pvEndConnectionData;
  QList<mdtClVehicleTypeLinkData> pvVehicleTypeLinkDataList;
  bool pvVehicleTypeLinksEdited;
};

#endif // #ifndef MDT_CL_LINK_DATA_H
