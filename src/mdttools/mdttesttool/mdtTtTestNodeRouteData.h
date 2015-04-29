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
#ifndef MDT_TT_TEST_NODE_ROUTE_DATA_H
#define MDT_TT_TEST_NODE_ROUTE_DATA_H

#include "mdtValue.h"
#include <QVariant>
#include <QString>
#include <QDateTime>
#include <vector>

/*! \brief Test node route relay data for test node route data
 */
struct mdtTtTestNodeRouteRelay
{
  /*! \brief Constructor
   */
  mdtTtTestNodeRouteRelay(const QVariant & id, const QVariant & schemaPosition)
   : id(id), schemaPosition(schemaPosition)
  {
  }

  /*! \brief Relay ID
   *
   * Related to field Unit_Id_FK_PK in TestNodeUnit_tbl
   */
  QVariant id;

  /*! \brief Schema position of relay
   *
   * Related to field SchemaPosition in Unit_tbl
   */
  QVariant schemaPosition;
};

/*! \brief Data for test node route
 */
class mdtTtTestNodeRouteData
{
 public:

  /*! \brief Set ID
   * \sa id()
   */
  void setId(const QVariant & id)
  {
    pvId = id;
  }

  /*! \brief Get ID
   *
   * Related to Id_PK in TestNodeRoute_tbl
   */
  inline QVariant id() const
  {
    return pvId;
  }

  /*! \brief Set test node ID
   * \sa testNodeId()
   */
  void setTestNodeId(const QVariant & id)
  {
    pvTestNodeId = id;
  }

  /*! \brief Get test node ID
   *
   * Related to TestNode_Id_FK in TestNodeRoute_tbl
   */
  inline QVariant testNodeId() const
  {
    return pvTestNodeId;
  }

  /*! \brief Set test node connection A ID
   * \sa connectionAId()
   */
  void setConnectionAId(const QVariant & id)
  {
    pvConnectionAId = id;
  }

  /*! \brief Get test node connection A ID
   *
   * Related to TestNodeUnitConnectionA_Id_FK in TestNodeRoute_tbl
   */
  inline QVariant connectionAId() const
  {
    return pvConnectionAId;
  }

  /*! \brief Set test node connection B ID
   * \sa connectionBId()
   */
  void setConnectionBId(const QVariant & id)
  {
    pvConnectionBId = id;
  }

  /*! \brief Get test node connection B ID
   *
   * Related to TestNodeUnitConnectionA_Id_FK in TestNodeRoute_tbl
   */
  inline QVariant connectionBId() const
  {
    return pvConnectionBId;
  }

  /*! \brief Set route resistance
   */
  void setResistance(const mdtValueDouble & r)
  {
    pvResistance = r;
  }

  /*! \brief Get route resistance
   */
  inline mdtValueDouble resistance() const
  {
    return pvResistance;
  }

  /*! \brief Set route resistance calibration date
   */
  void setCalibrationDate(const QDateTime & d)
  {
    pvCalibrationDate = d;
  }

  /*! \brief Get route resistance calibration date
   */
  inline QDateTime calibrationDate() const
  {
    return pvCalibrationDate;
  }

  /*! \brief Add a relay to enable
   */
  void addRelayToEnable(const QVariant & id, const QVariant & schemaPosition);

  /*! \brief Check how many relays are to enable
   */
  inline int relaysToEnableCount() const
  {
    return pvRelaysToEnable.size();
  }

  /*! \brief Access internal relays to enable vector
   *
   * Can be used, for example, to iterate all relays:
   * \code
   *  for(const auto & relay : data.relaysToEnableVector() ){
   *    doSomeStuff(relay);
   *  }
   * \endcode
   * Please take in accound that returned reference become invalid as soon as this mdtTtTestNodeRouteData object is destroyed.
   */
  const std::vector<mdtTtTestNodeRouteRelay> & relaysToEnableVector() const
  {
    return pvRelaysToEnable;
  }

  /*! \brief Clear route data
   */
  void clear();

 private:

  QVariant pvId;
  QVariant pvTestNodeId;
  QVariant pvConnectionAId;
  QVariant pvConnectionBId;
  mdtValueDouble pvResistance;
  QDateTime pvCalibrationDate;
  std::vector<mdtTtTestNodeRouteRelay> pvRelaysToEnable;
};

#endif // # ifndef MDT_TT_TEST_NODE_ROUTE_DATA_H
