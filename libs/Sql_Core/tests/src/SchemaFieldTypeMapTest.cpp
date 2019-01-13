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
#include "SchemaFieldTypeMapTest.h"
#include "Mdt/Sql/Schema/FieldTypeMap.h"
#include "Mdt/Sql/Schema/FieldType.h"

using namespace Mdt::Sql::Schema;

void SchemaFieldTypeMapTest::initTestCase()
{
}

void SchemaFieldTypeMapTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void SchemaFieldTypeMapTest::constructTest()
{
  FieldTypeMap ftm0;
  QVERIFY(!ftm0.isValid());
  ftm0.setDefaultImplementation();
  QVERIFY(ftm0.isValid());
  QCOMPARE(ftm0.fieldTypeFromQMetaType(QMetaType::Int, FieldLength()), FieldType::Integer);

  auto ftm1 = FieldTypeMap::make();
  QVERIFY(ftm1.isValid());
  QCOMPARE(ftm1.fieldTypeFromQMetaType(QMetaType::Int, FieldLength()), FieldType::Integer);
}

void SchemaFieldTypeMapTest::fieldTypeFromQMetaTypeTest()
{
  const auto ftm = FieldTypeMap::make();

  QCOMPARE(ftm.fieldTypeFromQMetaType(QMetaType::UnknownType, FieldLength()), FieldType::UnknownType);
  QCOMPARE(ftm.fieldTypeFromQMetaType(QMetaType::Bool, FieldLength()), FieldType::Boolean);
  QCOMPARE(ftm.fieldTypeFromQMetaType(QMetaType::Short, FieldLength()), FieldType::Smallint);
  QCOMPARE(ftm.fieldTypeFromQMetaType(QMetaType::UShort, FieldLength()), FieldType::Smallint);
  QCOMPARE(ftm.fieldTypeFromQMetaType(QMetaType::Int, FieldLength()), FieldType::Integer);
  QCOMPARE(ftm.fieldTypeFromQMetaType(QMetaType::UInt, FieldLength()), FieldType::Integer);
  QCOMPARE(ftm.fieldTypeFromQMetaType(QMetaType::Long, FieldLength()), FieldType::Bigint);
  QCOMPARE(ftm.fieldTypeFromQMetaType(QMetaType::ULong, FieldLength()), FieldType::Bigint);
  QCOMPARE(ftm.fieldTypeFromQMetaType(QMetaType::LongLong, FieldLength()), FieldType::Bigint);
  QCOMPARE(ftm.fieldTypeFromQMetaType(QMetaType::ULongLong, FieldLength()), FieldType::Bigint);
  QCOMPARE(ftm.fieldTypeFromQMetaType(QMetaType::Float, FieldLength()), FieldType::Float);
  QCOMPARE(ftm.fieldTypeFromQMetaType(QMetaType::Double, FieldLength()), FieldType::Double);
  QCOMPARE(ftm.fieldTypeFromQMetaType(QMetaType::QChar, FieldLength()), FieldType::Char);
  QCOMPARE(ftm.fieldTypeFromQMetaType(QMetaType::QString, FieldLength(1)), FieldType::Char);
  QCOMPARE(ftm.fieldTypeFromQMetaType(QMetaType::QString, FieldLength(2)), FieldType::Char);
  QCOMPARE(ftm.fieldTypeFromQMetaType(QMetaType::QString, FieldLength(3)), FieldType::Varchar);
  QCOMPARE(ftm.fieldTypeFromQMetaType(QMetaType::QString, FieldLength()), FieldType::Text);
  QCOMPARE(ftm.fieldTypeFromQMetaType(QMetaType::QByteArray, FieldLength()), FieldType::Blob);
}

void SchemaFieldTypeMapTest::fieldTypeFromQVariantTypeTest()
{
  const auto ftm = FieldTypeMap::make();

  QCOMPARE(ftm.fieldTypeFromQVariantType(QVariant::Int, FieldLength()), FieldType::Integer);
}

