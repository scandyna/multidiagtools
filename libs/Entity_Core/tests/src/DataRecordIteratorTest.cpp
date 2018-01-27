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
#include "DataRecordIteratorTest.h"
#include "Mdt/Entity/DataRecordIterator.h"
#include "Mdt/Entity/Def.h"

#include <boost/fusion/container.hpp>
// #include <boost/fusion/sequence.hpp>
// #include <boost/fusion/include/deref.hpp>
// #include <boost/fusion/include/begin.hpp>

#include <QDebug>

using namespace Mdt::Entity;

void DataRecordIteratorTest::initTestCase()
{
}

void DataRecordIteratorTest::cleanupTestCase()
{
}

// void DataRecordIteratorTest::sandbox()
// {
//   auto v = boost::fusion::make_vector(1,QString("A"),3.5);
// 
//   const auto first = boost::fusion::begin(v);
//   qDebug() << boost::fusion::deref( first );
//   qDebug() << *first;
//   qDebug() << *boost::fusion::next(first);
// }

/*
 * Common Boost Fusion sequence for the tests
 */

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

/*
 * Tests
 */

void DataRecordIteratorTest::constructTest()
{
  auto v0  = boost::fusion::make_vector();
  auto v1  = boost::fusion::make_vector(1);
  auto v3 = boost::fusion::make_vector(1,QString("A"),3.5);
  /*
   * Default constructed
   */
  ///DataRecordIterator<decltype(v)> it0;
  /*
   * Begin iterator
   */
  // 3 elements sequence
  DataRecordIterator<decltype(v3)> begin(v3);
  

  QFAIL("Not complete");
}

void DataRecordIteratorTest::copyTest()
{

  QFAIL("Not complete");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  DataRecordIteratorTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
