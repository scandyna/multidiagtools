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
#include "FieldDataValidatorTest.h"
#include "Mdt/Entity/FieldDataValidator.h"
#include "Mdt/Entity/DataTemplate.h"
#include "Mdt/Entity/IntegralUniqueIdTemplate.h"
#include "Mdt/Entity/Def.h"

// #include <QDebug>

using namespace Mdt::Entity;

/*
 * Custom type
 */

class MyCustomType
{
 public:

  MyCustomType() = default;
  MyCustomType(int v) = delete;

  void setValue(int v)
  {
    mValue = v;
  }

  int value() const
  {
    return mValue;
  }

  // Do not use isNull() for the test
  bool hasNonNullValue() const
  {
    return (mValue != 0);
  }

 private:

  int mValue = 0;
};

/*
 * Entities
 */

struct ArticleDataStruct
{
  qlonglong id;
  QString description;
  MyCustomType custom;
};

MDT_ENTITY_DEF(
  (ArticleDataStruct),
  Article,
  (id, FieldFlag::IsRequired, FieldFlag::IsUnique),
  (description, FieldMaxLength(5)),
  (custom, FieldFlag::IsRequired)
)

class ArticleData : public Mdt::Entity::DataTemplate<ArticleEntity>
{
 public:

  void setId(qlonglong id)
  {
    dataStruct().id = id;
  }

  qlonglong id() const
  {
    return constDataStruct().id;
  }

  void setCustomValue(int v)
  {
    dataStruct().custom.setValue(v);
  }

  MyCustomType custom() const
  {
    return constDataStruct().custom;
  }
};

struct MultiIdDataStruct
{
  int pkId = 0;
  int requiredId = 0;
  int optionalId = 0;
};

MDT_ENTITY_DEF(
  (MultiIdDataStruct),
  MultiId,
  (pkId, FieldFlag::IsPrimaryKey),
  (requiredId, FieldFlag::IsRequired),
  (optionalId)
)

class Id : public Mdt::Entity::IntegralUniqueIdTemplate<Id, int>
{
 public:

  using IntegralUniqueIdTemplate::IntegralUniqueIdTemplate;
};

class MultiIdData : public Mdt::Entity::DataTemplate<MultiIdEntity>
{
 public:

  void setPkId(Id id)
  {
    dataStruct().pkId = id.value();
  }

  Id pkId() const
  {
    return Id(constDataStruct().pkId);
  }

  void setRequiredId(int id)
  {
    dataStruct().requiredId = id;
  }

  int requiredId() const
  {
    return constDataStruct().requiredId;
  }

  void setOptionalId(int id)
  {
    dataStruct().optionalId = id;
  }

  int optionalId() const
  {
    return constDataStruct().optionalId;
  }
};

/*
 * Custom type validator implementation
 */

class MyCustomFieldDataValidatorImplementation : public Mdt::Entity::FieldDataValidatorCommonImplementation
{
 public:

  using Mdt::Entity::FieldDataValidatorCommonImplementation::validateData;

  Mdt::Entity::FieldDataValidatorState validateData(const MyCustomType & data, const Mdt::Entity::FieldAttributes & fieldAttributes) const
  {
    using Mdt::Entity::FieldDataValidatorState;

    if( !data.hasNonNullValue() && (fieldAttributes.isRequired()) ){
      return FieldDataValidatorState::RequiredButNull;
    }
    return FieldDataValidatorState::Ok;
  }
};

void FieldDataValidatorTest::stateChangeTest()
{
  FieldDataValidator<> v;
  QCOMPARE(v.state(), FieldDataValidatorState::Ok);
  QVERIFY(v.toGenericError().isNull());

  ArticleData article;
  article.setId(1);
  QVERIFY(v.validateData(article.id(), article.def().id()));
  QCOMPARE(v.state(), FieldDataValidatorState::Ok);
  QVERIFY(v.toGenericError().isNull());

  article.setId(0);
  QVERIFY(!v.validateData(article.id(), article.def().id()));
  QCOMPARE(v.state(), FieldDataValidatorState::RequiredButNull);
  QVERIFY(!v.toGenericError().isNull());
}

void FieldDataValidatorTest::customImplementationTest()
{
  FieldDataValidator<MyCustomFieldDataValidatorImplementation> v;
  ArticleData article;

  QCOMPARE(article.custom().value(), 0);
  QVERIFY(!v.validateData( article.custom(), article.def().custom() ));
  article.setCustomValue(5);
  QVERIFY(v.validateData( article.custom(), article.def().custom() ));

  article.setId(3);
  QVERIFY(v.validateData(article.id(), article.def().id()));
}

/*
 * Validate data test
 */

struct ValidationTestDataStruct
{
  int freeInt;
  int requiredInt;
  QString str;
  QString str3;
  QString requiredStr;
  QString requiredStr3;
};

MDT_ENTITY_DEF(
  (ValidationTestDataStruct),
  ValidationTest,
  (freeInt),
  (requiredInt, FieldFlag::IsRequired),
  (str),
  (str3, FieldMaxLength(3)),
  (requiredStr, FieldFlag::IsRequired),
  (requiredStr3, FieldMaxLength(3), FieldFlag::IsRequired)
)

class ValidationTestData : public DataTemplate<ValidationTestEntity>
{
 public:

  void setFreeInt(int v)
  {
    dataStruct().freeInt = v;
  }

  int freeInt() const
  {
    return constDataStruct().freeInt;
  }

  void setRequiredInt(int v)
  {
    dataStruct().requiredInt = v;
  }

  int requiredInt() const
  {
    return constDataStruct().requiredInt;
  }

  void setStr(const QString & str)
  {
    dataStruct().str = str;
  }

  QString str() const
  {
    return constDataStruct().str;
  }

