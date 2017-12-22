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
#include "ReflectionTest.h"
#include <QString>

#include <QDebug>

#include <QObject>
#include <QMetaProperty>
#include <QMetaMethod>
#include <QVariant>
#include <array>
#include <tuple>
#include <utility>

#include <boost/fusion/sequence.hpp>
#include <boost/fusion/include/sequence.hpp>
// #include <boost/fusion/tuple.hpp>
// #include <boost/fusion/include/tuple.hpp>
#include <boost/fusion/adapted.hpp>
#include <boost/fusion/include/adapted.hpp>
#include <boost/fusion/algorithm.hpp>
#include <boost/fusion/include/algorithm.hpp>

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/fusion/adapted/struct/define_struct.hpp>
#include <boost/fusion/include/define_struct.hpp>

#include <boost/fusion/adapted/struct/adapt_assoc_struct.hpp>
#include <boost/fusion/include/adapt_assoc_struct.hpp>

#include <boost/fusion/adapted/struct/adapt_assoc_struct_named.hpp>
#include <boost/fusion/include/adapt_assoc_struct_named.hpp>


// #include <boost/mpl/vector.hpp>
// #include <boost/mpl/transform.hpp>
// #include <boost/mpl/push_front.hpp>
// #include <boost/fusion/sequence.hpp>
// #include <boost/fusion/iterator.hpp>
// #include <boost/fusion/iterator/next.hpp>
// #include <boost/fusion/iterator/equal_to.hpp>
// #include <boost/fusion/iterator/key_of.hpp>
// #include <boost/fusion/iterator/value_of.hpp>
//
// #include <boost/fusion/container/map.hpp>

void ReflectionTest::initTestCase()
{
}

void ReflectionTest::cleanupTestCase()
{
}

struct PrintFV
{
  template<typename T>
  void operator()(const T & x) const
  {
    qDebug() << "x: " << x;
  }
};

// struct PrintPair
// {
//   template<typename T>
//   void operator()(const T & p) const
//   {
//     qDebug() << p.first.fieldName() << ": " << p.second;
//   }
// };

// struct StaticField
// {
//   static const QString fieldName()
//   {
//     return "Some field name";
//   }
// };

struct PrintField
{
  template<typename T>
  void operator()(const T & f) const
  {
    qDebug() << f.fieldName() << ": " << f.value;
  }
};

struct DataStruct
{
  struct idField
  {
    int value;
    static const QString fieldName()
    {
      return "id";
    }
  };

  struct firstNameField
  {
    QString value;
    static const QString fieldName()
    {
      return "firstName";
    }
  };

  idField m_idField;
  firstNameField m_firstNameField;
};
BOOST_FUSION_ADAPT_STRUCT(
  DataStruct,
  m_idField,
  m_firstNameField
)
// BOOST_FUSION_DEFINE_STRUCT(
//   (),
//   DataStruct,
//   (int, id)
//   (QString, firstName)
// )

// ========== to fusion map ==================

// template<typename Vector, typename First, typename Last, typename doContinue>
// struct ToFusionMapIterator;
// 
// template<typename Vector, typename First, typename Last>
// struct ToFusionMapIterator<Vector, First, Last, boost::mpl::false_>
// {
//   using Next = typename boost::fusion::result_of::next<First>::type;
//   using type = typename boost::mpl::push_front<
//     typename ToFusionMapIterator<
//       Vector,
//       Next,
//       Last,
//       typename boost::fusion::result_of::equal_to<Next, Last>::type
//     >::type,
//     boost::fusion::pair<
//       typename boost::fusion::result_of::key_of<First>::type,
//       typename boost::fusion::result_of::value_of_data<First>::type
//     >
//   >::type;
// };
// 
// template<typename Vector, typename First, typename Last>
// struct ToFusionMapIterator<Vector, First, Last, boost::mpl::true_>
// {
//   using type = Vector;
// };
// 
// template<typename FusionAssociativeSequence>
// struct AsFusionMap
// {
//   using First = typename boost::fusion::result_of::begin<FusionAssociativeSequence>::type;
//   using Last = typename boost::fusion::result_of::end<FusionAssociativeSequence>::type;
//   using type = typename boost::fusion::result_of::as_map<
//     typename ToFusionMapIterator<
//       boost::mpl::vector<>,
//       First,
//       Last,
//       typename boost::fusion::result_of::equal_to<First, Last>::type
//     >::type
//   >::type;
// };

// ========== to fusion map ==================

// struct PrintPair
// {
//   template<typename Pair>
//   void operator()(const Pair & p) const
//   {
//     qDebug() << p.second;
//   }
// };

struct DataStruct2
{
  int id;
  QString firstName;
};

struct DataStruct2Def
{
  struct idField
  {
    static const QString fieldName()
    {
      return "id";
    }

    /// For attributes: also a static function that construct it
    /*
     * static const attributes()
     * {
     *   return Attributes(...);
     * }
     */
  };

  struct firstNameField
  {
    static const QString fieldName()
    {
      return "firstName";
    }
  };

  idField id;
  firstNameField firstName;
};
BOOST_FUSION_ADAPT_STRUCT(
  DataStruct2Def,
  id,
  firstName
)

BOOST_FUSION_ADAPT_ASSOC_STRUCT(
  DataStruct2,
  (id, DataStruct2Def::idField)
  (firstName, DataStruct2Def::firstNameField)
)

struct PrintDef
{
  PrintDef(const DataStruct2 & data)
   : mData(data)
  {
  }

