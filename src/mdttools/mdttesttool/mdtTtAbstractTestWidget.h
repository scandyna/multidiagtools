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
#ifndef MDT_TT_ABSTRACT_TEST_WIDGET_H
#define MDT_TT_ABSTRACT_TEST_WIDGET_H

#include "mdtError.h"
#include "mdtTtTest.h"
#include "mdtTtTestData.h"
#include "mdtTtTestModelData.h"
#include "mdtTtTestNodeManager.h"
#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QString>
#include <QVariant>
#include <QVector>

#include <memory>

class mdtSqlFormWidget;

/*! \brief Base to create a test widget
 */
class mdtTtAbstractTestWidget : public QWidget
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtAbstractTestWidget(QSqlDatabase db, QWidget *parent = 0);

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
   */
  void setTestUiWidget(QWidget *widget);

  /*! \brief Get database instance
   */
  QSqlDatabase database() { return pvDatabase; }

  /*! \brief Get table model to access data in TestItem_view
   */
  std::shared_ptr<QSqlTableModel> testItemViewTableModel() { return pvTest->testItemViewTableModel(); }

  /*! \brief Get test node manager object
   */
  std::shared_ptr<mdtTtTestNodeManager> testNodeManager() { return pvTestNodeManager; }

  /*! \brief Get test object
   */
  std::shared_ptr<mdtTtTest> test() { return pvTest; }

  /*! \brief Set test data
   *
   * Will also emit testDataChanged()
   *
   * Note: data are not sent to database.
   */
  ///inline void setTestData(const mdtTtTestData & data) { pvTest->setTestData(data); }
  void setTestData(const mdtTtTestData & data) {}

  /*! \brief Set test data value
   *
   * Will also emit testDataChanged()
   *
   * Note: value is not sent to database.
   */
  ///inline void setTestDataValue(const QString & fieldName, const QVariant & value) { pvTest->setTestDataValue(fieldName, value); }
  void setTestDataValue(const QString & fieldName, const QVariant & value) {}

  /*! \brief Get test data
   *
   * Note: return cached data (will not get them from database).
   */
  inline mdtTtTestData testData() const { return pvTest->testData(); }

  /*! \brief Check if test is saved
   *
   * A test is saved if all cached data regarding Test_tbl and TestItem_tbl are stored in database.
   */
  inline bool testIsSaved() { return pvTest->testIsSaved(); }

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

  /*! \brief Set DutSerialNumber
   */
  void setDutSerialNumber(const QString & value);

 signals:

  /*! \brief Emited when test item table was set and select was successfull
   *
   * This can be used to setup a QTableView, a mdtSqlTableWidget, or what else
   *  (f.ex. resize or rename columns, etc...).
   */
  void testItemTableSet();

  /*! \brief Emited when test data has changed
   *
   * date comes from Test_view
   */
  void testDataChanged(const QSqlRecord & data);

 protected:

  /*! \brief Contains last error that occured
   */
  mdtError pvLastError;

  /*! \brief Display last error to the user
   */
  void displayLastError();

 private:

  Q_DISABLE_COPY(mdtTtAbstractTestWidget);

  QSqlDatabase pvDatabase;
  std::shared_ptr<mdtTtTestNodeManager> pvTestNodeManager;
  std::shared_ptr<mdtTtTest> pvTest;
  mdtSqlFormWidget *pvTestFormWidget;
};

#endif // #ifndef MDT_TT_ABSTRACT_TEST_WIDGET_H
