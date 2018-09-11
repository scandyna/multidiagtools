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
#include "PrimaryKeyTest.h"
#include "Mdt/Entity/PrimaryKey.h"
#include "Mdt/Entity/PrimaryKeyRecord.h"
#include "Mdt/Entity/PrimaryKeyRecordFilterExpression.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/DataTemplate.h"
#include "Mdt/QueryExpression/AbstractExpressionTreeVisitor.h"
#include "Mdt/QueryExpression/TravserseTreeGraph.h"
#include <boost/variant.hpp>

using namespace Mdt::Entity;

void PrimaryKeyTest::initTestCase()
{
}

void PrimaryKeyTest::cleanupTestCase()
{
}

/*
 * Visitors to parse a expression tree
 */

class ExpressionToStringVisitor : public Mdt::QueryExpression::AbstractExpressionTreeVisitor
{
 public:

  void processPreorder(Mdt::QueryExpression::LogicalOperator) override
  {
    mExpressionString += "(";
  }

  void processPostorder(Mdt::QueryExpression::LogicalOperator) override
  {
    mExpressionString += ")";
  }

  void processInorder(Mdt::QueryExpression::ComparisonOperator op) override
  {
    mExpressionString += comparisonOperatorToString(op);
  }

  void processInorder(Mdt::QueryExpression::LogicalOperator op) override
  {
    mExpressionString += ")" + logicalOperatorToString(op) + "(";
  }

  void processInorder(const Mdt::QueryExpression::EntityAndField & field) override
  {
    mExpressionString += field.entityAliasOrName() + "." + field.fieldAliasOrName();
  }

  void processInorder(const QVariant & value) override
  {
    mExpressionString += value.toString();
  }

  void clear()
  {
    mExpressionString.clear();
  }

  QString toString() const
  {
    return mExpressionString;
  }

 private:

  static QString comparisonOperatorToString(Mdt::QueryExpression::ComparisonOperator op)
  {
    using Mdt::QueryExpression::ComparisonOperator;
    switch(op){
      case ComparisonOperator::Equal:
        return "==";
      case ComparisonOperator::Like:
        return " Like ";
      case ComparisonOperator::NotEqual:
        return "!=";
      case ComparisonOperator::Less:
        return "<";
      case ComparisonOperator::LessEqual:
        return "<=";
      case ComparisonOperator::Greater:
        return ">";
      case ComparisonOperator::GreaterEqual:
        return ">=";
    }
  }

  static QString logicalOperatorToString(Mdt::QueryExpression::LogicalOperator op)
  {
    using Mdt::QueryExpression::LogicalOperator;
    switch(op){
      case LogicalOperator::And:
        return "&&";
      case LogicalOperator::Or:
        return "||";
    }
    return QString();
  }

  QString mExpressionString;
};

/*
 * Entities
 */

struct ArticleDataStruct
{
  qlonglong id;
  QString description;
};

MDT_ENTITY_DEF(
  (ArticleDataStruct),
  Article,
  (id, FieldFlag::IsPrimaryKey),
  (description, FieldFlag::IsRequired, FieldMaxLength(250))
)

class ArticleData : public DataTemplate<ArticleEntity>
{
 public:

  void setId(int id)
  {
    dataStruct().id = id;
  }

  int id() const
  {
    return constDataStruct().id;
  }
};

struct LinkDataStruct
{
  QString code;
  qlonglong startId;
  qlonglong endId;
  QString linkName;
};

MDT_ENTITY_DEF(
  (LinkDataStruct),
  Link,
  (code),
  (startId, FieldFlag::IsPrimaryKey),
  (endId, FieldFlag::IsPrimaryKey),
  (linkName, FieldFlag::IsRequired, FieldMaxLength(100))
)

class LinkData : public DataTemplate<LinkEntity>
{
 public:

  void setStartId(int id)
  {
    dataStruct().startId = id;
  }

  int startId() const
  {
    return constDataStruct().startId;
  }

  void setEndId(int id)
  {
    dataStruct().endId = id;
  }

  int endId() const
  {
    return constDataStruct().endId;
  }
};

struct ThreePkDataStruct
{
  QString a;
  int pk1;
  QString b;
  int pk2;
  int pk3;
};

MDT_ENTITY_DEF(
  (ThreePkDataStruct),
  ThreePk,
  (a),
  (pk1, FieldFlag::IsPrimaryKey),
  (b),
  (pk2, FieldFlag::IsPrimaryKey),
  (pk3, FieldFlag::IsPrimaryKey)
)

