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
#ifndef MDT_ITEM_MODEL_RELATION_FILTER_PROXY_MODEL_TEST_H
#define MDT_ITEM_MODEL_RELATION_FILTER_PROXY_MODEL_TEST_H

#include "TestBase.h"

class RelationFilterProxyModelTest : public TestBase
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void setModelTest();

  void setFilterTest();

  void parentModelMatchRowTest();
  void filterTest();
  void filterMultiColumnTest();
  void filterBenchmark();
  void filterBenchmark_data();

  void filterGetCurrentSourceRowListTest();

  /*! \todo Implement those tests
   *
   * To know more precisely how the proxy model
   *  should react on various events,
   *  it seems better to implement ItemView_Widgets
   *  library first, so we have some setup widgets.
   *  Also, it could also be good to create a example
   *  (in ItemView_Widgets) to experiment the result,
   *  and then add tests based on real bad feeling
   *  when using it.
   */
  void filterRoleTest();
  void setterEventTest();

  void dynamicFilterTest();
  void dynamicFilterInsertTest();
  void dynamicFilterInsertMultiColumnTest();
  void parentModelKeyChangeTest();
  void parentModelKeyMultiColumnKeyChangeTest();
  void parentModelKeyChangeRolesTest();

  void qtModelTest();
};

#endif // #ifndef MDT_ITEM_MODEL_RELATION_FILTER_PROXY_MODEL_TEST_H
