/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_SQL_COPIER_TEST_H
#define MDT_SQL_COPIER_TEST_H

#include "mdtTest.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QTemporaryFile>

class QModelIndex;
class QAbstractItemView;

class mdtSqlCopierTest : public mdtTest
{
 Q_OBJECT

 private slots:

  void sandbox();
  void bechSandBox();

  void initTestCase();
  void cleanupTestCase();

  void sqlFieldSetupDataTest();

  void fieldMappingTest();
  void fixedValueTest();

  void expressionMatchItemTest();
  void expressionMatchItemModelTest();

  void relatedTableInsertExpressionTest();
  void relatedTableInsertMatchItemModelTest();
  void relatedTableInsertExpressionDialogTest();

  void uniqueInsertExpressionTest();
  void uniqueInsertExpressionDialogTest();

  void tableMappingItemTest();
  void tableMappingItemRelatedTableExpTest();

  void tableMappingEditHelperContainsDFIndexesTest();
  void tableMappingEditHelperContainsDFIndexesTest_data();
  void tableMappingEditHelperRemoveItemsTest();
  void tableMappingEditHelperRemoveItemsTest_data();
  void tableMappingEditHelperItemDfiToAddTest();
  void tableMappingEditHelperItemDfiToAddTest_data();
  void tableMappingEditHelperInsertTest();
  void tableMappingEditHelperInsertTest_data();

  void databaseCopierTableMappingUpdateItemsTest();
  void databaseCopierTableMappingTableFetchTest();
  void databaseCopierTableMappingFieldMapTest();
  void databaseCopierTableMappingFixedValueTest();
  void databaseCopierTableMappingRelatedTableExpressionTest();

  void sqlDatabaseCopierTableMappingStateTest();
  
  void sqlDatabaseCopierTableMappingSqliteTest();
  
  void sqlDatabaseCopierTableMappingModelTest();
  void sqlDatabaseCopierTableMappingDialogTest();

  void sqlCopierDataMappingTest();

  void sqlCsvStringImportTableMappingTest();
  void sqlCsvFileImportTableMappingTest();
  void sqlCsvStringImportTableMappingModelTest();
  void sqlCsvFileImportTableMappingModelTest();

  void copyHelperTest();

  void sqlDatabaseCopierMappingTest();
  void sqlDatabaseCopierMappingModelTest();
  void sqlDatabaseCopierDialogTest();
  void sqlDatabaseCopierThreadTest();

 private:

  // Helper function to begin and end editing in a QAbstractItemView
  void beginEditing(QAbstractItemView & view, const QModelIndex & index);
  void endEditing(QAbstractItemView & view, const QModelIndex & editingIndex);

  /*
   * Create the test database
   */
  void createTestDatabase();
  // Populate database with some common data
  void populateTestDatabase();
  // Clear test database data
  void clearTestDatabaseData();

  QTemporaryFile pvTempFile;  // We keep it as member, so file is destroyed automatically
  QSqlDatabase pvDatabase;
};

#endif // #ifndef MDT_SQL_COPIER_TEST_H
