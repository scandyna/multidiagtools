/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_REFLECTION_RELATION_ALGORITHM_H
#define MDT_REFLECTION_RELATION_ALGORITHM_H

#include "Relation.h"
#include "FieldAlgorithm.h"
#include "StructAlgorithm.h"
#include "TypeTraits/IsRelation.h"
#include "Impl/AddFieldNameToList.h"
#include <QStringList>
#include <QLatin1String>
#include <boost/mpl/at.hpp>
#include <boost/mpl/for_each.hpp>

namespace Mdt{ namespace Reflection{

  /*! \brief Get the name given to the primary reflected struct in \a Relation
   *
   * \pre \a Relation must be a relation between two reflected structs
   */
  template<typename Relation>
  static constexpr const char *primaryNameFromRelation() noexcept
  {
    static_assert( TypeTraits::IsRelation<Relation>::value, "Relation must be a relation between two reflected structs" );

    return nameFromStructDef<typename Relation::primary_struct_def>();
  }

  /*! \brief Get the name given to the related reflected struct in \a Relation
   *
   * \pre \a Relation must be a relation between two reflected structs
   */
  template<typename Relation>
  static constexpr const char *relatedNameFromRelation() noexcept
  {
    static_assert( TypeTraits::IsRelation<Relation>::value, "Relation must be a relation between two reflected structs" );

    return nameFromStructDef<typename Relation::related_struct_def>();
  }

  namespace Impl{

    /*! \internal Used by forEachFieldPairInRelation()
    */
    template<typename Relation, typename BinaryFunction>
    struct CallRelationFieldPairFunction
    {
      CallRelationFieldPairFunction(BinaryFunction & f)
      : mF(f)
      {
      }

      template<typename PrimaryStructField>
      void operator()(PrimaryStructField)
      {
        constexpr auto fieldIndex = fieldIndexInMplSequence<PrimaryStructField, typename Relation::primary_struct_field_list>();
        using RelatedStructField = typename boost::mpl::at_c<typename Relation::related_struct_field_list, fieldIndex>::type;

        mF( PrimaryStructField(), RelatedStructField() );
      }

    private:

      BinaryFunction & mF;
    };

  } // namespace Impl{

  /*! \brief Apply a functor for each field pair in a relation
   *
   * \code
   * struct MyFunctor
   * {
   *   template<typename PrimaryStructField, typename RelatedStructField>
   *   void operator()(PrimaryStructField, RelatedStructField)
   *   {
   *   }
   * };
   * \endcode
   *
   * \pre \a Relation must be a relation between two reflected structs
   */
  template<typename Relation, typename BinaryFunction>
  void forEachFieldPairInRelation(BinaryFunction f)
  {
    static_assert( TypeTraits::IsRelation<Relation>::value, "Relation must be a relation between two reflected structs" );

    Impl::CallRelationFieldPairFunction<Relation, BinaryFunction> implF(f);
    boost::mpl::for_each< typename Relation::primary_struct_field_list>(implF);
  }

  /*! \brief Get a list of field names from the primary struct in a relation
   *
   * \pre \a Relation must be a relation between two reflected structs
   */
  template<typename Relation>
  QStringList primaryStructFieldNameListFromRelation()
  {
    static_assert( TypeTraits::IsRelation<Relation>::value, "Relation must be a relation between two reflected structs" );

    QStringList fieldNameList;
    Impl::AddFieldNameToList f(fieldNameList);

    boost::mpl::for_each< typename Relation::primary_struct_field_list>(f);

    return fieldNameList;
  }

  /*! \brief Get a list of field names from the related struct in a relation
   *
   * \pre \a Relation must be a relation between two reflected structs
   */
  template<typename Relation>
  QStringList relatedStructFieldNameListFromRelation()
  {
    static_assert( TypeTraits::IsRelation<Relation>::value, "Relation must be a relation between two reflected structs" );

    QStringList fieldNameList;
    Impl::AddFieldNameToList f(fieldNameList);

    boost::mpl::for_each< typename Relation::related_struct_field_list>(f);

    return fieldNameList;
  }

}} // namespace Mdt{ namespace Reflection{

#endif // #ifndef MDT_REFLECTION_RELATION_ALGORITHM_H
