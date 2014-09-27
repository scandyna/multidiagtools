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
#ifndef MDT_TT_ABSTRACT_TESTER_H
#define MDT_TT_ABSTRACT_TESTER_H

#include "mdtError.h"
#include "mdtTtTest.h"
///#include "mdtTtTestData.h"
///#include "mdtTtTestModelData.h"
#include "mdtTtTestNodeManager.h"
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QString>
#include <QVariant>
#include <QVector>
#include <QObject>
#include <memory>

class QWidget;
class mdtSqlFormWidget;

/*! \brief Base to create a tester
 */
class mdtTtAbstractTester : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtAbstractTester(QSqlDatabase db, QObject *parent = 0);

  /*! \brief Do some initialization
   *
   * Will setup test data and table models for test item view and table.
   *
   * If testUiWidget is valid, 
   */
  virtual bool init();

  /*! \brief
   *
   * All childs contained in widget that have a name prefixed fld_ will be mapped
   *  to corresponding fields in Test_tbl.
   *
   * Given widget will also be used as parent for dialogs
   *  that are displayed (selection dialogs, message boxes).
   */
  void setTestUiWidget(QWidget *widget);

  /*! \brief Get database instance
   */
  QSqlDatabase database() { return pvDatabase; }

  /*! \brief Get table model to access data in TestItem_view
   */
  ///std::shared_ptr<QSqlTableModel> testItemViewTableModel() { return pvTest->testItemViewTableModel(); }

  /*! \brief Get test node manager object
   */
  std::shared_ptr<mdtTtTestNodeManager> testNodeManager() { return pvTestNodeManager; }

  /*! \brief Get test object
   */
  std::shared_ptr<mdtTtTest> test() { return pvTest; }

  /*! \brief Get test data
   *
   * Note: return cached data (will not get them from database).
   */
  ///inline mdtTtTestData testData() const { return pvTest->testData(); }

  /*! \brief Check if a test is empty
   *
   * When creating a new test, it must be directly stored in database.
   *  This is because test items must allways be created.
   *
   * A test is empty if:
   *  - In Test_tbl: only Id_PK, TestModel_Id_FK and Date are set
   *  - In each related item in Test_tbl: only Id_PK, Test_Id_FK and TestModelItem_Id_FK are set
   */
  bool testIsEmpty() const;

  /*! \brief Check if test is saved
   *
   * A test is saved if all cached data regarding Test_tbl and TestItem_tbl are stored in database.
   */
  bool testIsSaved() const;

 public slots:

  /*! \brief Create a new test
   *
   * This default implmementation will first check if current test is clean (saved, ...),
   *  and warn the user if not.
   *  Then (if test is clean, or user choosed to not keep it),
   *  user can select a test model to use.
   *  Finaly, test is created, based on selected test model.
   */
  virtual void createTest();

  /*! \brief Open a existing test
   *
   * This default implmementation will first check if current test is clean (saved, ...),
   *  and warn the user if not.
   *  Then (if test is clean, or user choosed to not keep it),
   *  user can select a test.
   *  Finaly, selected test is set as current test.
   */
  virtual void openTest();

  /*! \brief Save current test
   */
  virtual void saveTest();

 signals:

  /*! \brief Emited when test data has changed
   *
   * date comes from Test_view
   */
  void testDataChanged(const QSqlRecord & data);

 protected:

  /*! \brief Remove current test if it is empty
   *
   * see testIsEmpty()
   */
  bool removeTestIfEmpty();

  /*! \brief Contains last error that occured
   */
  mdtError pvLastError;

  /*! \brief Display last error to the user
   */
  void displayLastError();

 private:

  Q_DISABLE_COPY(mdtTtAbstractTester);

  QSqlDatabase pvDatabase;
  std::shared_ptr<mdtTtTestNodeManager> pvTestNodeManager;
  std::shared_ptr<mdtTtTest> pvTest;
  ///std::shared_ptr<mdtSqlFormWidget> pvTestFormWidget;
  QWidget *pvParentWidget;        // For dialogs
};

#endif // #ifndef MDT_TT_ABSTRACT_TESTER_H
