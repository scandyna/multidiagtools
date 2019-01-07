/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#include "TypeTraitsTest.h"
#include "Mdt/Reflection/TypeTraits/IsField.h"
#include "Mdt/Reflection/TypeTraits/IsStructDef.h"
#include "Mdt/Reflection/TypeTraits/IsFieldAssociatedWithReflectedStruct.h"
#include "Mdt/Reflection/ReflectStruct.h"
#include "Mdt/Reflection/PrimaryKey.h"
#include <QString>

using namespace Mdt::Reflection;
using namespace Mdt::Reflection::TypeTraits;

/*
 * Entities
 */

struct ArticleDataStruct
{
  qlonglong id;
  QString description;
};

MDT_REFLECT_STRUCT(
  (ArticleDataStruct),
  Article,
  (id),
  (description, FieldFlag::IsRequired, FieldMaxLength(250))
)

struct ArticleDetailDataStruct
{
  qlonglong id;
  qlonglong articleId;
};

MDT_REFLECT_STRUCT(
  (ArticleDetailDataStruct),
  ArticleDetail,
  (id),
  (articleId)
)

// using ArticleDetailRelation = Mdt::Reflection::Relation<ArticleEntity, ArticleDetailEntity, ArticleDetailDef::articleIdField>;

struct ArticleTypeDataStruct
{
  QString code;
  QString label;
};

MDT_REFLECT_STRUCT(
  (ArticleTypeDataStruct),
  ArticleType,
  (code),
  (label)
)

/*
 * IsStructDef compile time tests
 */

static_assert(!IsStructDef<ArticleDataStruct>::value, "" );
static_assert( IsStructDef<ArticleDef>::value, "" );
static_assert(!IsStructDef<ArticleDef::id>::value , "" );
static_assert(!IsStructDef<ArticleDef::description>::value , "" );
// static_assert(!IsStructDef<ArticleDetailRelation>::value, "" );
static_assert(!IsStructDef<int>::value, "" );
static_assert(!IsStructDef<QString>::value, "" );

/*
 * IsField compile time tests
 */

static_assert( IsField<ArticleDef::id>::value , "" );
static_assert( IsField<ArticleDef::description>::value , "" );
static_assert(!IsField<ArticleDataStruct>::value , "" );
static_assert(!IsField<ArticleDef>::value , "" );
// static_assert(!IsField<ArticleDetailRelation>::value, "" );
static_assert(!IsField<int>::value , "" );
static_assert(!IsField<QString>::value , "" );

/*
 * IsFieldAssociatedWithReflectedStruct compile time tests
 */

static_assert( IsFieldAssociatedWithReflectedStruct<ArticleDataStruct, ArticleDef::id>::value , "" );
static_assert( !IsFieldAssociatedWithReflectedStruct<ArticleDef, ArticleDef::id>::value , "" );
static_assert( !IsFieldAssociatedWithReflectedStruct<QString, ArticleDef::id>::value , "" );
static_assert( !IsFieldAssociatedWithReflectedStruct<int, ArticleDef::id>::value , "" );

/*
 * IsPrimaryKey compile time tests
 */

/*
 * IsRelation compile time tests
 */

// static_assert(!IsRelation<ArticleDef::idField>::value , "" );
// static_assert(!IsRelation<ArticleDef::descriptionField>::value , "" );
// static_assert(!IsRelation<ArticleDataStruct>::value , "" );
// static_assert(!IsRelation<ArticleDef>::value , "" );
// static_assert(!IsRelation<ArticleEntity>::value, "" );
// static_assert( IsRelation<ArticleDetailRelation>::value, "" );
// static_assert(!IsRelation<int>::value , "" );
// static_assert(!IsRelation<QString>::value , "" );

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  TypeTraitsTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
