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
#ifndef MDT_CL_VEHICLE_TYPE_LINK_ASSIGNATION_WIDGET_H
#define MDT_CL_VEHICLE_TYPE_LINK_ASSIGNATION_WIDGET_H

#include "mdtError.h"
#include "mdtClLinkKeyData.h"
#include <QWidget>
#include <QSqlDatabase>
#include <QVariant>

class QVBoxLayout;

/*! \brief Widget for vehicle type link assignation
 */
class mdtClVehicleTypeLinkAssignationWidget : public QWidget
{
 public:

  /*! \brief Constructor
   */
  mdtClVehicleTypeLinkAssignationWidget(QWidget *parent, QSqlDatabase db);

  /*! \brief Set unit ID
   *
   * Will also list all vehicle types that are assigned to given unit
   */
  bool setUnitId(const QVariant & unitId);

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  Q_DISABLE_COPY(mdtClVehicleTypeLinkAssignationWidget);

  QSqlDatabase pvDatabase;
  QVBoxLayout *pvLayout;
  mdtError pvLastError;
};

#endif // #ifndef MDT_CL_VEHICLE_TYPE_LINK_ASSIGNATION_WIDGET_H

