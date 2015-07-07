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
#ifndef MDT_CL_VEHICLE_TYPE_LINK_ASSIGNATION_WIDGET_ITEM_H
#define MDT_CL_VEHICLE_TYPE_LINK_ASSIGNATION_WIDGET_ITEM_H

#include "mdtClVehicleTypeLinkKeyData.h"
#include "mdtClLinkKeyData.h"
#include <QCheckBox>
#include <QSqlRecord>

/*! \brief Let user assign a vehicle type to a link
 */
class mdtClVehicleTypeLinkAssignationWidgetItem : public QCheckBox
{
 public:

  /*! \brief Constructor
   *
   * \pre vehicleTypeData must contain fields VehicleType_Id_FK, Type, SubType and SeriesNumber
   * \pre VehicleType_Id_FK must not be null
   */
  mdtClVehicleTypeLinkAssignationWidgetItem(QWidget *parent, const QSqlRecord & vehicleTypeData);

  /*! \brief Set link PK
   */
  void setLink(const mdtClLinkPkData & pk);

  /*! \brief Get key data
   */
  mdtClVehicleTypeLinkKeyData keyData() const
  {
    return pvKeyData;
  }

 private:

  Q_DISABLE_COPY(mdtClVehicleTypeLinkAssignationWidgetItem);

  mdtClVehicleTypeLinkKeyData pvKeyData;
};

#endif // #ifndef MDT_CL_VEHICLE_TYPE_LINK_ASSIGNATION_WIDGET_ITEM_H
