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
#ifndef MDT_TT_BASIC_TEST_NODE_CALIBRATION_TOOL_H
#define MDT_TT_BASIC_TEST_NODE_CALIBRATION_TOOL_H

#include "mdtTtAbstractTestNodeCalibrationTool.h"
#include "mdtClPathGraph.h"
#include <QSqlDatabase>
#include <QList>
#include <QString>
#include <QStringList>

class mdtTtTestModelItemRouteData;

/*! \biref Test node calibration tool example
 */
class mdtTtBasicTestNodeCalibrationTool : public mdtTtAbstractTestNodeCalibrationTool
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtBasicTestNodeCalibrationTool(QSqlDatabase db, QObject *parent = 0);

  /*! \brief Do setup
   *
   * Will setup database controllers and map widgets contained in testNodeFormWidget.
   *
   * Note: connect signals and slots before calling this method,
   *  else UI will be incoherent at initial state.
   */
  bool setup(QWidget *testNodeFormWidget);

 public slots:

  /*! \brief Run calibration
   */
  void runCalibration();

  /*! \brief Save calibration
   */
  void saveCalibration();

 private:

  /*! \brief Measure bridge
   *
   * \return Resistance of bridge, or a value < 0 on error.
   */
  double measureBridge();

  /*! \brief Check K3, K4, K5, K6
   */
  bool checkSenseRelays();

  /*! \brief Calibrate K3, K4, K5, K6
   */
  bool calibrateSenseRelays();

  /*! \brief Check K1 and K2
   */
  bool checkIsoRelays();

  /*! \brief Calibrate K1 and K2
   */
  bool calibrateIsoRelays();

  /*! \brief Check K7 and K8
   */
  bool checkForceRelays();

  /*! \brief Calibrate K7 and K8
   */
  bool calibrateForceRelays();

  /*! \brief calibrate channel relays
   */
  bool calibrateChannelRelays();

  /*! \brief Set I/O coupler routes
   */
  bool setupIoCouplerRoutes(const QList<mdtTtTestModelItemRouteData> & routeDataList);

  /*! \brief Add instruments
   */
  void addInstruments();

  /*! \brief Connect to instruments
   */
  bool connectToInstruments();

  /*! \brief Disconnect from instruments
   */
  void disconnectFromInstruments();

  /*! \brief Get resistance of links that are travsersed for given connections
   *
   * \param schemaPositionA Schema position that contains connection A
   * \param connectorA Name of connector that contains connection A
   * \param contactA Name of contact that contains connection A
   * \param schemaPositionB Schema position that contains connection B
   * \param connectorB Name of connector that contains connection B
   * \param contactB Name of contact that contains connection B
   * \param relaysToEnable List of coupling and channel relays that are turned on (give schema position for each)
   * \param ok Will be set false if a error occured, true else
   */
  double getLinkResistanceForRoute(const QString & schemaPositionA, const QString & connectorA, const QString & contactA,
                                   const QString & schemaPositionB, const QString & connectorB, const QString & contactB,
                                   const QStringList & relaysToEnable, bool & ok);

  /*! \brief Frit line
   *
   * 
   */
  bool fritLine();

  Q_DISABLE_COPY(mdtTtBasicTestNodeCalibrationTool);

  mdtClPathGraph pvGraph;
};

#endif // #ifndef MDT_TT_BASIC_TEST_NODE_CALIBRATION_TOOL_H