class ThreePkData : public DataTemplate<ThreePkEntity>
{
 public:

  void setPk1(int id)
  {
    dataStruct().pk1 = id;
  }

  void setPk2(int id)
  {
    dataStruct().pk2 = id;
  }

  void setPk3(int id)
  {
    dataStruct().pk3 = id;
  }
};

struct FourPkDataStruct
{
  QString a;
  int pk1;
  QString b;
  int pk2;
  int pk3;
  QString c;
  int pk4;
};

MDT_ENTITY_DEF(
  (FourPkDataStruct),
  FourPk,
  (a),
  (pk1, FieldFlag::IsPrimaryKey),
  (b),
  (pk2, FieldFlag::IsPrimaryKey),
  (pk3, FieldFlag::IsPrimaryKey),
  (c),
  (pk4, FieldFlag::IsPrimaryKey)
)

class FourPkData : public DataTemplate<FourPkEntity>
{
 public:

  void setPk1(int id)
  {
    dataStruct().pk1 = id;
  }

  void setPk2(int id)
  {
    dataStruct().pk2 = id;
  }

  void setPk3(int id)
  {
    dataStruct().pk3 = id;
  }

  void setPk4(int id)
  {
    dataStruct().pk4 = id;
  }
};


/*
 * Tests
 */

void PrimaryKeyTest::fieldTest()
{
  PrimaryKeyField articlePkField(0, QVariant::LongLong);
  QCOMPARE(articlePkField.fieldIndex(), 0);
  QCOMPARE(articlePkField.fieldType(), QMetaType::LongLong);
  QCOMPARE(articlePkField.fieldName<ArticleDef>(), QString("id"));

  constexpr PrimaryKeyField linkStartPkField(1, QVariant::LongLong);
  QCOMPARE(linkStartPkField.fieldIndex(), 1);
  QCOMPARE(linkStartPkField.fieldType(), QMetaType::LongLong);
  QCOMPARE(linkStartPkField.fieldName<LinkDef>(), QString("startId"));

  PrimaryKeyField linkEndPkField(2, QVariant::LongLong);
  QCOMPARE(linkEndPkField.fieldIndex(), 2);
  QCOMPARE(linkEndPkField.fieldType(), QMetaType::LongLong);
  QCOMPARE(linkEndPkField.fieldName<LinkDef>(), QString("endId"));
}

void PrimaryKeyTest::primaryKeyFromEntityTest()
{
  auto articlePk = PrimaryKey::fromEntity<ArticleEntity>();
  QCOMPARE(articlePk.fieldCount(), 1);
  QCOMPARE(articlePk.fieldAt(0).fieldIndex(), 0);
  QCOMPARE(articlePk.fieldAt(0).fieldType(), QMetaType::LongLong);
  QCOMPARE(articlePk.fieldAt(0).fieldName<ArticleDef>(), QString("id"));
  QCOMPARE(articlePk.toFieldNameList<ArticleDef>(), QStringList({"id"}));

  auto linkPk = PrimaryKey::fromEntity<LinkEntity>();
  QCOMPARE(linkPk.fieldCount(), 2);
  QCOMPARE(linkPk.fieldAt(0).fieldIndex(), 1);
  QCOMPARE(linkPk.fieldAt(0).fieldType(), QMetaType::LongLong);
  QCOMPARE(linkPk.fieldAt(0).fieldName<LinkDef>(), QString("startId"));
  QCOMPARE(linkPk.fieldAt(1).fieldIndex(), 2);
  QCOMPARE(linkPk.fieldAt(1).fieldType(), QMetaType::LongLong);
  QCOMPARE(linkPk.fieldAt(1).fieldName<LinkDef>(), QString("endId"));
  QCOMPARE(linkPk.toFieldNameList<LinkDef>(), QStringList({"startId","endId"}));

  const auto threePk = PrimaryKey::fromEntity<ThreePkEntity>();
  QCOMPARE(threePk.fieldCount(), 3);
  QCOMPARE(threePk.fieldAt(0).fieldIndex(), 1);
  QCOMPARE(threePk.fieldAt(0).fieldType(), QMetaType::Int);
  QCOMPARE(threePk.fieldAt(0).fieldName<ThreePkDef>(), QString("pk1"));
  QCOMPARE(threePk.fieldAt(1).fieldIndex(), 3);
  QCOMPARE(threePk.fieldAt(1).fieldType(), QMetaType::Int);
  QCOMPARE(threePk.fieldAt(1).fieldName<ThreePkDef>(), QString("pk2"));
  QCOMPARE(threePk.fieldAt(2).fieldIndex(), 4);
  QCOMPARE(threePk.fieldAt(2).fieldType(), QMetaType::Int);
  QCOMPARE(threePk.fieldAt(2).fieldName<ThreePkDef>(), QString("pk3"));

  const auto fourPk = PrimaryKey::fromEntity<FourPkEntity>();
  QCOMPARE(fourPk.fieldCount(), 4);
  QCOMPARE(fourPk.fieldAt(0).fieldIndex(), 1);
  QCOMPARE(fourPk.fieldAt(0).fieldType(), QMetaType::Int);
  QCOMPARE(fourPk.fieldAt(0).fieldName<FourPkDef>(), QString("pk1"));
  QCOMPARE(fourPk.fieldAt(1).fieldIndex(), 3);
  QCOMPARE(fourPk.fieldAt(1).fieldType(), QMetaType::Int);
  QCOMPARE(fourPk.fieldAt(1).fieldName<FourPkDef>(), QString("pk2"));
  QCOMPARE(fourPk.fieldAt(2).fieldIndex(), 4);
  QCOMPARE(fourPk.fieldAt(2).fieldType(), QMetaType::Int);
  QCOMPARE(fourPk.fieldAt(2).fieldName<FourPkDef>(), QString("pk3"));
  QCOMPARE(fourPk.fieldAt(3).fieldIndex(), 6);
  QCOMPARE(fourPk.fieldAt(3).fieldType(), QMetaType::Int);
  QCOMPARE(fourPk.fieldAt(3).fieldName<FourPkDef>(), QString("pk4"));
}

