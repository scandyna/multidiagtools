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
#ifndef MDT_TT_TEST_MODEL_ITEM_ROUTE_DATA_H
#define MDT_TT_TEST_MODEL_ITEM_ROUTE_DATA_H

#include "mdtTtTestNodeUnitSetupData.h"
#include "mdtValue.h"
#include <QVariant>
#include <QList>
#include <QSqlRecord>

/*! \brief Container for test model item route data
 *
 * Contains data from TestModelItemRoute_tbl.
 *
 * A route is way that links a measure connection to a test connection
 *  in a coupler (a test node).
 */
class mdtTtTestModelItemRouteData
{
 public:

  /*! \brief Construct a empty route data
   */
  mdtTtTestModelItemRouteData();

  /*! \brief Construct data from a sql record
   *
   * Given record must contain all fields from TestModelItemRoute_tbl.
   *  If record includes TestConnection_Id_FK, testConnectionId will also be set.
   *  If record includes DutConnection_Id_FK, dutConnectionId will also be set.
   *
   * \pre record must contain following fields: Id_PK, TestModelItem_Id_FK, TestLink_Id_FK and MeasureTestNodeUnitConnection_Id_FK.
   */
  mdtTtTestModelItemRouteData(const QSqlRecord & record);

  /*! \brief Clear
   */
  void clearData();

  /*! \brief Set ID
   *
   * Will also update field TestModelItemRoute_Id_FK in each test node unit setup
   */
  void setId(const QVariant & id);

  /*! \brief Get ID
   */
  inline QVariant id() const { return pvId; }

  /*! \brief Set test model item ID
   *
   * Will also update field TestModelItem_Id_FK in each test node unit setup
   */
  void setTestModelItemId(const QVariant & id);

  /*! \brief Get test model item ID
   */
  inline QVariant testModelItemId() const { return pvTestModelItemId; }

  /*! \brief Set test link ID
   */
  void setTestLinkId(const QVariant & id);

  /*! \brief Get test link ID
   */
  inline QVariant testLinkId() const { return pvTestLinkId; }

  /*! \brief Set DUT connection ID
   */
  void setDutConnectionId(const QVariant & id);

  /*! \brief Get DUT connection ID
   */
  inline QVariant dutConnectionId() const { return pvDutConnectionId; }

  /*! \brief Set test connection ID
   */
  void setTestConnectionId(const QVariant & id);

  /*! \brief Get test connection ID
   */
  inline QVariant testConnectionId() const { return pvTestConnectionId; }

  /*! \brief Set measure connection ID
   */
  void setMeasureConnectionId(const QVariant & id);

  /*! \brief Get measure connection ID
   */
  inline QVariant measureConnectionId() const { return pvMeasureConnectionId; }

  /*! \brief Add a test node unit setup data
   *
   * Will update TestModelItem_Id_FK in data with
   *  internal testModelItemId
   *  and TestModelItemRoute_Id_FK with internal id.
   *
   * \pre Fields TestModelItem_Id_FK and TestModelItemRoute_Id_FK must exist in data.
   */
  void addSetupData(mdtTtTestNodeUnitSetupData data);

  /*! \brief Get test node unit setup data list
   */
  inline const QList<mdtTtTestNodeUnitSetupData> & setupDataList() const { return pvSetupDataList; }

  /*! \brief Get route resistance
   *
   * Route resistance is due to coupler (test node)
   *  relays. This values are set during a test node calibration.
   *
   * Note: this value is only available if added setup data were constructed from TestNodeUnitSetup_view.
   */
  mdtValue routeResistance() const;

  /*! \brief Get route resistance for bus coupling relays part
   *
   * This function is similar to routeResistance(),
   *  but returns only sum for bus coupling relays.
   */
  mdtValue busCouplingRelaysResistance() const;

  /*! \brief Get route resistance for channel relays part
   *
   * This function is similar to routeResistance(),
   *  but returns only sum for channel relays.
   */
  mdtValue channelRelaysResistance() const;

  /*! \brief Get ID list of bus coupling relays that are part of route
   */
  QList<QVariant> busCouplingRelaysIdList() const;

  /*! \brief Get ID list of channel relays that are part of route
   */
  QList<QVariant> channelRelaysIdList() const;

 private:

  QVariant pvId;
  QVariant pvTestModelItemId;
  QVariant pvTestLinkId;
  QVariant pvDutConnectionId;
  QVariant pvTestConnectionId;
  QVariant pvMeasureConnectionId;
  QList<mdtTtTestNodeUnitSetupData> pvSetupDataList;
};

#endif // #ifndef MDT_TT_TEST_MODEL_ITEM_ROUTE_DATA_H
