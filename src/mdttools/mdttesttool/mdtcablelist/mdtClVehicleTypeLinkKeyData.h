/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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

/*! \brief Vehicle type start, end key data
 *
 * Refers to VehicleType_Link_tbl
 */
struct mdtClVehicleTypeStartEndKeyData
{
 private:

  QVariant pvVehicleTypeStartId;
  QVariant pvVehicleTypeEndId;

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

  /*! \brief Check if key data is null
   *
   * Key data is null if one vehicle ID is null
   */
  bool isNull() const
  {
    return (pvVehicleTypeStartId.isNull() || pvVehicleTypeEndId.isNull());
  }

  /*! \brief Clear key data
   */
  void clear()
  {
    pvVehicleTypeStartId.clear();
    pvVehicleTypeEndId.clear();
  }
};

/*! \internal less than comparison functor for mdtClVehicleTypeStartEndKeyData
 */
struct mdtClVehicleTypeStartEndLessThan
{
  bool operator()(const mdtClVehicleTypeStartEndKeyData & a, const mdtClVehicleTypeStartEndKeyData & b)
  {
    qlonglong a1 = a.vehicleTypeStartId().toLongLong();
    qlonglong a2 = a.vehicleTypeEndId().toLongLong();
    qlonglong b1 = b.vehicleTypeStartId().toLongLong();
    qlonglong b2 = b.vehicleTypeEndId().toLongLong();

    if(a1 != b1){
      return a1 < b1;
    }
    return a2 < b2;
  }
};

/*! \brief Vehicle type - link key data
 *
 * Refers to VehicleType_Link_tbl
 */
struct mdtClVehicleTypeLinkKeyData
{
 private:

  mdtClVehicleTypeStartEndKeyData pvVehicleTypeStartEndFk;
  LinkPk pvLinkFk;

 public:

  /*! \brief Set start vehicle type (VehicleTypeStart_Id_FK)
   */
  void setVehicleTypeStartId(const QVariant & id)
  {
    pvVehicleTypeStartEndFk.setVehicleTypeStartId(id);
  }

  /*! \brief Get start vehicle type (VehicleTypeStart_Id_FK)
   */
  QVariant vehicleTypeStartId() const
  {
    return pvVehicleTypeStartEndFk.vehicleTypeStartId();
  }

  /*! \brief Set end vehicle type (VehicleTypeEnd_Id_FK)
   */
  void setVehicleTypeEndId(const QVariant & id)
  {
    pvVehicleTypeStartEndFk.setVehicleTypeEndId(id);
  }

  /*! \brief Get end vehicle type (VehicleTypeEnd_Id_FK)
   */
  QVariant vehicleTypeEndId() const
  {
    return pvVehicleTypeStartEndFk.vehicleTypeEndId();
  }

  /*! \brief Set vehicle type start/end FK part
   */
  void setVehicleTypeStartEndFk(const mdtClVehicleTypeStartEndKeyData & fk)
  {
    pvVehicleTypeStartEndFk = fk;
  }

  /*! \brief Get vehicle type start/end FK part
   */
  mdtClVehicleTypeStartEndKeyData vehicleTypeStartEndFk() const
  {
    return pvVehicleTypeStartEndFk;
  }

  /*! \brief Set link FK (UnitConnectionStart_Id_FK and UnitConnectionEnd_Id_FK)
   *
   * \pre fk must not be null
   */
  void setLinkFk(const LinkPk & fk)
  {
    Q_ASSERT(!fk.isNull());
    pvLinkFk = fk;
  }

  /*! \brief Get link FK (UnitConnectionStart_Id_FK and UnitConnectionEnd_Id_FK)
   */
  LinkPk linkFk() const
  {
    return pvLinkFk;
  }

  /*! \brief Check if key data is null
   *
   * Key data is null if one vehicle ID or link FK is null
   */
  bool isNull() const
  {
    return (pvVehicleTypeStartEndFk.isNull() || pvLinkFk.isNull());
  }

  /*! \brief Clear key data
   */
  void clear()
  {
    pvVehicleTypeStartEndFk.clear();
    pvLinkFk.clear();
  }
};

#endif // #ifndef MDT_CL_VEHICLE_TYPE_LINK_KEY_DATA_H
