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
#ifndef MDT_TEST_MAIN_H
#define MDT_TEST_MAIN_H

#include "SelectQueryTestBase.h"
#include "Mdt/QueryExpression/AbstractSelectQueryFactory.h"
// #include <memory>

class SelectQueryTest : public SelectQueryTestBase
{
 Q_OBJECT

  using SelectQueryFacotory = Mdt::QueryExpression::AbstractSelectQueryFactory;

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void execQueryTest();
  void execQueryMaxRowsTest();
  void execAndFetchAllTest();
  void fieldIndexTest();
  void fieldIndexEntityTest();
  void fieldIndexMultiEntityTest();
  void execQueryFilterTest();
  void useCaseFactoryTest();
};

#endif // #ifndef MDT_TEST_MAIN_H
