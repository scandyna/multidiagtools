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
#include "mdtClVehicleTypeLinkKeyData.h"
#include <QWidget>
#include <QSqlDatabase>
#include <QVariant>
#include <vector>

class QVBoxLayout;
class mdtClVehicleTypeCheckBox;

/*! \brief Widget for vehicle type link assignation
 */
class mdtClVehicleTypeLinkAssignationWidget : public QWidget
{
 public:

  /*! \brief Constructor
   */
  mdtClVehicleTypeLinkAssignationWidget(QWidget *parent, QSqlDatabase db);

  /*! \brief Build list of all vehicle type that are assigned to given start unit and end unit
   *
   * \pre startUnitId and endUnitId must not be null
   */
  bool buildVehicleTypeList(const QVariant & startUnitId, const QVariant & endUnitId);

  /*! \brief Select (set checked) each vehicle type that is assigned to given link
   *
   * \pre linkPk must not be null
   */
  bool selectVehicleTypeAssignedToLink(const mdtClLinkPkData & linkPk);

  /*! \brief Clear assignation list
   */
  void clear();

  /*! \brief Get list of vehicle type that are selected
   */
  QList<mdtClVehicleTypeStartEndKeyData> getSelectedVehicleTypeList() const;

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  /*! \brief Add given item (to layout)
   */
  void addItem(mdtClVehicleTypeCheckBox *item);

  /*! \brief Set each item that matches key selected (checked)
   */
  void setMatchingItemsSelected(const mdtClVehicleTypeStartEndKeyData & key);

  Q_DISABLE_COPY(mdtClVehicleTypeLinkAssignationWidget);

  QSqlDatabase pvDatabase;
  std::vector<mdtClVehicleTypeCheckBox*> pvItems;
  QVBoxLayout *pvLayout;
  mdtError pvLastError;
};

#endif // #ifndef MDT_CL_VEHICLE_TYPE_LINK_ASSIGNATION_WIDGET_H

