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

///#include "ui_mdtTtBasicTester.h"
///#include <QMainWindow>

#include "mdtTtAbstractTestWidget.h"
#include "mdtTtTest.h"
#include "mdtTtTestData.h"
#include "mdtTtTestNodeManager.h"
#include "mdtTtTestNodeSetupData.h"
#include "mdtError.h"
#include <QSqlDatabase>

class mdtSqlTableWidget;
///class QSqlTableModel;

/*! \brief Provide ability to run tests
 */
class mdtTtBasicTester : public mdtTtAbstractTestWidget /// : public QMainWindow, Ui::mdtTtBasicTester
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtBasicTester(QSqlDatabase db, QWidget *parent = 0);

  /*! \brief Setup
   */
  bool setup();

  /*! \brief Get last error
   */
  inline mdtError lastError() const { return pvLastError; }

 private slots:

  /*! \brief Create a test
   */
  ///void createTest();

  /*! \brief Set test model
   */
  void setTestModel();

  /*! \brief Save test
   */
  ///void saveTest();

  /*! \brief Run test
   */
  void runTest();

 private:

  Q_DISABLE_COPY(mdtTtBasicTester);

  /*! \brief Create new test
   */
  ///bool createNewTest();

  /*! \brief Save data part of Test_tbl
   */
  bool saveTestData();

  /*! \brief Display data part of Test_tbl
   */
  ///void displayTestData(bool getFromDatabase);

  /*! \brief Display data part of Test item
   *
   * Will reload test items from db that are part
   *  of current test and display them.
   */
  ///void displayTestItemData();

  /*! \brief Set data for a test item
   *
   * Will set data for given field
   *  and test item ID.
   *  Data is not saved to database.
   */
  ///void setTestItemData(const QVariant & testItemId, const QString & fieldName, const QVariant & data);

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
  
  /*! \brief Setup tables
   */
  bool setupTables();
};

#endif  // #ifndef MDT_TT_BASIC_TESTER_H
