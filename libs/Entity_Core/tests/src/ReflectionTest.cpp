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
#include <QFlags>
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

// #include <boost/fusion/adapted/struct/adapt_assoc_struct_named.hpp>
// #include <boost/fusion/include/adapt_assoc_struct_named.hpp>


#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/variadic_seq_to_seq.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/to_array.hpp>
#include <boost/preprocessor/tuple/reverse.hpp>
#include <boost/preprocessor/array/enum.hpp>
#include <boost/preprocessor/array/pop_front.hpp>


#include <boost/preprocessor/tuple/pop_front.hpp>
#include <boost/preprocessor/tuple/rem.hpp>

void ReflectionTest::initTestCase()
{
}

void ReflectionTest::cleanupTestCase()
{
}

template<typename Data>
struct PrintDef
{
  PrintDef(const Data & data)
   : mData(data)
  {
  }

  template<typename Key>
  void operator()(const Key &) const
  {
    qDebug() << Key::fieldName() << ":";
    static const auto attributes = Key::fieldAttributes();
    qDebug() << " required: " << attributes.isRequired();
    qDebug() << " unique:: " << attributes.isUnique();
    qDebug() << " max length: " << attributes.maxLength();
    qDebug() << " value: " << boost::fusion::at_key<Key>(mData);
  }

  const Data & mData;
};

template<typename Data>
struct ModifyData
{
  ModifyData(Data & data)
   : mData(data)
  {
  }

  template<typename Key>
  void operator()(const Key &) const
  {
    modify(boost::fusion::at_key<Key>(mData));
  }

  void modify(int & value) const
  {
    ++value;
  }

  void modify(QString & value) const
  {
    value += "+";
  }

  template<typename T>
  void modify(T &) const
  {
    qDebug() << "Modify unhandled type";
  }

  Data & mData;
};

class DataStorage
{
 public:

  template<typename T>
  void printData(const T & data)
  {
    qDebug() << "E: " << data.def().entityName();
    boost::fusion::for_each(data.def(), PrintDef<decltype(data.data())>(data.data()));
  }

  /// \todo Will be a list
  template<typename T>
  T selectAll(const T & t)
  {
    T r = t;
    qDebug() << "SELECT all from " << t.def().entityName();
    boost::fusion::for_each(r.def(), ModifyData<decltype(r.data())>(r.data()));
    return r;
  }

  /// \todo Will be a list
  template<typename T>
  T select(const T & t /*, conditions*/)
  {
    qDebug() << "SELECT from " << t.def().entityName() << " where ??";
    return t;
  }

};

enum class FieldFlag
{
  NoFlag = 0x00,
  IsRequired = 0x01,
  IsUnique = 0x02
};
Q_DECLARE_FLAGS(FieldFlags, FieldFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(FieldFlags)

class FieldMaxLength
{
 public:

  FieldMaxLength() = default;

  explicit FieldMaxLength(int length)
   : mLength(length)
  {
  }

  int length() const
  {
    return mLength;
  }

 private:

  int mLength = 0;
};

class FieldAttributes
{
 public:

  FieldAttributes() = default;

  explicit FieldAttributes(FieldFlags flags)
   : mFlags(flags)
  {
  }

  explicit FieldAttributes(FieldMaxLength maxLength)
   : mMaxLength(maxLength)
  {
  }

  explicit FieldAttributes(FieldFlags flags, FieldMaxLength maxLength)
   : mFlags(flags),
     mMaxLength(maxLength)
  {
  }

  bool isRequired() const
  {
    return mFlags.testFlag(FieldFlag::IsRequired);
  }

  bool isUnique() const
  {
    return mFlags.testFlag(FieldFlag::IsUnique);
  }

  int maxLength() const
  {
    return mMaxLength.length();
  }

 private:

  FieldFlags mFlags = FieldFlag::NoFlag;
  FieldMaxLength mMaxLength;
};

class ClientId
{
 public:

  ClientId() = default;
  ClientId(const ClientId &) = default;

  ClientId(int id)
   : mId(id)
  {
  }

  void setId(int id)
  {
    mId = id;
  }

  int id() const
  {
    return mId;
  }

 private:

  int mId = 0;
};
QDebug operator<<(QDebug dbg, const ClientId & id)
{
  dbg << id.id();
  return dbg;
}

struct ClientDataStruct
{
  ClientId id;
  QString firstName;
};

struct ClientDataStructDef
{
  static const QString entityName()
  {
    return "Client";
  }

  struct idField
  {
    static const QString fieldName()
    {
      return "id";
    }

    static const FieldAttributes fieldAttributes()
    {
      return FieldAttributes(FieldFlag::IsRequired | FieldFlag::IsUnique);
    }
  };

  struct firstNameField
  {
    static const QString fieldName()
    {
      return "firstName";
    }

    static const FieldAttributes fieldAttributes()
    {
      return FieldAttributes(FieldMaxLength(250));
    }
  };

  idField id;
  firstNameField firstName;
};
// Make it possible to iterate through the data struct definition
BOOST_FUSION_ADAPT_STRUCT(
  ClientDataStructDef,
  id,
  firstName
)

BOOST_FUSION_ADAPT_ASSOC_STRUCT(
  ClientDataStruct,
  (id, ClientDataStructDef::idField)
  (firstName, ClientDataStructDef::firstNameField)
)

/// \todo Create entity list + const entity list class templates ?

class ClientData
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

  const ClientDataStruct & data() const
  {
    return mData;
  }

  ClientDataStruct & data()
  {
    return mData;
  }

  static const ClientDataStructDef def()
  {
    return ClientDataStructDef{};
  }

 private:

  ClientDataStruct mData;
};

