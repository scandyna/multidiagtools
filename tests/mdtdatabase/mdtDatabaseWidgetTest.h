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
#ifndef MDT_DATABASE_WIDGET_TEST_H
#define MDT_DATABASE_WIDGET_TEST_H

#include "mdtTest.h"
#include "mdtSqlDatabaseManager.h"
#include <QFileInfo>
#include <QMessageBox>
#include <QSqlDatabase>

class QLineEdit;

/*
 * For sqlDataWidgetControllerTest we need a test widget
 */

class sqlDataWidgetControllerTestWidget : public QWidget
{
 Q_OBJECT

 public:

  sqlDataWidgetControllerTestWidget(QWidget *parent = 0);

  // Widgets acting on Client_tbl
  QLineEdit *fld_FirstName;
  QLineEdit *fld_Remarks;
  // Widgets acting on ClientDetail_tbl
  QLineEdit *fld_Detail;
  // Simulate buttons states
  bool toFirstEnabled;
  bool toLastEnabled;
  bool toNextEnabled;
  bool toPreviousEnabled;
  bool submitEnabled;
  bool revertEnabled;
  bool insertEnabled;
  bool removeEnabled;

 public slots:

  void setToFirstEnableState(bool enable);
  void setToLastEnableState(bool enable);
  void setToNextEnableState(bool enable);
  void setToPreviousEnableState(bool enable);
  void setInsertEnableState(bool enable);
  void setRemoveEnableState(bool enable);
  void setSubmitEnableState(bool enable);
  void setRevertEnableState(bool enable);
};

/*
 * Test itself
 */

class mdtDatabaseWidgetTest : public mdtTest
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void sqlRelationTest();
  void sqlFieldHandlerTest();
  void sqlDataWidgetControllerTest();
  void sqlDataWidgetController2tableTest();
  void sqlTableViewControllerTest();
  void sqlControllerParentChildTest();

  // Table selection tests
  void sqlTableSelectionItemTest();
  void sqlTableSelectionRowTest();
  void sqlTableSelectionTest();

  void sqlSelectionDialogTest();

  void sqlTableWidgetTest();

  void sqlFormTest();

 private:

  // Create test database schema - Will FAIL on problem
  void createDatabaseSchema();

  void populateTestDatabase();
  void clearTestDatabaseData();

  void populate1000Names();

  mdtSqlDatabaseManager pvDatabaseManager;
};

#endif // #ifndef MDT_DATABASE_WIDGET_TEST_H
