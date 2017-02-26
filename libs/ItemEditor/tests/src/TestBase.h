/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_ITEM_EDITOR_TEST_BASE_H
#define MDT_ITEM_EDITOR_TEST_BASE_H

#include "Mdt/Application.h"
#include "ItemViewTestEditTriggers.h"
#include <QObject>
#include <QtTest/QtTest>
#include <Qt>

class QAbstractItemModel;
class QAbstractProxyModel;
class QAbstractItemView;

class TestBase : public QObject
{
 Q_OBJECT

 protected:

  static void displayModel(QAbstractItemModel *model);
  static void displayModel(QAbstractItemModel & model);
  static void displayModels(QAbstractItemModel *sourceModel, QAbstractProxyModel *proxyModel);
  static void displayModels(QAbstractItemModel & sourceModel, QAbstractProxyModel & proxyModel);
  // Helper functions get/set data in a model
  static bool setModelData(QAbstractItemModel *model, int row, int column, const QVariant & value, Qt::ItemDataRole role = Qt::EditRole);
  static bool setModelData(QAbstractItemModel & model, int row, int column, const QVariant & value, Qt::ItemDataRole role = Qt::EditRole);
  static QVariant getModelData(const QAbstractItemModel *model, int row, int column, Qt::ItemDataRole role = Qt::DisplayRole);
  static QVariant getModelData(const QAbstractItemModel & model, int row, int column, Qt::ItemDataRole role = Qt::DisplayRole);
  // Helper functions get/set flags in a model
  static Qt::ItemFlags getModelFlags(const QAbstractItemModel *model, int row, int column);
  static Qt::ItemFlags getModelFlags(const QAbstractItemModel & model, int row, int column);
  // Helper function for editing in a QAbstractItemView
  static void beginEditing(QAbstractItemView & view, const QModelIndex & index, BeginEditTrigger trigger);
  static void editText(QAbstractItemView & view, const QModelIndex & editingIndex, const QString & str);
  static void endEditing(QAbstractItemView & view, const QModelIndex & editingIndex, EndEditTrigger trigger);
  static void edit(QAbstractItemView & view, const QModelIndex & index, const QString & str, BeginEditTrigger beginEditTrigger, EndEditTrigger endEditTrigger);

};

#define MDT_ITEM_EDITOR_TEST_MAIN_IMPL(TestClass, debugEnv) \
int main(int argc, char **argv) \
{ \
  Mdt::Application app(argc, argv); \
  TestClass test; \
  if(!app.init()){ \
    return 1; \
  } \
  if(debugEnv){ \
    app.debugEnvironnement(); \
  } \
  return QTest::qExec(&test, argc, argv); \
}

#define MDT_ITEM_EDITOR_TEST_MAIN(TestClass) MDT_ITEM_EDITOR_TEST_MAIN_IMPL(TestClass, false)
#define MDT_ITEM_EDITOR_TEST_MAIN_DEBUG_ENV(TestClass) MDT_ITEM_EDITOR_TEST_MAIN_IMPL(TestClass, true)

#endif // #ifndef MDT_ITEM_EDITOR_TEST_BASE_H
