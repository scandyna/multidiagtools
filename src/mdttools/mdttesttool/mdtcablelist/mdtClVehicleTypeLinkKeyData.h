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
#ifndef MDT_CL_VEHICLE_TYPE_LINK_KEY_DATA_H
#define MDT_CL_VEHICLE_TYPE_LINK_KEY_DATA_H

#include "mdtClLinkKeyData.h"
#include <QVariant>

/*! \brief Vehicle type - link key data
 *
 * Refers to VehicleType_Link_tbl
 */
struct mdtClVehicleTypeLinkKeyData
{
 private:

  QVariant pvVehicleTypeStartId;
  QVariant pvVehicleTypeEndId;
  mdtClLinkPkData pvLinkFk;

 public:

  /*! \brief Set start vehicle type (VehicleTypeStart_Id_FK)
   */
  void setVehicleTypeStartId(const QVariant & id)
  {
    pvVehicleTypeStartId = id;
  }

  /*! \brief Get start vehicle type (VehicleTypeStart_Id_FK)
   */
  QVariant vehicleTypeStartId() const
  {
    return pvVehicleTypeStartId;
  }

  /*! \brief Set end vehicle type (VehicleTypeEnd_Id_FK)
   */
  void setVehicleTypeEndId(const QVariant & id)
  {
    pvVehicleTypeEndId = id;
  }

  /*! \brief Get end vehicle type (VehicleTypeEnd_Id_FK)
   */
  QVariant vehicleTypeEndId() const
  {
    return pvVehicleTypeEndId;
  }

  /*! \brief Set link FK (UnitConnectionStart_Id_FK and UnitConnectionEnd_Id_FK)
   *
   * \pre fk must not be null
   */
  void setLinkFk(const mdtClLinkPkData & fk)
  {
    Q_ASSERT(!fk.isNull());
    pvLinkFk = fk;
  }

  /*! \brief Get link FK (UnitConnectionStart_Id_FK and UnitConnectionEnd_Id_FK)
   */
  mdtClLinkPkData linkFk() const
  {
    return pvLinkFk;
  }

  /*! \brief Check if key data is null
   *
   * Key data is null if one vehicle ID or link FK is null
   */
  bool isNull() const
  {
    return (pvVehicleTypeStartId.isNull() || pvVehicleTypeEndId.isNull() || pvLinkFk.isNull());
  }

  /*! \brief Clear key data
   */
  void clear()
  {
    pvVehicleTypeStartId.clear();
    pvVehicleTypeEndId.clear();
    pvLinkFk.clear();
  }
};

#endif // #ifndef MDT_CL_VEHICLE_TYPE_LINK_KEY_DATA_H
