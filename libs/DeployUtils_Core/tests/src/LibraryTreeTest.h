/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef LIBRARY_TREE_TEST_TEST_H
#define LIBRARY_TREE_TEST_TEST_H

#include "TestBase.h"

class LibraryTreeTest : public TestBase
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void graphSandbox();

  void treeNodeTest();
  void treeTest();

//   void fillAndSetDependenciesTest();
//   void fillAndSetDependenciesTest_data();
//   void fillAndSetDependenciesBenchmark();
//   void fillAndSetDependenciesBenchmark_data();

//  private:
// 
//   static Mdt::PlainText::StringRecordList toStringRecordList(const Mdt::DeployUtils::LibraryInfoList & libraryInfoList);
//   static Mdt::PlainText::StringRecordList createRecordList(const QStringList & libNameList, const QString & pathPrefix);
//   static Mdt::PlainText::StringRecordList generateLibraryRecordList(int n, const QString & pathPrefix);
};

#endif // #ifndef LIBRARY_TREE_TEST_TEST_H
