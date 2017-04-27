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
#include "DataWidgetMapperItemDelegateTest.h"
#include "Mdt/ItemEditor/DataWidgetMapperItemDelegate.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include <QLineEdit>
#include <QModelIndex>
#include <QVariant>
#include <QMetaType>

using namespace Mdt::ItemEditor;
using namespace Mdt::ItemModel;

void DataWidgetMapperItemDelegateTest::initTestCase()
{
}

void DataWidgetMapperItemDelegateTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void DataWidgetMapperItemDelegateTest::setModelDataLineEditTest()
{
  QFETCH(QString, inputText);
  QFETCH(QVariant, expectedData);
  QFETCH(bool, expectedIsNull);
  DataWidgetMapperItemDelegate delegate;
  QLineEdit editor;
  QModelIndex index;
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(1, 1);
  /*
   * Check
   */
  editor.setText(inputText);
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  delegate.setModelData(&editor, &model, index);
  if(expectedIsNull){
    QVERIFY(getModelData(model, 0, 0).isNull());
  }else{
    QCOMPARE(getModelData(model, 0, 0), expectedData);
  }
}

void DataWidgetMapperItemDelegateTest::setModelDataLineEditTest_data()
{
  QTest::addColumn<QString>("inputText");
  QTest::addColumn<QVariant>("expectedData");
  QTest::addColumn<bool>("expectedIsNull");

  QTest::newRow("Empty") << "" << QVariant() << true;
  QTest::newRow("A") << "A" << QVariant("A") << false;
}

void DataWidgetMapperItemDelegateTest::setEditorDataLineEditTest()
{
  QFETCH(QString, inputText);
  QFETCH(QVariant, modelData);
  QFETCH(QString, expectedText);
  DataWidgetMapperItemDelegate delegate;
  QLineEdit editor;
  QModelIndex index;
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(1, 1);
  /*
   * Check
   */
  editor.setText(inputText);
  QVERIFY(setModelData(model, 0, 0, modelData));
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  delegate.setEditorData(&editor, index);
  QCOMPARE(editor.text(), expectedText);
}

void DataWidgetMapperItemDelegateTest::setEditorDataLineEditTest_data()
{
  QTest::addColumn<QString>("inputText");
  QTest::addColumn<QVariant>("modelData");
  QTest::addColumn<QString>("expectedText");

  QTest::newRow("A,null") << "A" << QVariant() << "";
  QTest::newRow("A,B") << "A" << QVariant("B") << "B";
  QTest::newRow("1,0") << "1" << QVariant(0) << "0";
  QTest::newRow("1,(int,null)") << "1" << QVariant(QVariant::Int) << "";
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  DataWidgetMapperItemDelegateTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
