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
#ifndef MDT_CL_LINK_H
#define MDT_CL_LINK_H

#include "mdtTtBase.h"
#include "mdtClLinkData.h"
#include <QSqlDatabase>
#include <QList>
#include <QVariant>
#include <QModelIndex>

/*! \brief Helper class for link management
 */
class mdtClLink : public mdtTtBase
{
 public:

  /*! \brief Constructor
   */
  mdtClLink(QObject* parent, QSqlDatabase db);

  /*! \brief Destructor
   */
  ~mdtClLink();

  /*! \brief Build vehicle type link data based on given start and end vehicle ID lists
   *
   * The list is built regarding distinct cases:
   *  - If both start and end vehicle types lists contains exactly the same count of items,
   *     each start ID is put together with each end ID , in the order of both lists.
   *  - If one vehicle types list has only 1 item, and the other has many items,
   *     the ID of the 1 item list is copied to the other list
   *     (case of 1 vehicle type linked to many vehicle types) .
   *  - All other cases are wrong.
   */
  bool buildVehicleTypeLinkDataList(mdtClLinkData & linkData, const QList<QVariant> & vtStartIdList, const QList<QVariant> & vtEndIdList);

  /*! \brief Add a link
   *
   * Will also add required vehicle type links stored in linkData.
   *  To generate vehicle type links, use buildVehicleTypeLinkDataList().
   */
  bool addLink(const mdtClLinkData & linkData, bool handleTransaction = true);

  /*! \brief Add a list of links
   *
   * \sa addLink().
   */
  bool addLinks(const QList<mdtClLinkData> & linkDataList, bool handleTransaction = true);

  /*! \brief Remove a unit link
   *
   * Will also remove all vehicle type related links
   */
  bool removeLink(const QVariant &unitConnectionStartId, const QVariant &unitConnectionEndId);

  /*! \brief Remove each unit link that is contained in selection
   */
  bool removeLinks(const QList<QModelIndexList> &indexListOfSelectedRowsByRows);

 private:

  /*! \brief Check vehicle type start and end ID lists
   */
  bool checkVehicleTypeStartEndIdLists(const QList<QVariant> & vtStartIdList, const QList<QVariant> & vtEndIdList);

  /*! \brief Add link to vehicle type table
   */
  bool addLinkToVehicleType(const mdtClVehicleTypeLinkData & data);

  /*! \brief Add a list of links to vehicle type table
   */
  bool addLinkToVehicleTypeList(const QList<mdtClVehicleTypeLinkData> & dataList);

  Q_DISABLE_COPY(mdtClLink);
};

#endif // MDT_CL_LINK_H