void PrimaryKeyTest::primaryKeyRecordBasicTest()
{
  PrimaryKeyRecord pkr;
  QCOMPARE(pkr.fieldCount(), 0);
  QVERIFY(pkr.isNull());

  pkr.addField( PrimaryKeyRecordField(2, "A") );
  QCOMPARE(pkr.fieldCount(), 1);
  QVERIFY(!pkr.isNull());
  QCOMPARE(pkr.fieldAt(0).fieldIndex(), 2);
  QCOMPARE(pkr.fieldAt(0).value(), QVariant("A"));
}

void PrimaryKeyTest::primaryKeyRecordFromEntityDataTest()
{
  ArticleData article;
  article.setId(11);
  auto articlePk = PrimaryKeyRecord::fromEntityData(article);
  QCOMPARE(articlePk.fieldCount(), 1);
  QCOMPARE(articlePk.fieldAt(0).fieldIndex(), 0);
  QCOMPARE(articlePk.fieldAt(0).value(), QVariant(11));

  LinkData link;
  link.setStartId(5);
  link.setEndId(6);
  auto linkPk = PrimaryKeyRecord::fromEntityData(link);
  QCOMPARE(linkPk.fieldCount(), 2);
  QCOMPARE(linkPk.fieldAt(0).fieldIndex(), 1);
  QCOMPARE(linkPk.fieldAt(0).value(), QVariant(5));
  QCOMPARE(linkPk.fieldAt(1).fieldIndex(), 2);
  QCOMPARE(linkPk.fieldAt(1).value(), QVariant(6));

  ThreePkData threePkData;
  threePkData.setPk1(11);
  threePkData.setPk2(12);
  threePkData.setPk3(13);
  auto threePk = PrimaryKeyRecord::fromEntityData(threePkData);
  QCOMPARE(threePk.fieldCount(), 3);
  QCOMPARE(threePk.fieldAt(0).fieldIndex(), 1);
  QCOMPARE(threePk.fieldAt(0).value(), QVariant(11));
  QCOMPARE(threePk.fieldAt(1).fieldIndex(), 3);
  QCOMPARE(threePk.fieldAt(1).value(), QVariant(12));
  QCOMPARE(threePk.fieldAt(2).fieldIndex(), 4);
  QCOMPARE(threePk.fieldAt(2).value(), QVariant(13));

  FourPkData fourPkData;
  fourPkData.setPk1(11);
  fourPkData.setPk2(12);
  fourPkData.setPk3(13);
  fourPkData.setPk4(14);
  const auto fourPk = PrimaryKeyRecord::fromEntityData(fourPkData);
  QCOMPARE(fourPk.fieldCount(), 4);
  QCOMPARE(fourPk.fieldAt(0).fieldIndex(), 1);
  QCOMPARE(fourPk.fieldAt(0).value(), QVariant(11));
  QCOMPARE(fourPk.fieldAt(1).fieldIndex(), 3);
  QCOMPARE(fourPk.fieldAt(1).value(), QVariant(12));
  QCOMPARE(fourPk.fieldAt(2).fieldIndex(), 4);
  QCOMPARE(fourPk.fieldAt(2).value(), QVariant(13));
  QCOMPARE(fourPk.fieldAt(3).fieldIndex(), 6);
  QCOMPARE(fourPk.fieldAt(3).value(), QVariant(14));
}

