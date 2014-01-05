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

#include "mdtClBase.h"
#include "mdtValue.h"
#include <QSqlDatabase>
#include <QModelIndex>
#include <QVariant>
#include <QString>
#include <QStringList>
#include <QList>

class QSqlTableModel;

/*! \brief Helper class to manipulate test data
 *
 * 
 */
class mdtTtTest : public mdtClBase
{
 public:

  /*! \brief Constructor
   */
  mdtTtTest(QSqlDatabase db);

  /*! \brief Set test item model
   *
   * If a model that contains test item data
   *  is allready used, for example in the GUI,
   *  it's possible to re-use it here.
   *  When do so, test results (measured values, states, ...)
   *  will allways be reflected to GUI.
   *
   * Other goal to use a model is to get/set data
   *  in a efficient way, without instanciating QSqlQuery
   *  objects for each, possibly single data, query.
   *
   * The given model will not be deleted by this class .
   */
  bool setTestItemSqlModel(QSqlTableModel *model);

  /*! \brief Get a list of test item IDs for given test ID
   *
   * Items are sorted by sequence number, ascending
   */
  QList<QVariant> getTestItemIdListForTestId(const QVariant & testId);

  /*! \brief Get test node units that must be enabled
   */
  ///QStringList getNodeUnitsToEnable(const QVariant & testItemId);

  /*! \brief Set test model
   *
   * Will also add a test item for each test item .
   */
  bool setTestModel(const QVariant & testResultId, const QVariant & testId);

  /*! \brief Add a test item
   */
  bool addItem(const QVariant & testResultId, const QVariant & testItemId);

  /*! \brief Add a test items based on given test ID
   */
  bool addItemsByTestId(const QVariant & testResultId, const QVariant & testId);

  /*! \brief Edit a test item
   */
  bool editItem(const QVariant & testItemId, const QString & fieldName, const QVariant & data);

  /*! \brief Set measured value
   *
   * Note: value will not be stored to database immediatly,
   *  call submitTestItemSqlModelData() to do it .
   */
  bool setMeasuredValue(const QVariant & testItemId, const mdtValue & value);

  /*! \brief Submit test items SQL model data to database
   *
   * If a error occurs, it will be available with lastError() .
   */
  bool submitTestItemSqlModelData();

 ///private slots:

  /*! \brief Set sql model pointers tu Null whenn they are destroyed
   */
  ///void onSqlModelDestroyed(QObject *obj);

 private:

  /*! \brief Check if test item sql model was set
   *
   * Will store a error if not ok .
   */
  bool testItemSqlModelOk();

  /*! \brief Get index for given testItemId and column
   *
   * Will store a error if a invalid index is returned
   */
  QModelIndex indexOfTestItem(const QVariant & testItemId, int column);

  /*! \brief Helper method to store data into test item SQL model
   *
   * If a error occurs, it will be stored .
   */
  bool setTestItemSqlModelData(const QModelIndex & index, const QVariant & data);

  /*! \brief Helper method to get data from test item SQL model
   *
   * If a error occurs, it will be stored .
   */
  QVariant getTestItemSqlModelData(const QModelIndex & index);

  /*! \brief Helper method to get data from test item SQL model
   *
   * \overload getTestItemSqlModelData(const QModelIndex &)
   */
  QVariant getTestItemSqlModelData(int row, int column);

  Q_DISABLE_COPY(mdtTtTest);

  // Test item data models
  QSqlTableModel *pvTestItemSqlModel;
  int pvColIdxOfTestItemId;
  int pvColIdxOfExpectedValue;
  int pvColIdxOfMeasuredValue;
  int pvColIdxOfResult;
};

#endif // #ifndef MDT_TT_TEST_H
