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
#ifndef MDT_ITEM_EDITOR_SQL_TABLE_VIEW_CONTROLLER_TEST_H
#define MDT_ITEM_EDITOR_SQL_TABLE_VIEW_CONTROLLER_TEST_H

#include "ItemViewTestEditTriggers.h"
#include "Schema/ClientAddressSchema.h"
#include "Schema/ClientPopulation.h"
#include <QObject>
#include <QtTest/QtTest>
#include <QTemporaryFile>
#include <QSqlDatabase>

class QAbstractItemView;

class SqlTableViewControllerTest : public QObject
{
  Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void setModelThenViewTest();
  void setViewThenModelTest();
  void setDefaultModelThenViewTest();
  void setViewThenDefaultModelTest();
  void changeModelTest();
  void changeDefaultModelTest();
  void setTableTest();
  void selectTest();
  void currentRowChangeTest();
  void stateChangeSignalTest();

 private:

  // Helper function for editing in a QAbstractItemView
  void beginEditing(QAbstractItemView & view, int row, int column, BeginEditTrigger trigger);
  void editText(QAbstractItemView & view, int editingRow, int editingColumn, const QString & str);
  void endEditing(QAbstractItemView & view, int editingRow, int editingColumn, EndEditTrigger trigger);
  void edit(QAbstractItemView & view, int row, int column, const QString & str, BeginEditTrigger beginEditTrigger, EndEditTrigger endEditTrigger);

  void createSchema(const Schema::ClientAddressSchema & schema);
  bool deleteClientData();
  bool repopulateClientTable(const Schema::ClientPopulation & tp);

  QTemporaryFile mTempFile;  // We keep it as member, so file is destroyed automatically
  QSqlDatabase mDatabase;
};

#endif // #ifndef MDT_ITEM_EDITOR_SQL_TABLE_VIEW_CONTROLLER_TEST_H