void ReflectionTest::sandboxFusion()
{
  ClientDataStruct ds2;
  ds2.id = 22;
  ds2.firstName = "Name 22";
  qDebug() << "2 id: " << boost::fusion::at_key<ClientDataStructDef::idField>(ds2);
  qDebug() << "2 first name: " << boost::fusion::at_key<ClientDataStructDef::firstNameField>(ds2);

//   boost::fusion::for_each(ClientDataStructDef(), PrintDef(ds2));

}

#if !BOOST_PP_VARIADICS
 #error "Compiler preprocessor does not support variadics"
#endif

#define MDT_ENTITY_FIELD_DEF_NAME(fieldName) \
  BOOST_PP_CAT(fieldName, Field)

#define MDT_ENTITY_FIELD_ELEM_NAME(elem) \
  BOOST_PP_TUPLE_ELEM(0, elem)

#define MDT_ENTITY_FIELD_ELEM_NAME_STR(elem) \
  BOOST_PP_STRINGIZE( MDT_ENTITY_FIELD_ELEM_NAME(elem) )

#define MDT_ENTITY_FIELD_ELEM_DEF_NAME(elem) \
  MDT_ENTITY_FIELD_DEF_NAME( MDT_ENTITY_FIELD_ELEM_NAME(elem) )

#define MDT_ENTITY_FIELD_ELEM_ATTRIBUTES_ARGS(elem) \
  BOOST_PP_ARRAY_ENUM( \
    BOOST_PP_ARRAY_POP_FRONT( \
      BOOST_PP_TUPLE_TO_ARRAY(elem) \
    ) \
  )
//   BOOST_PP_TUPLE_REM() BOOST_PP_TUPLE_POP_FRONT(elem)

#define MDT_ENTITY_FIELD_DEF(fieldTuple) \
  struct MDT_ENTITY_FIELD_ELEM_DEF_NAME(fieldTuple) \
  { \
    static const QString fieldName() \
    { \
      return QStringLiteral( MDT_ENTITY_FIELD_ELEM_NAME_STR(fieldTuple) ); \
    } \
    \
    static const FieldAttributes fieldAttributes() \
    { \
      return FieldAttributes(MDT_ENTITY_FIELD_ELEM_ATTRIBUTES_ARGS(fieldTuple)); \
    } \
  };

#define MDT_ENTITY_FIELD_DEF_INSTANCE(fieldTuple) \
  MDT_ENTITY_FIELD_ELEM_DEF_NAME(fieldTuple) MDT_ENTITY_FIELD_ELEM_NAME(fieldTuple);

#define MDT_ENTITY_DATA_STRUCT_DEF_MEMBER_FIELD(r, data, elem) \
  MDT_ENTITY_FIELD_DEF(elem) \
  MDT_ENTITY_FIELD_DEF_INSTANCE(elem)

#define MDT_ENTITY_DATA_STRUCT_DEF_MEMBER_FIELD_LIST(...) \
  BOOST_PP_SEQ_FOR_EACH( \
    MDT_ENTITY_DATA_STRUCT_DEF_MEMBER_FIELD, data, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__) \
  )

#define MDT_ENTITY_DATA_STRUCT_DEF_MEMBER(r, data, elem) \
  ( MDT_ENTITY_FIELD_ELEM_NAME(elem) )