void PrimaryKeyTest::selectFieldFromPrimaryKeyRecordFieldTest()
{
  using Mdt::QueryExpression::EntityAndField;

  PrimaryKeyRecordField articleIdPkrf(0, 8);
  const auto articleIdSf = selectFieldFromPrimaryKeyRecordField<ArticleEntity>(articleIdPkrf);
  const auto articleIdEaf = boost::get<EntityAndField>(articleIdSf.internalVariant().internalVariant());
  QCOMPARE(articleIdEaf.entityAliasOrName(), QString("Article"));
  QCOMPARE(articleIdEaf.fieldAliasOrName(), QString("id"));

  PrimaryKeyRecordField linkStartIdPkrf(1, 21);
  const auto linkStartIdSf = selectFieldFromPrimaryKeyRecordField<LinkEntity>(linkStartIdPkrf);
  const auto linkStartIdEaf = boost::get<EntityAndField>(linkStartIdSf.internalVariant().internalVariant());
  QCOMPARE(linkStartIdEaf.entityAliasOrName(), QString("Link"));
  QCOMPARE(linkStartIdEaf.fieldAliasOrName(), QString("startId"));

  PrimaryKeyRecordField linkEndIdPkrf(2, 21);
  const auto linkEndIdSf = selectFieldFromPrimaryKeyRecordField<LinkEntity>(linkEndIdPkrf);
  const auto linkEndIdEaf = boost::get<EntityAndField>(linkEndIdSf.internalVariant().internalVariant());
  QCOMPARE(linkEndIdEaf.entityAliasOrName(), QString("Link"));
  QCOMPARE(linkEndIdEaf.fieldAliasOrName(), QString("endId"));
}

void PrimaryKeyTest::filterExpressionFromPrimaryKeyRecordTest()
{
  using Mdt::QueryExpression::traverseExpressionTree;

  Mdt::QueryExpression::FilterExpression filter;
  ExpressionToStringVisitor visitor;
  QString expectedString;

  ArticleData article;
  article.setId(33);
  auto articlePk = PrimaryKeyRecord::fromEntityData(article);
  filter = filterExpressionFromPrimaryKeyRecord<ArticleEntity>(articlePk);
  expectedString = "Article.id==33";
  visitor.clear();
  traverseExpressionTree(filter.internalTree(), visitor);
  QCOMPARE(visitor.toString(), expectedString);

  LinkData link;
  link.setStartId(5);
  link.setEndId(6);
  auto linkPk = PrimaryKeyRecord::fromEntityData(link);
  filter = filterExpressionFromPrimaryKeyRecord<LinkEntity>(linkPk);
  expectedString = "(Link.startId==5)&&(Link.endId==6)";
  visitor.clear();
  traverseExpressionTree(filter.internalTree(), visitor);
  QCOMPARE(visitor.toString(), expectedString);

  ThreePkData threePkData;
  threePkData.setPk1(11);
  threePkData.setPk2(12);
  threePkData.setPk3(13);
  auto threePk = PrimaryKeyRecord::fromEntityData(threePkData);
  filter = filterExpressionFromPrimaryKeyRecord<ThreePkEntity>(threePk);
  expectedString = "((ThreePk.pk1==11)&&(ThreePk.pk2==12))&&(ThreePk.pk3==13)";
  visitor.clear();
  traverseExpressionTree(filter.internalTree(), visitor);
  QCOMPARE(visitor.toString(), expectedString);

  FourPkData fourPkData;
  fourPkData.setPk1(11);
  fourPkData.setPk2(12);
  fourPkData.setPk3(13);
  fourPkData.setPk4(14);
  const auto fourPk = PrimaryKeyRecord::fromEntityData(fourPkData);
  filter = filterExpressionFromPrimaryKeyRecord<FourPkEntity>(fourPk);
  expectedString = "(((FourPk.pk1==11)&&(FourPk.pk2==12))&&(FourPk.pk3==13))&&(FourPk.pk4==14)";
  visitor.clear();
  traverseExpressionTree(filter.internalTree(), visitor);
  QCOMPARE(visitor.toString(), expectedString);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  PrimaryKeyTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
