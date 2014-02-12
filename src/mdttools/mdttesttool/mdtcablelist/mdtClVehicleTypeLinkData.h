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
#ifndef MDT_CL_VEHICLE_TYPE_LINK_DATA_H
#define MDT_CL_VEHICLE_TYPE_LINK_DATA_H

#include <QVariant>

/*! \brief Data exchange container for vehicle type links
 */
class mdtClVehicleTypeLinkData
{
 public:

  /*! \brief Set vehicle type start ID
   */
  void setVehicleTypeStartId(const QVariant & value);

  /*! \brief Get vehicle type start ID
   */
  inline const QVariant vehicleTypeStartId() const;

  /*! \brief Set vehicle type end ID
   */
  void setVehicleTypeEndId(const QVariant & value);

  /*! \brief Get vehicle type end ID
   */
  inline const QVariant vehicleTypeEndId() const;

  /*! \brief Set unit connection start ID
   */
  void setUnitConnectionStartId(const QVariant & value);

  /*! \brief Get unit connection start ID
   */
  inline const QVariant unitConnectionStartId() const;

  /*! \brief Set unit connection end ID
   */
  void setUnitConnectionEndId(const QVariant & value);

  /*! \brief Get unit connection end ID
   */
  inline const QVariant unitConnectionEndId() const;

 private:

  QVariant pvVehicleTypeStartId;
  QVariant pvVehicleTypeEndId;
  QVariant pvUnitConnectionStartId;
  QVariant pvUnitConnectionEndId;
};

inline const QVariant mdtClVehicleTypeLinkData::vehicleTypeStartId() const 
{
  return pvVehicleTypeStartId;
}

inline const QVariant mdtClVehicleTypeLinkData::vehicleTypeEndId() const 
{
  return pvVehicleTypeEndId;
}

inline const QVariant mdtClVehicleTypeLinkData::unitConnectionStartId() const 
{
  return pvUnitConnectionStartId;
}

inline const QVariant mdtClVehicleTypeLinkData::unitConnectionEndId() const 
{
  return pvUnitConnectionEndId;
}


#endif // #ifndef MDT_CL_VEHICLE_TYPE_LINK_DATA_H