  void setStr3(const QString & str)
  {
    dataStruct().str3 = str;
  }

  QString str3() const
  {
    return constDataStruct().str3;
  }

  void setRequiredStr(const QString & str)
  {
    dataStruct().requiredStr = str;
  }

  QString requiredStr() const
  {
    return constDataStruct().requiredStr;
  }

  void setRequiredStr3(const QString & str)
  {
    dataStruct().requiredStr3 = str;
  }

  QString requiredStr3() const
  {
    return constDataStruct().requiredStr3;
  }
};

void FieldDataValidatorTest::validateDataTest()
{
  FieldDataValidator<> v;
  ValidationTestData data;

  data.setFreeInt(0);
  QVERIFY(v.validateData( data.freeInt(), data.def().freeInt() ));
  QCOMPARE(v.state(), FieldDataValidatorState::Ok);
  data.setFreeInt(5);
  QVERIFY(v.validateData( data.freeInt(), data.def().freeInt() ));
  QCOMPARE(v.state(), FieldDataValidatorState::Ok);

  data.setRequiredInt(0);
  QVERIFY(!v.validateData( data.requiredInt(), data.def().requiredInt() ));
  QCOMPARE(v.state(), FieldDataValidatorState::RequiredButNull);
  data.setRequiredInt(1);
  QVERIFY(v.validateData( data.requiredInt(), data.def().requiredInt() ));
  QCOMPARE(v.state(), FieldDataValidatorState::Ok);

  QVERIFY(data.str().isEmpty());
  QVERIFY(v.validateData( data.str(), data.def().str() ));
  QCOMPARE(v.state(), FieldDataValidatorState::Ok);
  data.setStr("A");
  QVERIFY(v.validateData( data.str(), data.def().str() ));
  QCOMPARE(v.state(), FieldDataValidatorState::Ok);

  QVERIFY(data.str3().isEmpty());
  QVERIFY(v.validateData( data.str3(), data.def().str3() ));
  QCOMPARE(v.state(), FieldDataValidatorState::Ok);
  data.setStr3("A");
  QVERIFY(v.validateData( data.str3(), data.def().str3() ));
  QCOMPARE(v.state(), FieldDataValidatorState::Ok);
  data.setStr3("ABC");
  QVERIFY(v.validateData( data.str3(), data.def().str3() ));
  QCOMPARE(v.state(), FieldDataValidatorState::Ok);
  data.setStr3("ABCD");
  QVERIFY(!v.validateData( data.str3(), data.def().str3() ));
  QCOMPARE(v.state(), FieldDataValidatorState::MaxLengthExeeded);

  QVERIFY(data.requiredStr().isEmpty());
  QVERIFY(!v.validateData( data.requiredStr(), data.def().requiredStr() ));
  QCOMPARE(v.state(), FieldDataValidatorState::RequiredButNull);
  data.setRequiredStr(" ");
  QVERIFY(!v.validateData( data.requiredStr(), data.def().requiredStr() ));
  QCOMPARE(v.state(), FieldDataValidatorState::RequiredButNull);
  data.setRequiredStr("A");
  QVERIFY(v.validateData( data.requiredStr(), data.def().requiredStr() ));
  QCOMPARE(v.state(), FieldDataValidatorState::Ok);
  data.setRequiredStr("ABCD");
  QVERIFY(v.validateData( data.requiredStr(), data.def().requiredStr() ));
  QCOMPARE(v.state(), FieldDataValidatorState::Ok);

  QVERIFY(data.requiredStr3().isEmpty());
  QVERIFY(!v.validateData( data.requiredStr3(), data.def().requiredStr3() ));
  QCOMPARE(v.state(), FieldDataValidatorState::RequiredButNull);
  data.setRequiredStr3(" ");
  QVERIFY(!v.validateData( data.requiredStr3(), data.def().requiredStr3() ));
  QCOMPARE(v.state(), FieldDataValidatorState::RequiredButNull);
  data.setRequiredStr3("ABC");
  QVERIFY(v.validateData( data.requiredStr3(), data.def().requiredStr3() ));
  QCOMPARE(v.state(), FieldDataValidatorState::Ok);
  data.setRequiredStr3("ABCD");
  QVERIFY(!v.validateData( data.requiredStr3(), data.def().requiredStr3() ));
  QCOMPARE(v.state(), FieldDataValidatorState::MaxLengthExeeded);
}

void FieldDataValidatorTest::validateItergralUniqueIdTest()
{
  FieldDataValidator<> v;
  MultiIdData data;

  QVERIFY(data.pkId().isNull());
  QVERIFY( !v.validateData(data.pkId(), data.def().pkId()) );
  QCOMPARE(v.state(), FieldDataValidatorState::RequiredButNull);

  data.setPkId(Id(1));
  QVERIFY( v.validateData(data.pkId(), data.def().pkId()) );
  QCOMPARE(v.state(), FieldDataValidatorState::Ok);

  QCOMPARE(data.requiredId(), 0);
  QVERIFY( !v.validateData(data.requiredId(), data.def().requiredId()) );
  QCOMPARE(v.state(), FieldDataValidatorState::RequiredButNull);

  data.setRequiredId(2);
  QVERIFY( v.validateData(data.requiredId(), data.def().requiredId()) );
  QCOMPARE(v.state(), FieldDataValidatorState::Ok);

  QCOMPARE(data.optionalId(), 0);
  QVERIFY( v.validateData(data.optionalId(), data.def().optionalId()) );
  QCOMPARE(v.state(), FieldDataValidatorState::Ok);

  data.setOptionalId(3);
  QVERIFY( v.validateData(data.optionalId(), data.def().optionalId()) );
  QCOMPARE(v.state(), FieldDataValidatorState::Ok);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  FieldDataValidatorTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
