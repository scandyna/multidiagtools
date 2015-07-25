/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_WIDGETS_TEST_DATA_H
#define MDT_WIDGETS_TEST_DATA_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QString>
#include <QList>
#include <QFileInfo>

/*
 * Helper class to create temporary test data
 */
class mdtWidgetsTestData
{
 public:

  // Constructor
  mdtWidgetsTestData();
  // Destructor - will also remove created data
  ~mdtWidgetsTestData()
  {
    clear();
  }
  // Get database instance
  QSqlDatabase database()
  {
    return pvDatabase;
  }
  // Clear craeted data
  void clear();
  // Create Client_tbl
  bool createClientTable();
  // Populate Client_tbl
  bool populateClientTable();

 private:

  void debugSqlError(const QSqlError & error);
  void debugSqlError(const QSqlQuery & query);
  // Create test database schema - Will FAIL on problem
  void createDatabaseSchema();

  Q_DISABLE_COPY(mdtWidgetsTestData)

  QSqlDatabase pvDatabase;
  QFileInfo pvDbFileInfo;
};

#endif // #ifndef MDT_WIDGETS_TEST_DATA_H

