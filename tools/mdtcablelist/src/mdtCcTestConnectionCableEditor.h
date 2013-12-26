/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#ifndef MDT_CC_TEST_CONNECTION_CABLE_EDITOR_H
#define MDT_CC_TEST_CONNECTION_CABLE_EDITOR_H

#include "mdtError.h"
#include <QObject>
#include <QSqlDatabase>
#include <QVariant>
#include <QList>

/*! \brief Class for test connection cable edition
 *
 * Note: visualisation of cables is not provided here,
 *        use unit editor for this .
 */
class mdtCcTestConnectionCableEditor : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtCcTestConnectionCableEditor(QObject *parent, QSqlDatabase db);

  /*! \brief Create a test connection cable
   */
  void createCable();

  /*! \brief Connect a test cable
   */
  void connectTestCable();

  /*! \brief Disconnect a test cable
   */
  void disconnectTestCable();

 private:

  /*! \brief Select vehicle that contains unit to test
   */
  QVariant selectDutVehicleId();

  /*! \brief Select unit to test
   */
  QVariant selectDutUnitId(const QVariant & vehicleId);

  /*! \brief Select test node to use
   */
  QVariant selectTestNode();

  /*! \brief Select test cable to use
   */
  QVariant selectTestCable();

  /*! \brief Select start connector
   */
  QVariant selectStartConnectorId(const QVariant & dutUnitId);

  /*! \brief Select end connector(s) from given list
   */
  QList<QVariant> selectEndConnectorIdList(const QList<QVariant> & unitConnectorIdList);

  Q_DISABLE_COPY(mdtCcTestConnectionCableEditor);

  QSqlDatabase pvDatabase;
  mdtError pvLastError;
};

#endif // #ifndef MDT_CC_TEST_CONNECTION_CABLE_EDITOR_H
