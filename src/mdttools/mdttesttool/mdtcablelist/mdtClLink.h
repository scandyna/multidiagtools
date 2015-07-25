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
#ifndef MDT_CL_LINK_H
#define MDT_CL_LINK_H

#include "mdtTtBase.h"
#include "mdtClLinkData.h"
#include "mdtClLinkModificationKeyData.h"
#include "mdtClVehicleTypeLinkKeyData.h"
#include "mdtClArticleLinkData.h"
#include "mdtSqlTableSelection.h"
#include <QSqlDatabase>
#include <QList>
#include <QVariant>
#include <QString>
#include <QModelIndex>

/*! \brief Helper class for link management
 */
class mdtClLink : public mdtTtBase
{
 public:

  /*! \brief Constructor
   */
  mdtClLink(QObject* parent, QSqlDatabase db);

  /*! \brief Constructor
   */
  mdtClLink(QSqlDatabase db);

  /*! \brief Destructor
   */
  ~mdtClLink();

  /*! \brief Add a link to database
   *
   * \param linkData Link data to add to Link_tbl
   */
  bool addLink(const mdtClLinkData & linkData);

  /*! \brief Add a link and assign given vehicle types to it
   *
   * \param linkData Link data to add to Link_tbl
   * \param vehicleTypeList List of vehicle types to add in assignations (VehicleType_Link_tbl)
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   */
  bool addLink(const mdtClLinkData & linkData, const QList<mdtClVehicleTypeStartEndKeyData> & vehicleTypeList, bool handleTransaction);

  /*! \brief Add a link to database
   *
   * \param linkData Link data to add to Link_tbl
   * \param modification Link versionning key data (LinkModification_tbl part)
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   * \deprecated
   */
  bool addLink(const mdtClLinkData & linkData, const mdtClLinkModificationKeyData & modification, bool handleTransaction);

  /*! \brief Add a link and assign given vehicle types to it
   *
   * \param linkData Link data to add to Link_tbl
   * \param modification Link versionning key data (LinkModification_tbl part)
   * \param vehicleTypeList List of vehicle types to add in assignations (VehicleType_Link_tbl)
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   * \deprecated
   */
  bool addLink(const mdtClLinkData & linkData, const mdtClLinkModificationKeyData & modification, const QList<mdtClVehicleTypeStartEndKeyData> & vehicleTypeList, bool handleTransaction);

  /*! \brief Add a link on base of given article link
   *
   * Will get start and end unit connections that are related to article link's start and end connection
   *  for given unit.
   *  Identification and Resistance are also copied from given article link.
   *  Note that only the Link_tbl part is created here (vehicle types assignations and versionning is not done).
   *
   * \pre articleLinkData must not be null
   * \pre unitId must not be null
   */
  bool addLink(const mdtClArticleLinkData & articleLinkData, const QVariant & unitId);

  /*! \brief Check if a link exists
   */
  bool linkExists(const mdtClLinkPkData & pk, bool & ok);

  /*! \brief Get link data
   *
   * \param pk PK of link for witch to get data
   * \return Data for requested link.
   *          A null data object is returned if requested link does not exist, or a error occured.
   *          Use ok parameter to diffrenciate both cases.
   */
  mdtClLinkData getLinkData(const mdtClLinkPkData & pk, bool & ok);

  /*! \brief Get link data for given article link and start unit and end unit
   * \todo + version + modification ...
   */
  mdtClLinkData getLinkData(const mdtClArticleLinkPkData articleLinkPk, const QVariant & startUnitId, const QVariant & endUnitId, bool & ok);

  /*! \brief Update a link
   *
   * \param linkPk Link to update
   * \param linkData New link data
   * \pre linkPk must not be null
   * \pre linkData must not be null
   */
  bool updateLink(const mdtClLinkPkData & linkPk, const mdtClLinkData & linkData);

  /*! \brief Update a link and its vehicle type assignations
   *
   * \param linkPk Link to update
   * \param linkData New link data
   * \param vehicleTypeList List of vehicle types assignations (VehicleType_Link_tbl)
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   * \pre linkPk must not be null
   * \pre linkData must not be null
   */
  bool updateLink(const mdtClLinkPkData & linkPk, const mdtClLinkData & linkData,
                  const QList<mdtClVehicleTypeStartEndKeyData> & vehicleTypeList, bool handleTransaction);

  /*! \brief Update a link, its modification and its vehicle type assignations
   *
   * \param linkPk Link to edit
   * \param linkData New link data
   * \param oldModification Modification to update (LinkModification_tbl part)
   * \param modification New modification (LinkModification_tbl part)
   * \param vehicleTypeList List of vehicle types assignations (VehicleType_Link_tbl)
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   * \pre linkPk must not be null
   * \pre linkData must not be null
   * 
   * \deprecated
   */
  bool updateLink(const mdtClLinkPkData & linkPk, const mdtClLinkData & linkData, 
                  const mdtClLinkModificationKeyData & oldModification, const mdtClLinkModificationKeyData & modification,  
                  const QList<mdtClVehicleTypeStartEndKeyData> & vehicleTypeList, bool handleTransaction);

  /*! \brief Remove a link
   *
   * Will also remove each vehicle types assigned to given link (VehicleType_Link_tbl)
   *
   * \param pk PK of link to remove
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   */
  bool removeLink(const mdtClLinkPkData & pk, bool handleTransaction);

  /*! \brief Remove each unit link that is contained in selection
   *
   * \sa removeLink()
   */
  bool removeLinks(const mdtSqlTableSelection & s);

  /*! \brief Add a link modification (LinkModification_tbl)
   */
  bool addModification(const mdtClLinkModificationKeyData & key);

  /*! \brief Update modification
   */
  bool updateModification(const mdtClLinkModificationKeyData & originalModification, const mdtClLinkModificationKeyData & newModification, bool handleTransaction);

  /*! \brief Remove link modification (LinkModification_tbl)
   */
  bool removeModification(const mdtClLinkModificationKeyData & key);

  /*! \brief Remove all modifications related to given link
   */
  bool removeModifications(const mdtClLinkPkData & linkPk);

 private:

  /*! \brief Fill given record with given data
   */
  void fillRecord(mdtSqlRecord & record, const mdtClLinkData & data);

  /*! \brief Fill given data with given record
   */
  void fillData(mdtClLinkData & data, const QSqlRecord & record);

  Q_DISABLE_COPY(mdtClLink);
};

#endif // MDT_CL_LINK_H
