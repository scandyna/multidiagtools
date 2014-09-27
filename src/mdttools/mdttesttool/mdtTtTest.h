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
#ifndef MDT_TT_TEST_H
#define MDT_TT_TEST_H

#include "mdtTtTestData.h"

#include "mdtTtBase.h"
#include "mdtTtTestModelData.h"
#include "mdtValue.h"
#include "mdtTtTestNodeUnitSetupData.h"
#include "mdtTtTestItemNodeSetupData.h"
#include "mdtSqlDataWidgetController.h"
#include "mdtSqlTableViewController.h"
#include <QSqlDatabase>
#include <QModelIndex>
#include <QVariant>
#include <QString>
#include <QStringList>
#include <QList>
#include <QSqlRecord>
#include <memory>
#include <limits>

class QSqlTableModel;
class mdtSqlRelation;

/*! \brief Helper class to manipulate test data
 *
 * 
 */
class mdtTtTest : public mdtTtBase
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTest(QObject *parent, QSqlDatabase db);

  /*! \brief Do some initialization
   *
   * Will setup test data and table models for test item view and table.
   */
  bool init();

  /*! \brief Access controller that acts on Test_view
   */
  std::shared_ptr<mdtSqlDataWidgetController> testViewController()
  {
    return pvTestViewController;
  }

  /*! \brief Access controller that acts on TestItem_view
   *
   * \pre init() must be done before calling this method.
   */
  std::shared_ptr<mdtSqlTableViewController> testItemViewController()
  {
    Q_ASSERT(pvTestItemViewController);
    return pvTestItemViewController;
  }

  /*! \brief Get table model to access data in Test_tbl
   */
  ///std::shared_ptr<QSqlTableModel> testTableModel() { return pvTestTableModel; }

  /*! \brief Get table model to access data in TestItem_view
   */
  ///std::shared_ptr<QSqlTableModel> testItemViewTableModel() { return pvTestItemViewTableModel; }

  /*! \brief Set test data
   *
   * Will also emit testDataChanged()
   *
   * Note: data are not sent to database.
   */
  ///void setTestData(const mdtTtTestData & data);

  /*! \brief Set test data value
   *
   * Will also emit testDataChanged()
   *
   * Note: value is not sent to database.
   */
  ///void setTestDataValue(const QString & fieldName, const QVariant & value);

  /*! \brief Get test data
   *
   * Returns data from Test_view
   */
  ///QSqlRecord testData() const;

  /*! \brief Get test data value for given fieldName
   *
   * Note: return cached data (will not get them from database).
   */
  ///QVariant testDataValue(const QString & fieldName) const;

  /*! \brief Set current test to given testId
   *
   * Note: will directly load data for given testId.
   *  To check test state before, use:
   *   - testIsSaved()
   *   - 
   */
  void setCurrentTest(const QVariant & testId);

  /*! \brief Create a new test based on given testModelId
   *
   * Note: will directly create the test.
   *  To check test state before, use:
   *   - testIsSaved()
   *   - testIsEmpty()
   */
  bool createTest(const QVariant & testModelId);

  /*! \brief Save current test
   */
  bool saveCurrentTest();

  /*! \brief Remove current test
   *
   * Note: will directly create the test.
   *  To check test state before, use:
   *   - testIsSaved()
   *   - testIsEmpty()
   */
  bool removeCurrentTest();

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
   *  If no test was set (Test_tbl.Id_PK is NULL), test is considered saved.
   *  Note: each call of this method will check all test items.
   */
  bool testIsSaved();

  /*! \brief Set test item data
   */

  /*! \brief Set current test item data
   *
   * Note: will not save to database. Use saveCurrentTest() for that.
   */
  void setCurrentTestItemData(const QString& fieldName, const QVariant& data);

  /*! \brief Get current test item data
   *
   * Note: will return cached value.
   */
  QVariant currentTestItemData(const QString & fieldName) const;

  /*! \brief Set measured value to current test item
   *
   * Will first check if given value has -OL or +OL flag,
   *  and set resulting value to, respectiveley, -inf or +inf if true.
   *
   * The resulting value is then compared to expected one,
   *  and result is alos determined.
   *
   * Finally, all values are stored to current test item,
   *  but only cached (see setCurrentTestItemData()).
   *
   * Note: instrumentRangeMin and instrumentRangeMax are only
   *  informative, they are stored as they are given.
   *  For proper limit evaluation, only mdtValue's flags are used.
   */
  void setMeasuredValue(const mdtValue & value, const QVariant & instrumentRangeMin, const QVariant & instrumentRangeMax);

  /*! \brief Check if x is in range
   *
   * Return true if x is in [min;max]
   */
  static bool isInRange(double x, double min, double max = std::numeric_limits<double>::max());

  /*! \brief Check if x is in OK range
   *
   * Return true if x is in ]limitMin;limitMax[
   */
  static bool isInOkRange(double x, double limitMin, double limitMax);

  /*! \brief Check if x is in limit range
   *
   * Return true if x is in ]failMin;limitMin] or in [limitMax;failMax[
   */
  static bool isInLimitRange(double x, double limitMin, double limitMax, double failMin, double failMax);

  /*! \brief Check if more test item is available
   */
  bool hasMoreTestItem() const;

  /*! \brief Reset the test item cursor
   */
  void resetTestItemCursor();

  /*! \brief Go to next test item
   *
   * \return The test item ID
   */
  QVariant nextTestItem();

  /*! \brief Get setup data for given test item ID
   */
  mdtTtTestItemNodeSetupData getSetupData(const QVariant & testItemId, bool & ok);

  /*! \brief Convert a mdtValue to a double
   *
   * If value has -OL flag set, -1e300 is returned.
   * If value has +OL flag set, 1e300 is returned.
   */
  ///static QVariant valueToDouble(const mdtValue & value);

  /*! \brief Convert a double to a value
   *
   * If dblVal is <= -1e300, -OL flag will be set.
   * If dblVal is >= 1e300, +OL flag will be set.
   */
  ///static mdtValue doubleToValue(const QVariant & dblVal);

  /*! \brief Set measured value
   *
   * Note: value will not be stored to database immediatly,
   *  call submitTestItemSqlModelData() to do it .
   */
  ///bool setMeasuredValue(const QVariant & testItemId, const mdtValue & value);

 signals:

  /*! \brief Emited when test data has changed
   *
   * data comes from Test_view
   */
  ///void testDataChanged(const QSqlRecord & data);

 private:

  /*! \brief Set current index row for Test_view table model
   */
  ///void setCurrentTestIndexRow(int row);

  /*! \brief Add test items for given test model ID to current test (see pvTestData)
   */
  bool addTestItems(const QVariant & testId, const QVariant & testModelId);

  /*! \brief Remove test items from current test (see pvTestData)
   */
  bool removeTestItems(const QVariant & testId);

  /*! \brief Get row in pvTestItemTableModel for given testItemId
   */
  ///int getTestItemTableModelIndexRow(const QVariant & testItemId);

  Q_DISABLE_COPY(mdtTtTest);

  QSqlDatabase pvDatabase;
  /**
  std::shared_ptr<QSqlTableModel> pvTestViewTableModel;         // Access data in Test_view
  std::shared_ptr<QSqlTableModel> pvTestTableModel;             // Access data in Test_tbl
  std::shared_ptr<mdtSqlRelation> pvTestTableRelation;          // Test_view <-> Test_tbl relation
  */
  std::shared_ptr<mdtSqlDataWidgetController> pvTestViewController; // Access data in Test_view
  ///int pvCurrentTestRow;
  // Test item data models
  /**
  std::shared_ptr<QSqlTableModel> pvTestItemViewTableModel;     // Access data in TestItem_view
  std::shared_ptr<QSqlTableModel> pvTestItemTableModel;         // Access data in TestItem_tbl
  std::shared_ptr<mdtSqlRelation> pvTestItemViewRelation;       // Test_view <-> TestItem_view relation
  std::shared_ptr<mdtSqlRelation> pvTestItemTableRelation;      // Test_view <-> TestItem_tbl relation
  */
  // Test item data
  std::shared_ptr<mdtSqlTableViewController> pvTestItemViewController;  // Access data in TestItem_view
  int pvCurrentTestItemRow;
};

#endif // #ifndef MDT_TT_TEST_H
