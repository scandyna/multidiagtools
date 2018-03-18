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
#ifndef DATA_LIST_ITEM_MODEL_TEST_H
#define DATA_LIST_ITEM_MODEL_TEST_H

#include "TestBase.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/DataList.h"
#include "Mdt/Entity/RowResizableTableModel.h"

struct ArticleDataStruct
{
  qlonglong id;
  QString description;
};

MDT_ENTITY_DEF(
  (ArticleDataStruct),
  Article,
  (id, FieldFlag::IsRequired, FieldFlag::IsUnique),
  (description, FieldMaxLength(250))
)

class ArticleData : public Mdt::Entity::DataTemplate<ArticleEntity>
{
 public:

  void setId(int id)
  {
    dataStruct().id = id;
  }

  int id() const
  {
    return constDataStruct().id;
  }

};

using ArticleDataList = Mdt::Entity::DataList<ArticleData>;

class ArticleTableModel : public Mdt::Entity::RowResizableTableModel<ArticleDataList>
{
 Q_OBJECT

 public:

  using ParentClass = Mdt::Entity::RowResizableTableModel<ArticleDataList>;
  using ParentClass::ParentClass;
};

/*
 * Test class
 */

class DataListItemModelTest : public TestBase
{
 Q_OBJECT

 private slots:

  void rowResziableTableModelInsertTest();
  void rowResziableTableModelRemoveTest();
};

#endif // #ifndef DATA_LIST_ITEM_MODEL_TEST_H
