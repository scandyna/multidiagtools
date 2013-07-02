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
#ifndef MDT_DATABASE_TEST_H
#define MDT_DATABASE_TEST_H

#include "mdtTest.h"
#include <QFileInfo>
#include <QMessageBox>

class mdtDatabaseTest : public mdtTest
{
 Q_OBJECT

 private slots:

  // Create database and tables
  void initTestCase();

  void relationsTest();

  void sqlFieldHandlerTest();

  void sqlFormWidgetTest();

  void sqlTableWidgetTest();

  void sortFilterProxyModelTest();

  // Will Accept/reject the application's modal widget if not null
  void acceptModalDialog();
  void rejectModalDialog();
  // Simulate the user clicking on a button in a QMessageBox
  void clickMessageBoxButton(QMessageBox::StandardButton button);
  void clickMessageBoxButtonYes();
  void clickMessageBoxButtonNo();
  void clickMessageBoxButtonCancel();
  void clickMessageBoxButtonOk();

  /*
   * Some old tests ...
   */
  void mdtSqlTableModelTest();
  ///void mdtSqlDataWidgetMapperTest();

  void cleanupTestCase();

 private:

  QFileInfo pvDbFileInfo;
};

#endif // #ifndef MDT_DATABASE_TEST_H
