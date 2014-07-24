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
#include "mdtTtTestData.h"
#include "mdtTtTestModelData.h"
#include "mdtTtTestNodeManager.h"
#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QString>
#include <QVariant>

#include <memory>

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
   * Will setup test data and table model for test item table (see testItemTableModel()).
   */
  virtual bool init();

  /*! \brief Get database instance
   */
  QSqlDatabase database() { return pvDatabase; }

  /*! \brief Get test item table model
   */
  std::shared_ptr<QSqlTableModel> testItemTableModel() { return pvTestItemTableModel; }

  /*! \brief Get test node manager object
   */
  std::shared_ptr<mdtTtTestNodeManager> testNodeManager() { return pvTestNodeManager; }

  /*! \brief Set test data
   *
   * Will also emit testDataChanged()
   *
   * Note: data are not sent to database.
   */
  void setTestData(const mdtTtTestData & data);

  /*! \brief Set test data value
   *
   * Will also emit testDataChanged()
   *
   * Note: value is not sent to database.
   */
  void setTestDataValue(const QString & fieldName, const QVariant & value);

  /*! \brief Get test data
   *
   * Note: return cached data (will not get them from database).
   */
  mdtTtTestData testData() const { return pvTestData; }

 signals:

  /*! \brief Emited when test item table was set and select was successfull
   *
   * This can be used to setup a QTableView, a mdtSqlTableWidget, or what else
   *  (f.ex. resize or rename columns, etc...).
   */
  void testItemTableSet();

  /*! \brief Emited when test data has changed
   */
  void testDataChanged(const mdtTtTestData & data);

 protected:

  /*! \brief Contains last error that occured
   */
  mdtError pvLastError;

 private:

  Q_DISABLE_COPY(mdtTtAbstractTestWidget);

  mdtTtTestData pvTestData;
  QSqlDatabase pvDatabase;
  std::shared_ptr<QSqlTableModel> pvTestItemTableModel;
  std::shared_ptr<mdtTtTestNodeManager> pvTestNodeManager;
};

#endif // #ifndef MDT_TT_ABSTRACT_TEST_WIDGET_H
