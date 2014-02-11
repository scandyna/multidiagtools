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
#ifndef MDT_CL_UNIT_VEHICLE_TYPE_H
#define MDT_CL_UNIT_VEHICLE_TYPE_H

#include "mdtTtBase.h"
#include <QSqlDatabase>
#include <QList>
#include <QVariant>
#include <QString>
#include <QModelIndex>

/*! \brief Helper class for Unit <-> VehicleType assignments
 */
class mdtClUnitVehicleType : public mdtTtBase
{
 public:

  /*! \brief Constructor
   */
  mdtClUnitVehicleType(QObject *parent, QSqlDatabase db);

  /*! \brief Destructor
   */
  ~mdtClUnitVehicleType();

  /*! \brief Get SQL statement for vehicle type assignment selection
   *
   * List all vehicle types that are not assigned to given unit
   */
  QString sqlForVehicleTypeNotAssignedToUnit(const QVariant & unitId) const;

  /*! \brief Add unit <-> vehicle type assignment
   */
  bool addUnitVehicleTypeAssignment(const QVariant & unitId, const QVariant & vehicleTypeId);

  /*! \brief Add unit <-> vehicle type assignments
   */
  bool addUnitVehicleTypeAssignments(const QVariant & unitId, const QList<QVariant> & vehicleTypeIdList);

  /*! \brief Add unit <-> vehicle type assignments
   *
   * \pre vehicleTypeIdList must contain only 1 column.
   */
  bool addUnitVehicleTypeAssignments(const QVariant & unitId, const QModelIndexList & vehicleTypeIdList);

  /*! \brief Remove unit <-> vehicle type assignment
   */
  bool removeUnitVehicleAssignment(const QVariant & unitId, const QVariant & vehicleTypeId);

  /*! \brief Remove unit <-> vehicle type assignments
   */
  bool removeUnitVehicleAssignments(const QVariant & unitId, const QList<QVariant> & vehicleTypeIdList);

  /*! \brief Remove unit <-> vehicle type assignments
   */
  bool removeUnitVehicleAssignments(const QVariant & unitId, const QModelIndexList & vehicleTypeIdList);

 private:

  Q_DISABLE_COPY(mdtClUnitVehicleType);
};

#endif  // #ifndef MDT_CL_UNIT_VEHICLE_TYPE_H
