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
#ifndef MDT_CL_DIRECT_LINK_H
#define MDT_CL_DIRECT_LINK_H

#include "mdtTtBase.h"
#include "mdtSqlRecord.h"
#include <QSqlDatabase>
#include <QVariant>
#include <QList>
#include <QSqlRecord>

class mdtClPathGraph;

/*! \brief Create a temporary table with direct links informations
 *
 * Real links are stored in Link_tbl.
 *  LinkList_view is also avalable to get more informations.
 *
 * Link_tbl references all links that exists in practice,
 *  and also all connections.
 *  With mdtClPathGraph, it's possible get the path, including several linked links,
 *  starting from a defined unit connection.
 *  In some cases, it can be more interesting to have a synthetic view of paths
 *  starting from a element.
 *  Typical usage could be to see witch connection from a starting connector
 *  goes to witch connection of a other connector. This result will also
 *  contain several, so called, direct links.
 *
 */
class mdtClDirectLink : public mdtTtBase
{
 public:

  /*! \brief Constructor
   *
   * Will also create DirectLink_tbl temporary table.
   *
   * Note: if DirectLink_tbl allready exists, it will be overwritten.
   */
  mdtClDirectLink(QObject *parent, QSqlDatabase db);

  /*! \brief Destructor
   *
   * Will also remove DirectLink_tbl.
   */
  ~mdtClDirectLink();

  /*! \brief Get a list of ID unit connections that are part of given unit
   */
  QList<QVariant> getUnitConnectionIdListPartOfUnit(const QList<QVariant> & unitConnectionIdList, const QVariant & unitId, bool *ok);

  /*! \brief Get a list of ID unit connections that are part of given unit connector
   */
  QList<QVariant> getUnitConnectionIdListPartOfUnitConnector(const QList<QVariant> & unitConnectionIdList, const QVariant & unitConnectorId, bool *ok);

  /*! \brief Will get data from LinkList_view for given start and end connections and put them into DirectLink_tbl
   */
  bool addLink(const QVariant & unitConnectionStartId, const QVariant & unitConnectionEndId);

  /*! \brief Add direct links for connections of connectors S and E that are linked together
   */
  bool addLinksByUnitConnector(const QVariant unitConnectorIdS, const QVariant unitConnectorIdE, mdtClPathGraph *graph);

 private:

  /*! \brief Get start data from LinkList_view
   */
  QList<QSqlRecord> getStartData(const QVariant & unitConnectionId, bool *ok);

  /*! \brief Get end data from LinkList_view
   */
  QList<QSqlRecord> getEndData(const QVariant & unitConnectionId, bool *ok);

  /*! \brief Add src data list to dest data list
   *
   * Will only add data from src that not exist in dest.
   *  Internally, connectionExistInList() is used.
   */
  void addDataList(const QList<QSqlRecord> & src, QList<QSqlRecord> & dest);

  /*! \brief Get a record with left and right data
   */
  mdtSqlRecord getRecordForTable(const QSqlRecord & leftData, const QSqlRecord & rightData);

  /*! \brief Check if a given connection allready exist in a list
   *
   * Note: uses connectionsAreTheSame() for comaraison.
   */
  bool connectionExistInList(const QSqlRecord & connection, const QList<QSqlRecord> & list);

  /*! \brief Check if A and B are the same connection
   *
   * Note: A and B are the same connection if:
   *  - UnitConnection[Start|End]_Id_FK are the same in A and B , and
   *  - [Start|End]VehicleType_Id_PK are the same in A and B
   */
  bool connectionsAreTheSame(const QSqlRecord & A, const QSqlRecord & B);

  /*! \brief Create DirectLink_tbl
   *
   * Will have the same structure than LinkList_view.
   */
  bool createDirectLinkTable();

  /*! \brief Remove DirectLink_tbl
   */
  bool removeDirectLinkTable();

  Q_DISABLE_COPY(mdtClDirectLink);
};

#endif // #ifndef MDT_CL_DIRECT_LINK_H