  template<typename Key>
  void operator()(const Key &) const
  {
    qDebug() << "Iterating on def ...";
    // Use boost::fusion::at_key<>() to get the value
    qDebug() << Key::fieldName() << ": " <<  boost::fusion::at_key<Key>(mData);
  }

  const DataStruct2 & mData;
};

class Data
{
 public:

  void setId(int id)
  {
    mData.id = id;
  }

  void setFirstName(const QString & fn)
  {
    mData.firstName = fn;
  }

  const DataStruct2 & data() const
  {
    return mData;
  }

  static const DataStruct2Def def()
  {
    return DataStruct2Def{};
  }

 private:

  DataStruct2 mData;
};

class DataStorage
{
 public:

  void printData(const Data & data){
    boost::fusion::for_each(data.def(), PrintDef(data.data()));
  }
};

void ReflectionTest::sandboxFusion()
{
  boost::fusion::vector<int, char, QString> v(1, 'c', "str");
  qDebug() << "v[0]: " << boost::fusion::at_c<0>(v);
  qDebug() << "v[1]: " << boost::fusion::at_c<1>(v);
  qDebug() << "v[2]: " << boost::fusion::at_c<2>(v);

  boost::fusion::for_each(v, PrintFV());

  DataStruct ds;
  ds.m_idField.value = 12;
  ds.m_firstNameField.value = "Name 12";
  boost::fusion::for_each(ds, PrintField());

  DataStruct2 ds2;
  ds2.id = 22;
  ds2.firstName = "Name 22";
  qDebug() << "2 id: " << boost::fusion::at_key<DataStruct2Def::idField>(ds2);
  qDebug() << "2 first name: " << boost::fusion::at_key<DataStruct2Def::firstNameField>(ds2);

  boost::fusion::for_each(DataStruct2Def(), PrintDef(ds2));

  Data data;
  data.setId(5);
  data.setFirstName("FN 5");
  DataStorage storage;
  storage.printData(data);
//   DataStruct

//   const auto map = AsFusionMap<DataStruct2>();
//   boost::fusion::for_each(map, PrintPair());

//   DataStruct2 ds2;
//   boost::fusion::for_each(ds2, PrintPair());
//   std::pair<StaticField, int> field;
//   boost::fusion::vector< std::pair<StaticField, int> > v2;
//
//   boost::fusion::for_each(v2, PrintPair());
//   boost::fusion::for_each(field, PrintFV());
//   std::tuple<int, QString> t(10, "ten");
//   boost::fusion::for_each(t, PrintFV());

//   DataStruct ds;
//   ds.id = 25;
//   ds.firstName = "Name 25";
//   qDebug() << "id: " << ds.id << ", fn: " << ds.firstName << " (empty: " << ds.firstName.isEmpty() << ")";
//   boost::fusion::for_each(ds, PrintFV());
}

// class Field
// {
//  public:
// 
//   explicit Field(const QString & name)
//    : mFieldName(name)
//   {
//   }
// 
//   QString fieldName() const
//   {
//     return mFieldName;
//   }
// 
//  private:
// 
// //   constexpr int index;
//   QString mFieldName;
// };

// struct ClientDataStruct
// {
// //   int id;
//   QString firstName;
// 
//   int & id()
//   {
//     return std::get<0>(mTuple);
//   }
// 
//   const int & id() const
//   {
//     return std::get<0>(mTuple);
//   }
// 
//   /// static function returning
// 
// //   template<int index>
// //   auto valueAt()
// //   {
// //     return std::get<index>(mTuple);
// //   }
// 
//   // Use BOOST_xxx_ADAPT_STRUCT() or.. BOOST_xx_CREATE_STRUCT()
//   std::tuple<int, QString> mTuple;
// };

// struct ClientStaticDefinition
// {
//   static const QString entityName()
//   {
//     return "Client";
//   }
// 
//   static const Field id()
//   {
//     return Field("id");
//   }
// 
//   static const Field firstName()
//   {
//     return Field("firstName");
//   }
// 
// //   static std::array<Field, 2> fieldList()
// //   {
// //     return {{Field<0>("id"), Field<1>("firstName")}};
// //   }
// };

// class ClientData
// {
//  public:
// 
//   void setId(int id)
//   {
//     mData.id() = id;
//   }
// 
//   int id() const
//   {
//     return mData.id();
//   }
// 
//   static ClientStaticDefinition def()
//   {
//     return ClientStaticDefinition{};
//   }
// 
//  private:
// 
//   ClientDataStruct mData;
// };


/// Iterate a tuple: http://blogorama.nerdworks.in/iteratingoverastdtuple/
///                  https://codereview.stackexchange.com/questions/134814/for-each-for-tuple-likes
///                  Boost.Fusion provides for_each() on std::tuple ! SA range_c
/// Note: base for a runtime index ? (if needed..)
/// Note: for conditionnal update: flag in field

template<typename T>
void debugEntityDef(const T & entity)
{
  qDebug() << "Entity: " << entity.def().entityName();
//   for(const auto & field : entity.def().fieldList()){
//     qDebug() << " field: " << field.fieldName();
//   }
}

// auto reflectMembers(const ClientDataStruct & s)
// {
//   struct MetaData
//   {
//   };
// }


void ReflectionTest::sandbox()
{
//   qDebug() << "sizeof(ClientStaticDefinition): " << sizeof(ClientStaticDefinition);
// 
//   ClientData client;
//   debugEntityDef(client);
//   qDebug() << client.def().id().fieldName();
//   qDebug() << client.def().firstName().fieldName();
}

/*
 * Tests
 */

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  ReflectionTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
