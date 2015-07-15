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

class mdtSqlRecord;

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

  /*! \brief Add a vehicle type - link assignation
   */
  bool addVehicleTypeLink(const mdtClVehicleTypeLinkKeyData & key);

  /*! \brief Assign given vehicle type list to given link
   *
   * \param linkPk Link for witch vehicle assignations must be added
   * \param vehicleTypeList List of vehicle types to add in assignations
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   */
  bool addVehicleTypeLinks(const mdtClLinkPkData & linkPk, const QList<mdtClVehicleTypeStartEndKeyData> & vehicleTypeList, bool handleTransaction);

  /*! \brief Get vehicle type link key data list for given link PK
   *
   * \param pk PK of link for witch vehicle type link are assigned
   * \return List of vehicle type - link key data.
   *          A empty list is returned if no vehicle type is assigned to given link, or a error occured.
   *          Use ok parameter to diffrenciate both cases.
   */
  QList<mdtClVehicleTypeLinkKeyData> getVehicleTypeLinkKeyDataList(const mdtClLinkPkData & pk, bool & ok);

  /*! \brief Get vehicle type start, end, key data list for given link PK
   *
   * \param pk PK of link for witch vehicle type link are assigned
   * \return List of vehicle type start, end, key data.
   *          A empty list is returned if no vehicle type is assigned to given link, or a error occured.
   *          Use ok parameter to diffrenciate both cases.
   */
  QList<mdtClVehicleTypeStartEndKeyData> getVehicleTypeStartEndKeyDataList(const mdtClLinkPkData & pk, bool & ok);

  /*! \brief Remove a vehicle type - link assignation
   */
  bool removeVehicleTypeLink(const mdtClVehicleTypeLinkKeyData & key);

  /*! \brief Remove given vehicle type links assigned to given link
   *
   * \param linkPk Link from witch vehicle assignations must be removed
   * \param vehicleTypeList List of vehicle types to remove from linkPk
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   */
  bool removeVehicleTypeLinks(const mdtClLinkPkData & linkPk, const QList<mdtClVehicleTypeStartEndKeyData> & vehicleTypeList, bool handleTransaction);

  /*! \brief Update vehicle type - link assignation
   *
   * \param linkPk Link for witch assignations must be updated
   * \param expectedVehicleTypeKeyList List of expected vehicle types that must be assigned to link
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   */
  bool updateVehicleTypeLink(const mdtClLinkPkData & linkPk, QList<mdtClVehicleTypeStartEndKeyData> expectedVehicleTypeKeyList, bool handleTransaction);

  /*! \todo Fonction pour mettre à jours l'assignation vt-link
   *
   *  - IN: list de VT-Link "tel qu'on le veut"
   *  - DO: suppression des VT-link en trop et ajout des manquants
   *  -> Note: voir ce qui est directement possible en SQL.
   */


 private:

  /*! \brief Fill record with given key data
   */
  void fillRecord(mdtSqlRecord & record, const mdtClVehicleTypeLinkKeyData & key);

  /*! \brief Fill data with record from VehicleType_Link_tbl
   */
  void fillData(mdtClVehicleTypeLinkKeyData & key, const QSqlRecord & record);

  /*! \brief Fill data with record from VehicleType_Link_tbl
   */
  void fillData(mdtClVehicleTypeStartEndKeyData & key, const QSqlRecord & record);

  Q_DISABLE_COPY(mdtClVehicleTypeLink);
};

#endif // #ifndef MDT_CL_VEHICLE_TYPE_LINK_H
