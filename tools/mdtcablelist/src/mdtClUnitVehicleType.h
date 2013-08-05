/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QList>
#include <QVariant>
#include <QModelIndex>

/*! \brief Helper class for Unit <-> VehicleType assignments
 */
class mdtClUnitVehicleType
{
 public:

  /*! \brief Constructor
   */
  mdtClUnitVehicleType(QSqlDatabase db);

  /*! \brief Destructor
   */
  ~mdtClUnitVehicleType();

  /*! \brief Get a list of vehicle type that are not assigned to given unit
   */
  QSqlQueryModel &vehicleTypeNotAssignedToUnitModel(const QVariant &unitId);

  /*! \brief Add unit <-> vehicle type assignments
   *
   * \pre vehicleTypeIdList must contain only 1 column.
   */
  bool addUnitVehicleTypeAssignments(const QVariant & unitId, const QModelIndexList & vehicleTypeIdList);

  /*! \brief Remove unit <-> vehicle type assignments
   */
  bool removeUnitVehicleAssignments(const QVariant & unitId, const QModelIndexList & vehicleTypeIdList);

  /*! \brief Get last occured error
   */
  QSqlError lastError() const;

 private:

  /*! \brief Clear last error and set it invalid
   */
  void clearLastError();

  Q_DISABLE_COPY(mdtClUnitVehicleType);

  QSqlQueryModel pvVehicleTypeNotAssignedToUnitModel;
  QSqlDatabase pvDatabase;
  QSqlQueryModel pvUnitVehicleTypeModel;
  QSqlError pvLastError;
};

#endif  // #ifndef MDT_CL_UNIT_VEHICLE_TYPE_H
