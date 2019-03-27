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
#ifndef MDT_QUERY_EXPRESSION_REFLECTION_PRIMARY_KEY_RECORD_ALGORITHM_H
#define MDT_QUERY_EXPRESSION_REFLECTION_PRIMARY_KEY_RECORD_ALGORITHM_H

#include "Mdt/Reflection/FieldAlgorithm.h"
#include "Mdt/Reflection/StructAlgorithm.h"
#include "Mdt/Reflection/PrimaryKeyAlgorithm.h"
#include "Mdt/QueryExpression/FilterExpression.h"
#include "Mdt/QueryExpression/ExpressionTree.h"
#include "Mdt/QueryExpression/EntityAndField.h"
#include "Mdt/QueryExpression/ComparisonOperator.h"
#include "Mdt/QueryExpression/LogicalOperator.h"
#include "Mdt/QueryExpression/QueryEntity.h"
#include "Mdt/Reflection/PrimaryKeyAlgorithm.h"
#include "Mdt/Reflection/TypeTraits/IsPrimaryKeyClass.h"
#include "Mdt/Reflection/TypeTraits/IsPrimaryKeyRecord.h"
#include "Mdt/Reflection/TypeTraits/IsReflectedStructAssociatedWithPrimaryKey.h"
#include <QString>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Get a filter expression from a primary key id value referring to the primary key of a struct definition
   *
   * \code
   * using PersonPrimaryKey = Mdt::Reflection::AutoIncrementIdPrimaryKey<PersonDef::id>;
   *
   * FilterExpression filter = filterExpressionFromIdPrimaryKeyValue<PersonPrimaryKey>(25);
   * \endcode
   *
   * Above code will return a expression like:
   * \code
   * Person.id == 25
   * \endcode
   *
   * \pre \a PrimaryKey must be primary key class for a reflected struct
   * \pre \a PrimaryKey must contain exactly 1 field
   */
  template<typename PrimaryKey, typename T>
  FilterExpression filterExpressionFromIdPrimaryKeyValue(const T & value)
  {
    static_assert( Mdt::Reflection::TypeTraits::IsPrimaryKeyClass<PrimaryKey>::value,
                   "PrimaryKey must be a primary key class for a reflected struct" );
    static_assert( Mdt::Reflection::primaryKeyFieldCount<PrimaryKey>() == 1,
                   "PrimaryKey must contain exactly 1 field" );

    FilterExpression filter;
    ExpressionTree tree;
    QueryEntity entity( Mdt::Reflection::nameFromPrimaryKeyQString<PrimaryKey>() );

    tree.addNode( EntityAndField(entity, Mdt::Reflection::fieldNameFromSingleFieldPrimaryKeyQString<PrimaryKey>()),
                  ComparisonOperator::Equal, value );
    filter.setFilterExpressionTree(tree);

    return filter;
  }

  namespace Impl{

    /*! \internal
     *
     *              (AND)
     *             /     \
     *         (AND)      (==)
     *        /    \      /  \
     *     (==)     (==) (FC) (c)
     *     /  \     /  \
     *  (FA)  (a) (FB) (b)
     */
    template<typename PrimaryKeyRecord>
    class AddPrimaryKeyRecordFieldToFilterExpressionTree
    {
      using primary_key = typename PrimaryKeyRecord::primary_key;

     public:

      AddPrimaryKeyRecordFieldToFilterExpressionTree(const PrimaryKeyRecord & pkr, ExpressionTree & tree)
       : mPkr(pkr),
         mTree(tree),
         mEntity( Mdt::Reflection::nameFromPrimaryKeyQString<primary_key>() )
      {
      }

      template<typename Field>
      void operator()(Field)
      {
        if(!mFirstNodeAdded){
          mFirstNodeAdded = true;
          mLeftVertex = mTree.addNode( EntityAndField(mEntity, Mdt::Reflection::fieldNameQString<Field>() ),
                                       ComparisonOperator::Equal, mPkr.template variantValue<Field>() );
        }else{
          mRightVertex = mTree.addNode( EntityAndField(mEntity, Mdt::Reflection::fieldNameQString<Field>() ),
                                       ComparisonOperator::Equal, mPkr.template variantValue<Field>() );
          mLeftVertex = mTree.addNode(mLeftVertex, LogicalOperator::And, mRightVertex);
        }
      }

     private:

      const PrimaryKeyRecord & mPkr;
      ExpressionTree & mTree;
      QueryEntity mEntity;
      bool mFirstNodeAdded = false;
      ExpressionTreeVertex mLeftVertex;
      ExpressionTreeVertex mRightVertex;

    };

  } // namespace Impl{

  /*! \brief Get a filter expression from a primary key record referring to the primary key of a struct definition
   *
   * \code
   * using CableLinkPrimaryKey = Mdt::Reflection::PrimaryKey<CableLinkDef::startConnectionId, CableLinkDef::endConnectionId>;
   * using CableLinkIdRecord = Mdt::Reflection::PrimaryKeyRecord<CableLinkPrimaryKey>;
   *
   * CableLinkIdRecord linkIdRecord;
   * linkIdRecord.setValue<CableLinkDef::startConnectionId>(12);
   * linkIdRecord.setValue<CableLinkDef::endConnectionId>(30);
   *
   * FilterExpression filter = filterExpressionFromPrimaryKeyRecord(linkIdRecord);
   * \endcode
   *
   * Above code wil return a expression like:
   * \code
   * (Link.startConnectionId == 12) && (Link.endConnectionId == 30)
   * \endcode
   *
   * \pre \a PrimaryKeyRecord must be a primary key record associated with a primary key for a reflected struct
   */
  template<typename PrimaryKeyRecord>
  FilterExpression filterExpressionFromPrimaryKeyRecord(const PrimaryKeyRecord & pkr)
  {
    static_assert( Mdt::Reflection::TypeTraits::IsPrimaryKeyRecord<PrimaryKeyRecord>::value,
                   "PrimaryKeyRecord must be a primary key record associated with a primary key for a reflected struct" );

    using primary_key = typename PrimaryKeyRecord::primary_key;

    FilterExpression filter;
    ExpressionTree tree;
    Impl::AddPrimaryKeyRecordFieldToFilterExpressionTree<PrimaryKeyRecord> f(pkr, tree);

    Mdt::Reflection::forEachPrimaryKeyField<primary_key>(f);
    filter.setFilterExpressionTree(tree);

    return filter;
  }

  /*! \brief Get a filter expression from the values part of a primary key in a reflected struct
   *
   * \code
   * using PersonPrimaryKey = Mdt::Reflection::PrimaryKey<PersonDef::id>;
   *
   * PersonDataStruct person;
   * person.id = 25;
   *
   * FilterExpression filter = filterExpressionFromStructPrimaryKey<PersonPrimaryKey>(person);
   * \endcode
   *
   * Above code will return a expression like:
   * \code
   * Person.id == 25
   * \endcode
   *
   * It is also possible to have more than 1 field part of the primary key:
   * \code
   * using CableLinkPrimaryKey = Mdt::Reflection::PrimaryKey<CableLinkDef::startConnectionId, CableLinkDef::endConnectionId>;
   *
   * CableLinkDataStruct link;
   * link.startConnectionId = 12;
   * link.endConnectionId = 30;
   *
   * FilterExpression filter = filterExpressionFromStructPrimaryKey<CableLinkPrimaryKey>(link);
   * \endcode
   *
   * Above code wil return a expression like:
   * \code
   * (Link.startConnectionId == 12) && (Link.endConnectionId == 30)
   * \endcode
   *
   * \pre \a PrimaryKey must be a primary key class for a reflected struct
   * \pre \a Struct must be a reflected struct associated with \a PrimaryKey
   */
  template<typename PrimaryKey, typename Struct>
  FilterExpression filterExpressionFromStructPrimaryKey(const Struct & s)
  {
    static_assert( Mdt::Reflection::TypeTraits::IsPrimaryKeyClass<PrimaryKey>::value,
                   "PrimaryKey must be a primary key class for a reflected struct" );
    static_assert( Mdt::Reflection::TypeTraits::IsReflectedStructAssociatedWithPrimaryKey<Struct, PrimaryKey>::value,
                   "Struct must be a reflected struct associated with a PrimaryKey" );

    

  }

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_REFLECTION_PRIMARY_KEY_RECORD_ALGORITHM_H
