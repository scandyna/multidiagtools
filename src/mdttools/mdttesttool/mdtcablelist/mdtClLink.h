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
#include "mdtClVehicleTypeLinkKeyData.h"
#include "mdtClArticleLinkUnitConnectionKeyData.h"
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

  /*! \brief Add list of links based on given article links
   *
   * \param keyList A link will be added in Link_tbl based on article link for each key.
   * \param versionPk Created links will be created for given version
   * \param modificationPk Created links will be created for given modification
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   */
  bool addLinkList(const QList<mdtClArticleLinkUnitConnectionKeyData> & keyList,
                   const mdtClLinkVersionPkData & versionPk, const mdtClModificationPkData & modificationPk,
                   bool handleTransaction);

  /*! \brief Add list of links based on given article links
   *
   * \param keyList A link will be added in Link_tbl based on article link for each key.
   * \param versionPk Created links will be created for given version
   * \param modificationPk Created links will be created for given modification
   * \param vehicleTypeList List of vehicle types to add in assignations (VehicleType_Link_tbl) for each created link
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   */
  bool addLinkList(const QList<mdtClArticleLinkUnitConnectionKeyData> & keyList,
                   const mdtClLinkVersionPkData & versionPk, const mdtClModificationPkData & modificationPk,
                   const QList<mdtClVehicleTypeStartEndKeyData> & vehicleTypeList,  bool handleTransaction);

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
