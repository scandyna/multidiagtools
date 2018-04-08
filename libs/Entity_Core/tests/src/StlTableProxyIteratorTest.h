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
#ifndef STL_TABLE_PROXY_ITERATOR_TEST_H
#define STL_TABLE_PROXY_ITERATOR_TEST_H

#include "TestBase.h"

class StlTableProxyIteratorTest : public TestBase
{
 Q_OBJECT

 private slots:

  void iteratorRequirementTest();
  void inputIteratorRequirementTest();
  void memberAccessTest();
  void outputIteratorRequirementTest();
  void forwardIteratorRequirementTest();
  void bidirectionalIteratorRequirementTest();
  void randomAccessIteratorRequirementTest();
  void stdFindTest();
  void stdSortTest();
//   void simpleSetGetTest();
//   void stlAlgorithmTest();
//   void editTableModelTest();
};

#endif // #ifndef STL_TABLE_PROXY_ITERATOR_TEST_H
