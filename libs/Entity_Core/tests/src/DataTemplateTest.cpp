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
#include "DataTemplateTest.h"
#include "Mdt/Entity/DataTemplate.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/Debug.h"


// #include <tuple>
// #include <utility>
// #include <type_traits>
// #include <stdexcept>
#include <initializer_list>
#include <type_traits>
#include <QString>
#include <QVariant>

#include <boost/fusion/container.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/fusion/include/size.hpp>
#include <boost/fusion/include/at.hpp>

#include <QDebug>

void DataTemplateTest::initTestCase()
{
}

void DataTemplateTest::cleanupTestCase()
{
}

/*
 * Tests
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

class ArticleData : public Mdt::Entity::DataTemplate<ArticleDataStruct, ArticleDef>
{
};

/** Try:
 *   A functor
 */

//template<typename T>
struct ValueAtColumn
{
  ValueAtColumn(int expectedIndex)
   : mExpectedIndex(expectedIndex)
  {
    /// Asserts
  }

  template<typename Key>
  void operator()(const Key &) const
  {
    if(mIndex == mExpectedIndex){
      // value = ...
    }
    ++mIndex;
  }

  QVariant value;

  mutable int mIndex = 0;
  const int mExpectedIndex;
};

/**
 * See:
 *  https://groups.google.com/forum/#!msg/comp.lang.c++.moderated/E6gRss0Sjx4/7o8jP19xHXcJ
 *  https://www.codeproject.com/Articles/23304/High-Performance-Heterogeneous-Container
 *
 * -> https://codereview.stackexchange.com/questions/51407/stdtuple-foreach-implementation
 * -> https://www.reddit.com/r/cpp/comments/6vyqra/variadic_switch_case/
 */

template<typename Sequence, int I>
void setValueImplItem(Sequence & seq, const QVariant & value)
{
  using ref_type = typename boost::fusion::result_of::at_c<Sequence, I>::type;
  using type = typename std::remove_reference<ref_type>::type;

  boost::fusion::at_c<I>(seq) = value.value<type>();
}

template<typename Sequence, int... Is>
void setValueImpl(Sequence & seq, int index, const QVariant & value, std::integer_sequence<int, Is...>)
{
  (void)std::initializer_list<int> {(index == Is ? ( setValueImplItem<Sequence, Is>(seq, value) ),0 : 0)...};
  //(void)std::initializer_list<int> {(index == Is ? (boost::fusion::at_c<Is>(seq) = value.value<boost::fusion::result_of::at_c<Sequence, Is>>()),0 : 0)...};
}

template<typename Sequence>
void setValue(Sequence & seq, int index, const QVariant & value)
{
  constexpr int size = boost::fusion::result_of::size<Sequence>::type::value;

  Q_ASSERT(index >= 0);
  Q_ASSERT(index < size);

  setValueImpl(seq, index, value, std::make_integer_sequence<int, size>{});
}

template<typename Sequence, int I>
QVariant getValueImplItem(const Sequence & seq)
{
  return boost::fusion::at_c<I>(seq);
}

template<typename Sequence, int... Is>
QVariant getValueImpl(const Sequence & seq, int index, std::integer_sequence<int, Is...>)
{
  QVariant value;
  (void)std::initializer_list<int> {(index == Is ? (value = getValueImplItem<Sequence, Is>(seq)),0 : 0)...};
  return value;
}

template<typename Sequence>
QVariant getValue(const Sequence & seq, int index)
{
  constexpr int size = boost::fusion::result_of::size<Sequence>::type::value;

  Q_ASSERT(index >= 0);
  Q_ASSERT(index < size);

  return getValueImpl(seq, index, std::make_integer_sequence<int, size>{});
}

void DataTemplateTest::sandbox()
{

  auto v = boost::fusion::make_vector(1,QString("A"),3);
  for(int i = 0; i < 3; ++i){
    qDebug() << "v[" << i << "]: " << getValue(v, i);
  }
  boost::fusion::at_c<0>(v) = 5;
  setValue(v, 0, 5);
  setValue(v, 1, "B");
  setValue(v, 2, 6);
  for(int i = 0; i < 3; ++i){
    qDebug() << "v[" << i << "]: " << getValue(v, i);
  }

  ArticleData art1;
  setValue(art1.data(), 0, 25);
  qDebug() << getValue(art1.constData(), 0);
  setValue(art1.data(), 1, "Des 25");
  qDebug() << getValue(art1.constData(), 1);

}

/**
 * For tests, reate structs:
 *
 * struct NotCopiable
 * {
 * };
 *
 * struct NotMovable
 * {
 * };
 */

void DataTemplateTest::dataUsageTest()
{
  ArticleData art1;
  const auto def = art1.def();
  QCOMPARE(def.entityName(), QString("Article"));
  QCOMPARE(def.id.fieldName(), QString("id"));
  QCOMPARE(def.description.fieldName(), QString("description"));
  art1.data().id = 1;
  art1.data().description = "Article 1";
  QCOMPARE(art1.constData().id, 1);
  QCOMPARE(art1.constData().description, QString("Article 1"));
  QCOMPARE(art1.columnCount(), 2);
  art1.setValue(0, 25);
  art1.setValue(1, "Article 25");
  QCOMPARE(art1.constData().id, 25);
  QCOMPARE(art1.constData().description, QString("Article 25"));
  QCOMPARE(art1.value(0), QVariant(25));
  QCOMPARE(art1.value(1), QVariant("Article 25"));
  Mdt::Entity::printEntityDataToConsole(art1);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  DataTemplateTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