#define MDT_ENTITY_DATA_STRUCT_DEF_MEMBER_SEQ(...) \
  BOOST_PP_SEQ_FOR_EACH( \
    MDT_ENTITY_DATA_STRUCT_DEF_MEMBER, data, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__) \
  )

#define MDT_ENTITY_DATA_STRUCT_DEF_MEMBER_ASSOC(r, defName, elem) \
  ( MDT_ENTITY_FIELD_ELEM_NAME(elem) , defName::MDT_ENTITY_FIELD_ELEM_DEF_NAME(elem) )

#define MDT_ENTITY_DATA_STRUCT_DEF_MEMBER_ASSOC_SEQ(defName, ...) \
  BOOST_PP_SEQ_FOR_EACH( \
    MDT_ENTITY_DATA_STRUCT_DEF_MEMBER_ASSOC, defName, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__) \
  )

/*
#define MDT_ENTITY_DATA_STRUCT_DEF_ADAPT_ASSOC_MEMBER(r, dataStructDef, elem) \
  (elem, dataStructDef::BOOST_PP_CAT(elem, Field))

#define MDT_ENTITY_DATA_STRUCT_DEF_ADAPT_ASSOC_MEMBER_LIST(dataStructDef, ...) \
  BOOST_PP_SEQ_FOR_EACH( \
    MDT_ENTITY_DATA_STRUCT_DEF_ADAPT_ASSOC_MEMBER, dataStructDef, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__) \
  )
*/

#define MDT_ENTITY_DEF_ELEM_DATA_STRUCT_NAME(defTuple) \
  BOOST_PP_TUPLE_ELEM(0 BOOST_PP_TUPLE_REVERSE(defTuple) )

#define MDT_ENTITY_DEF_ELEM_NAME(defTuple) \
  BOOST_PP_CAT( MDT_ENTITY_DEF_ELEM_DATA_STRUCT_NAME(defTuple), Def )

#define MDT_ENTITY_DATA_STRUCT_DEF_NAME(dataStructName) \
  BOOST_PP_CAT(dataStructName, Def)

#define MDT_ENTITY_NAME_STR(name) \
  BOOST_PP_STRINGIZE(name)

#define MDT_ENTITY_DATA_STRUCT_DEF(dataStructName, name, ...) \
  struct MDT_ENTITY_DATA_STRUCT_DEF_NAME(dataStructName) \
  { \
    static const QString entityName() \
    { \
      return QStringLiteral( MDT_ENTITY_NAME_STR(name) ); \
    } \
      \
    MDT_ENTITY_DATA_STRUCT_DEF_MEMBER_FIELD_LIST(__VA_ARGS__) \
  }; \
  BOOST_FUSION_ADAPT_STRUCT( \
    MDT_ENTITY_DATA_STRUCT_DEF_NAME(dataStructName) , \
    BOOST_PP_SEQ_ENUM( MDT_ENTITY_DATA_STRUCT_DEF_MEMBER_SEQ(__VA_ARGS__) ) \
  ) \
  BOOST_FUSION_ADAPT_ASSOC_STRUCT( \
    dataStructName, \
    MDT_ENTITY_DATA_STRUCT_DEF_MEMBER_ASSOC_SEQ( \
      MDT_ENTITY_DATA_STRUCT_DEF_NAME(dataStructName), __VA_ARGS__ \
    ) \
  )


// namespace MyEntities{
struct ArticleDataStruct
{
  qlonglong id;
  QString description;
  QString remarks;
  bool other;
};
// }

MDT_ENTITY_DATA_STRUCT_DEF(
  (ArticleDataStruct),
  Article,
  (id, FieldFlag::IsRequired | FieldFlag::IsUnique, FieldMaxLength(233) ),
  (description, FieldFlag::IsRequired, FieldMaxLength(250)),
  (remarks),
  (other)
)

// MDT_ENTITY_DATA_STRUCT_ADAPT(
//   ArticleDataStruct,
// )

void ReflectionTest::sandBoxMacro()
{
//   idField id;
//   qDebug() << id.fieldName();
//   ArticleDataStruct ads;
//   ArticleDataStructDef def;
//   qDebug() << def.entityName();
//   qDebug() << def.id.fieldName();
// 
//   qDebug() << "E: " << def.entityName();
//   boost::fusion::for_each(def, PrintDef<decltype(ads)>(ads));
}

void ReflectionTest::sandbox()
{
  ClientData client;
  client.setId(5);
  client.setFirstName("FN 5");
  DataStorage storage;
  storage.printData(client);
  client = storage.selectAll(client);
  storage.printData(client);
  client = storage.select(client);
  storage.printData(client);
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
