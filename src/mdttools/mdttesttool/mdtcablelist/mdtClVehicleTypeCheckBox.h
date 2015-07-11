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
#ifndef MDT_CL_VEHICLE_TYPE_CHECK_BOX_H
#define MDT_CL_VEHICLE_TYPE_CHECK_BOX_H

#include <QCheckBox>
#include <QSqlRecord>
#include <QVariant>

/*! \brief Let user assign a vehicle type to a link
 */
class mdtClVehicleTypeCheckBox : public QCheckBox
{
 public:

  /*! \brief Constructor
   *
   * \pre vehicleTypeData must contain fields VehicleType_Id_FK, Type, SubType and SeriesNumber
   * \pre VehicleType_Id_FK must not be null
   */
  mdtClVehicleTypeCheckBox(QWidget *parent, const QSqlRecord & vehicleTypeData);

  /*! \brief Get vehicle type ID
   */
  QVariant vehicleTypeId() const
  {
    return pvVehicleTypeId;
  }

 private:

  Q_DISABLE_COPY(mdtClVehicleTypeCheckBox);

  QVariant pvVehicleTypeId;
};

#endif // #ifndef MDT_CL_VEHICLE_TYPE_CHECK_BOX_H
