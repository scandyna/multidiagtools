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
#ifndef MDT_TT_BASIC_TESTER_H
#define MDT_TT_BASIC_TESTER_H

#include "mdtTtAbstractTester.h"
#include "mdtTtTest.h"
#include "mdtTtTestData.h"
#include "mdtTtTestNodeManager.h"
#include "mdtTtTestNodeSetupData.h"
#include "mdtError.h"
#include <QSqlDatabase>

class mdtSqlTableWidget;

/*! \brief Provide ability to run tests
 */
class mdtTtBasicTester : public mdtTtAbstractTester /// : public QMainWindow, Ui::mdtTtBasicTester
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtBasicTester(QSqlDatabase db, QObject *parent = 0);

  /*! \brief Setup
   */
  bool setup();

  /*! \brief Get last error
   */
  inline mdtError lastError() const { return pvLastError; }

 private slots:

  /*! \brief Run test
   */
  void runTest();

 private:

  Q_DISABLE_COPY(mdtTtBasicTester);

  /*! \brief Add instruments
   */
  bool addInstruments();

  /*! \brief Connect to instruments
   */
  bool connectToInstruments();

  /*! \brief Disconnect from instruments
   */
  bool disconnectFromInstruments();

  /*! \brief Setup instruments
   */
  bool setupInstruments(const QVariant & testItemId);

  /*! \brief Setup a test node
   */
  bool setupTestNode(const mdtTtTestNodeSetupData & setupData);

  /*! \brief Setup I/O coupler 0
   */
  bool setupIoCoupler0(const mdtTtTestNodeSetupData & setupData);
};

#endif  // #ifndef MDT_TT_BASIC_TESTER_H
