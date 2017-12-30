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


#include <tuple>
#include <utility>
#include <type_traits>
#include <stdexcept>
#include <initializer_list>
#include <QString>
#include <QVariant>

#include <boost/fusion/container.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/fusion/include/size.hpp>

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

template<
  typename Tuple,
  typename Indices = std::make_index_sequence<std::tuple_size<Tuple>::value>
>
struct RuntimeGetFunctionTable;

template<typename Tuple, size_t ... Indices>
struct RuntimeGetFunctionTable< Tuple, std::index_sequence<Indices...> >
{
  using return_type = typename std::tuple_element<0, Tuple>::type &;
  using getFunctionPointer = return_type (*)(Tuple&) noexcept;
  static constexpr getFunctionPointer table[std::tuple_size<Tuple>::value] = {
    &std::get<Indices>...
  };
};

template<typename Tuple, size_t ... Indices>
constexpr typename
  RuntimeGetFunctionTable< Tuple, std::index_sequence<Indices...> >::getFunctionPointer
  RuntimeGetFunctionTable< Tuple, std::index_sequence<Indices...> >::table[std::tuple_size<Tuple>::value];

template<typename Tuple>
constexpr
typename std::tuple_element<0, typename std::remove_reference<Tuple>::type>::type &
runtime_get(Tuple && t, size_t index)
{
  using tuple_type = typename std::remove_reference<Tuple>::type;

  Q_ASSERT(index >= 0);
  Q_ASSERT(index < std::tuple_size<tuple_type>::value);

  return RuntimeGetFunctionTable<tuple_type>::table[index](t);
}

/**
 * See:
 *  https://groups.google.com/forum/#!msg/comp.lang.c++.moderated/E6gRss0Sjx4/7o8jP19xHXcJ
 *  https://www.codeproject.com/Articles/23304/High-Performance-Heterogeneous-Container
 *
 * -> https://codereview.stackexchange.com/questions/51407/stdtuple-foreach-implementation
 * -> https://www.reddit.com/r/cpp/comments/6vyqra/variadic_switch_case/
 */

void printSomeInt(int i)
{
  qDebug() << "i: " << i;
}

template<typename Sequence, int... Is>
QVariant fusion_runtime_get_impl(const Sequence & seq, int index, std::integer_sequence<int, Is...>)
{
  QVariant value;
//   if(index == I){
//     return I;
//   }
  
  std::initializer_list<int> {(index == Is ? (value = boost::fusion::at_c<Is>(seq)),0 : 0)...};
//   using swallow = int[];
//   swallow{ (printSomeInt(Is), int{})... };
  
//   (qDebug() << "index: " << index << " , Is size: " << sizeof...(Is) << ", Is: " << Is)...;
  return value;
//   return fusion_runtime_get_impl<Is...>(index);
}

template<typename Sequence>
QVariant fusion_runtime_get(const Sequence & seq, int index)
{
  constexpr int size = boost::fusion::size(seq);

  Q_ASSERT(index >= 0);
  Q_ASSERT(index < size);

  return fusion_runtime_get_impl(seq, index, std::make_integer_sequence<int, size>{});
//   switch(index){
//     case 0:
//       return boost::fusion::at_c<0>(seq);
//     case 1:
//       return boost::fusion::at_c<1>(seq);
//     case 2:
//       return boost::fusion::at_c<2>(seq);
//     case 3:
//       return boost::fusion::at_c<3>(seq);
//   }
// 
//   return QVariant();
}

void DataTemplateTest::sandbox()
{
  auto t = std::make_tuple(1.0, 2.0, 3.0);

  qDebug() << "t[0]: " << std::get<0>(t);
  qDebug() << "t[1]: " << std::get<1>(t);
  qDebug() << "t[2]: " << std::get<2>(t);

  for(int i = 0; i < 3; ++i){
    qDebug() << "t[" << i << "]: " << runtime_get(t, i);
  }

  auto v = boost::fusion::make_vector(1,"A",3);
  for(int i = 0; i < 3; ++i){
    qDebug() << "v[" << i << "]: " << fusion_runtime_get(v, i);
  }
}

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
