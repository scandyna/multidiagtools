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
#include "FieldAttributesItemDelegateTest.h"
#include "Mdt/Entity/FieldAttributesItemDelegate.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/RowResizableTableModel.h"
#include "Mdt/Entity/DataTemplate.h"
#include "Mdt/Entity/DataList.h"
#include <QScopedPointer>

#include <QTableView>

using namespace Mdt::Entity;

/*
 * Entity
 */

struct ArticleDataStruct
{
  qlonglong id;
  QString description;
  double qty;
};

MDT_ENTITY_DEF(
  (ArticleDataStruct),
  Article,
  (id, FieldFlag::IsRequired, FieldFlag::IsUnique),
  (description, FieldMaxLength(50)),
  (qty)
)

class ArticleData : public DataTemplate<ArticleEntity>
{
};

using ArticleDataList = DataList<ArticleData>;

/*
 * Tests
 */

void FieldAttributesItemDelegateTest::sandbox()
{
  RowResizableTableModel<ArticleDataList> model;
  FieldAttributesItemDelegate<ArticleEntity> delegate;
  QTableView view;
  
  view.setItemDelegate(&delegate);
  view.setModel(&model);
  model.appendRow();
  view.resize(500, 500);
  view.show();

  while(view.isVisible()){
    QTest::qWait(500);
  }
}

void FieldAttributesItemDelegateTest::fieldAttributesAtColumnTest()
{
  FieldAttributesItemDelegate<ArticleEntity> delegate;

  QVERIFY(delegate.fieldAttributesAtColumn(0).maxLength() < 1);
  QCOMPARE(delegate.fieldAttributesAtColumn(1).maxLength(), 50);
  QVERIFY(delegate.fieldAttributesAtColumn(2).maxLength() < 1);
}

void FieldAttributesItemDelegateTest::createEditorTest()
{
  RowResizableTableModel<ArticleDataList> model;
  FieldAttributesItemDelegate<ArticleEntity> delegate;
  QModelIndex index;

  model.appendRow();
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  QScopedPointer<QWidget> descriptionEditor( delegate.createEditor(nullptr, QStyleOptionViewItem(), index) );
  QCOMPARE(descriptionEditor->property("maxLength"), QVariant(50));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  FieldAttributesItemDelegateTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