void SchemaFieldTypeMapTest::fieldTypeFlagsFromQMetaTypeTest()
{
  const auto ftm = FieldTypeMap::make();

  QCOMPARE(ftm.fieldTypeFlagsFromQMetaType(QMetaType::UnknownType), FieldTypeFlags(FieldTypeFlag::NoFlag));
  QCOMPARE(ftm.fieldTypeFlagsFromQMetaType(QMetaType::Bool), FieldTypeFlags(FieldTypeFlag::NoFlag));
  QCOMPARE(ftm.fieldTypeFlagsFromQMetaType(QMetaType::Short), FieldTypeFlags(FieldTypeFlag::NoFlag));
  QCOMPARE(ftm.fieldTypeFlagsFromQMetaType(QMetaType::UShort), FieldTypeFlags(FieldTypeFlag::IsUnsigned));
  QCOMPARE(ftm.fieldTypeFlagsFromQMetaType(QMetaType::Int), FieldTypeFlags(FieldTypeFlag::NoFlag));
  QCOMPARE(ftm.fieldTypeFlagsFromQMetaType(QMetaType::UInt), FieldTypeFlags(FieldTypeFlag::IsUnsigned));
  QCOMPARE(ftm.fieldTypeFlagsFromQMetaType(QMetaType::Long), FieldTypeFlags(FieldTypeFlag::NoFlag));
  QCOMPARE(ftm.fieldTypeFlagsFromQMetaType(QMetaType::ULong), FieldTypeFlags(FieldTypeFlag::IsUnsigned));
  QCOMPARE(ftm.fieldTypeFlagsFromQMetaType(QMetaType::LongLong), FieldTypeFlags(FieldTypeFlag::NoFlag));
  QCOMPARE(ftm.fieldTypeFlagsFromQMetaType(QMetaType::ULongLong), FieldTypeFlags(FieldTypeFlag::IsUnsigned));
  QCOMPARE(ftm.fieldTypeFlagsFromQMetaType(QMetaType::Float), FieldTypeFlags(FieldTypeFlag::NoFlag));
  QCOMPARE(ftm.fieldTypeFlagsFromQMetaType(QMetaType::Double), FieldTypeFlags(FieldTypeFlag::NoFlag));
  QCOMPARE(ftm.fieldTypeFlagsFromQMetaType(QMetaType::QChar), FieldTypeFlags(FieldTypeFlag::NoFlag));
  QCOMPARE(ftm.fieldTypeFlagsFromQMetaType(QMetaType::QString), FieldTypeFlags(FieldTypeFlag::NoFlag));
  QCOMPARE(ftm.fieldTypeFlagsFromQMetaType(QMetaType::QByteArray), FieldTypeFlags(FieldTypeFlag::NoFlag));
}

void SchemaFieldTypeMapTest::fieldTypeFlagsFromQVariantTypeTest()
{
  const auto ftm = FieldTypeMap::make();

  QCOMPARE(ftm.fieldTypeFlagsFromQVariantType(QVariant::UInt), FieldTypeFlags(FieldTypeFlag::IsUnsigned));
}

void SchemaFieldTypeMapTest::qmetaTypeFromFieldTypeTest()
{
  const auto ftm = FieldTypeMap::make();

  QCOMPARE(ftm.qmetaTypeFromFieldType(FieldType::UnknownType, FieldLength(), FieldTypeFlags(FieldTypeFlag::NoFlag)), QMetaType::UnknownType);
  QCOMPARE(ftm.qmetaTypeFromFieldType(FieldType::Boolean, FieldLength(), FieldTypeFlags(FieldTypeFlag::NoFlag)), QMetaType::Bool);
  QCOMPARE(ftm.qmetaTypeFromFieldType(FieldType::Smallint, FieldLength(), FieldTypeFlags(FieldTypeFlag::NoFlag)), QMetaType::Short);
  QCOMPARE(ftm.qmetaTypeFromFieldType(FieldType::Smallint, FieldLength(), FieldTypeFlags(FieldTypeFlag::IsUnsigned)), QMetaType::UShort);
  QCOMPARE(ftm.qmetaTypeFromFieldType(FieldType::Integer, FieldLength(), FieldTypeFlags(FieldTypeFlag::NoFlag)), QMetaType::Int);
  QCOMPARE(ftm.qmetaTypeFromFieldType(FieldType::Integer, FieldLength(), FieldTypeFlags(FieldTypeFlag::IsUnsigned)), QMetaType::UInt);
  QCOMPARE(ftm.qmetaTypeFromFieldType(FieldType::Bigint, FieldLength(), FieldTypeFlags(FieldTypeFlag::NoFlag)), QMetaType::LongLong);
  QCOMPARE(ftm.qmetaTypeFromFieldType(FieldType::Bigint, FieldLength(), FieldTypeFlags(FieldTypeFlag::IsUnsigned)), QMetaType::ULongLong);
  QCOMPARE(ftm.qmetaTypeFromFieldType(FieldType::Float, FieldLength(), FieldTypeFlags(FieldTypeFlag::NoFlag)), QMetaType::Float);
  QCOMPARE(ftm.qmetaTypeFromFieldType(FieldType::Double, FieldLength(), FieldTypeFlags(FieldTypeFlag::NoFlag)), QMetaType::Double);
  QCOMPARE(ftm.qmetaTypeFromFieldType(FieldType::Char, FieldLength(), FieldTypeFlags(FieldTypeFlag::NoFlag)), QMetaType::QChar);
  QCOMPARE(ftm.qmetaTypeFromFieldType(FieldType::Char, FieldLength(1), FieldTypeFlags(FieldTypeFlag::NoFlag)), QMetaType::QChar);
  QCOMPARE(ftm.qmetaTypeFromFieldType(FieldType::Char, FieldLength(2), FieldTypeFlags(FieldTypeFlag::NoFlag)), QMetaType::QString);
  QCOMPARE(ftm.qmetaTypeFromFieldType(FieldType::Varchar, FieldLength(3), FieldTypeFlags(FieldTypeFlag::NoFlag)), QMetaType::QString);
  QCOMPARE(ftm.qmetaTypeFromFieldType(FieldType::Text, FieldLength(), FieldTypeFlags(FieldTypeFlag::NoFlag)), QMetaType::QString);
  QCOMPARE(ftm.qmetaTypeFromFieldType(FieldType::Blob, FieldLength(), FieldTypeFlags(FieldTypeFlag::NoFlag)), QMetaType::QByteArray);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SchemaFieldTypeMapTest test;

  return QTest::qExec(&test, argc, argv);
}
