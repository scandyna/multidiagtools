/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_ENTITY_RELATION_ALGORITHM_H
#define MDT_ENTITY_RELATION_ALGORITHM_H

#include "Relation.h"
#include "PrimaryKey.h"
#include "ForeignKey.h"
#include "TypeTraits/IsEntity.h"
#include "TypeTraits/IsEntityDef.h"
#include "TypeTraits/IsEntityFieldDef.h"
#include <QStringList>
#include <initializer_list>

namespace Mdt{ namespace Entity{

  namespace Impl{

    template<typename UnaryFunction,typename ForeignEntityField , typename ...ForeignEntityFields>
    void applyFunctorToRelationForeignField(UnaryFunction & f)
    {
      f(ForeignEntityField());
    }

    template<typename PrimaryEntity, typename ForeignEntity, typename ...ForeignEntityFields, typename UnaryFunction>
    void forEachRelationForeignField(const Relation<PrimaryEntity, ForeignEntity, ForeignEntityFields...> &, UnaryFunction & f)
    {
      (void)std::initializer_list<int>{ (applyFunctorToRelationForeignField<UnaryFunction, ForeignEntityFields>(f) ,0)... };
    }

  } // namespace Impl{

  /*! \brief Apply a function object for each foreign field in a relation
   *
   * \param f a function object with a signature equivalent to:
   *    \code
   *    template<typename Field>
   *    void f(const Field & field);
   *    \endcode
   */
  template<typename Relation, typename UnaryFunction>
  void forEachRelationForeignField(UnaryFunction & f)
  {
    Impl::forEachRelationForeignField(Relation(), f);
  }

  namespace Impl{

    /*! \internal Add a field name to a field name list
     */
    class AddFieldNameToFieldNameList
    {
    public:

      template<typename Field>
      void operator()(const Field &)
      {
        mFieldNameList.append( Field::fieldName() );
      }

      QStringList fieldNameList() const
      {
        return mFieldNameList;
      }

    private:

      QStringList mFieldNameList;
    };

  } // namespace Impl{

  /*! \brief Get the list of field names part of the primary key in a relation
   */
  template<typename EntityRelation>
  QStringList getPrimaryKeyFieldNameList()
  {
    using PrimaryEntity = typename EntityRelation::primary_entity;

    Impl::AddFieldNameToFieldNameList f;

    forEachPrimaryKeyField<PrimaryEntity>(f);

    return f.fieldNameList();
  }

  /*! \brief Get the list of field names part of the foreign key in a relation
   */
  template<typename EntityRelation>
  QStringList getForeignKeyFieldNameList()
  {
    Impl::AddFieldNameToFieldNameList f;

    forEachRelationForeignField<EntityRelation>(f);

    return f.fieldNameList();
  }

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_RELATION_ALGORITHM_H
