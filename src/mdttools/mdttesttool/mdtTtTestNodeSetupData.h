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
#ifndef MDT_TT_TEST_NODE_SETUP_DATA_H
#define MDT_TT_TEST_NODE_SETUP_DATA_H

#include "mdtTtTestNodeUnitSetupData.h"
#include <QString>
#include <QVariant>
#include <QList>

/*! \brief Storage class for test node setup data
 */
class mdtTtTestNodeSetupData
{
 public:

  /*! \brief Set test node identification
   */
  void setNodeIdentification(const QString & id);

  /*! \brief Get node identification
   */
  inline QString nodeIdentification() const { return pvNodeIdentification; }

  /*! \brief Set device identification
   */
  void setDeviceIdentification(const QVariant & id);

  /*! \brief Get device identification
   */
  inline QVariant devicedentification() const { return pvDeviceIdentification; }

  /*! \brief Clear test node unit setup data list
   */
  void clearUnitSetup();

  /*! \brief Add a test node unit setup
   */
  void addUnitSetup(const mdtTtTestNodeUnitSetupData & data);

  /*! \brief Get test node unit setup data list
   */
  inline const QList<mdtTtTestNodeUnitSetupData> & unitSetupList() const { return pvUnitSetupData; }

 private:

  QString pvNodeIdentification;
  QVariant pvDeviceIdentification;
  QList<mdtTtTestNodeUnitSetupData> pvUnitSetupData;
};

#endif // #ifndef MDT_TT_TEST_NODE_SETUP_DATA_H
