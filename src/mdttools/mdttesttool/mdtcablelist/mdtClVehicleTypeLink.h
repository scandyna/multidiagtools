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
#ifndef MDT_CL_VEHICLE_TYPE_LINK_H
#define MDT_CL_VEHICLE_TYPE_LINK_H

#include "mdtClLink.h"
#include "mdtClLinkKeyData.h"
#include "mdtClVehicleTypeLinkKeyData.h"
#include <QList>
#include <QSqlRecord>

/*! \brief Helper class for vehicle type link manipulation
 *
 * Refers to VehicleType_Link_tbl
 */
class mdtClVehicleTypeLink : public mdtClLink
{
 public:

  /*! \brief Constructor
   */
  mdtClVehicleTypeLink(QObject *parent, QSqlDatabase db);

  /*! \brief Constructor
   */
  mdtClVehicleTypeLink(QSqlDatabase db);

  /*! \brief Get vehicle type link key data list for given link PK
   *
   * \param pk PK of link for witch vehicle type link are assigned
   * \return List of vehicle type - link key data.
   *          A empty list is returned if no vehicle type is assigned to given link, or a error occured.
   *          Use ok parameter to diffrenciate both cases.
   */
  QList<mdtClVehicleTypeLinkKeyData> getVehicleTypeLinkKeyDataList(const mdtClLinkPkData & pk, bool & ok);

  /*! \todo Fonction pour mettre à jours l'assignation vt-link
   *
   *  - IN: list de VT-Link "tel qu'on le veut"
   *  - DO: suppression des VT-link en trop et ajout des manquants
   *  -> Note: voir ce qui est directement possible en SQL.
   */


 private:

  /*! \brief Fill data with record from VehicleType_Link_tbl
   */
  void fillData(mdtClVehicleTypeLinkKeyData & key, const QSqlRecord & record);

  Q_DISABLE_COPY(mdtClVehicleTypeLink);
};

#endif // #ifndef MDT_CL_VEHICLE_TYPE_LINK_H
