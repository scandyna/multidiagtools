/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include <QList>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlRecord>

/*! \brief Data container class for link data
 *
 * Permit to echange data with Link_tbl.
 *
 * This class was made for data edition.
 *  It is also mandatory that fields matches Link_tbl.
 *  To get data from a view, witch also can miss fields, 
 *  or have different field name, considere QSqlRecord.
 */
class mdtClLinkData : public mdtSqlRecord
{
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

  /*! \brief Get start connection data (WR)
   */
  ///inline mdtClUnitConnectionData & startConnectionData() { return pvStartConnectionData; }

  /*! \brief Get end connection data (RD)
   */
  inline const mdtClUnitConnectionData & endConnectionData() const { return pvEndConnectionData; }

  /*! \brief Get end connection data (WR)
   */
  ///inline mdtClUnitConnectionData & endConnectionData() { return pvEndConnectionData; }

  /*! \brief Add vehicle type link data
   *
   * Note: in given data, unitConnectionStartId and unitConnectionEndId are not relevant,
   *  because they are set internally.
   */
  void addVehicleTypeLinkData(const mdtClVehicleTypeLinkData & data);

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
};

#endif // #ifndef MDT_CL_LINK_DATA_H
