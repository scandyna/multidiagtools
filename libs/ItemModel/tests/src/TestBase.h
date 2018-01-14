/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_ITEM_MODEL_TEST_BASE_H
#define MDT_ITEM_MODEL_TEST_BASE_H

#include "Mdt/CoreApplication.h"
#include "Mdt/Application.h"
#include "Mdt/TestLib/ItemModel.h"
#include <QObject>
#include <QtTest/QtTest>
#include <Qt>

class QAbstractItemModel;
class QAbstractProxyModel;

// Helper functions get/set data in a model
using Mdt::TestLib::getModelData;
using Mdt::TestLib::setModelData;

// Helpers to resize model
bool prependRowToModel(QAbstractItemModel *model);
bool prependRowToModel(QAbstractItemModel & model);
bool appendRowToModel(QAbstractItemModel *model);
bool appendRowToModel(QAbstractItemModel & model);

// Helper functions get/set flags in a model
Qt::ItemFlags getModelFlags(const QAbstractItemModel *model, int row, int column);
Qt::ItemFlags getModelFlags(const QAbstractItemModel & model, int row, int column);

class TestBase : public QObject
{
 Q_OBJECT

 public:

  static void displayModel(QAbstractItemModel *model);
  static void displayModel(QAbstractItemModel & model);
  static void displayModels(QAbstractItemModel *sourceModel, QAbstractProxyModel *proxyModel);
  static void displayModels(QAbstractItemModel & sourceModel, QAbstractProxyModel & proxyModel);

};

#define MDT_ITEM_MODEL_TEST_MAIN_IMPL(TestClass, debugEnv) \
int main(int argc, char **argv) \
{ \
  Mdt::Application app(argc, argv); \
  TestClass test; \
  if(debugEnv){ \
    app.debugEnvironment(); \
  } \
  return QTest::qExec(&test, argc, argv); \
}

#define MDT_ITEM_MODEL_TEST_MAIN(TestClass) MDT_ITEM_MODEL_TEST_MAIN_IMPL(TestClass, false)
#define MDT_ITEM_MODEL_TEST_MAIN_DEBUG_ENV(TestClass) MDT_ITEM_MODEL_TEST_MAIN_IMPL(TestClass, true)

#endif // #ifndef MDT_ITEM_MODEL_TEST_BASE_H
