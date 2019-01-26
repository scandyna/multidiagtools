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
#ifndef MDT_SQL_REFLECTION_SCHEMA_REFLECTION_FOREIGN_KEY_ALGORITHM_H
#define MDT_SQL_REFLECTION_SCHEMA_REFLECTION_FOREIGN_KEY_ALGORITHM_H

#include "Mdt/Sql/Schema/ForeignKey.h"
#include "Mdt/Sql/Schema/ForeignField.h"
#include "Mdt/Sql/Schema/ForeignKeySettings.h"
#include "Mdt/Reflection/RelationAlgorithm.h"
#include "Mdt/Reflection/FieldAlgorithm.h"
#include "Mdt/Reflection/TypeTraits/IsRelation.h"
#include <QString>
#include <QLatin1String>
#include <type_traits>

namespace Mdt{ namespace Sql{ namespace Schema{ namespace Reflection{

  namespace Impl{

    /*! \internal
     */
    struct AddFieldPairToForeignKey
    {
      AddFieldPairToForeignKey(ForeignKey & fk)
       : mFk(fk)
      {
      }

      template<typename PrimaryStructField, typename RelatedStructField>
      void operator()(PrimaryStructField, RelatedStructField)
      {
        mFk.addFieldNames(
          QLatin1String(Mdt::Reflection::fieldName<RelatedStructField>()),
          ForeignField(QLatin1String(Mdt::Reflection::fieldName<PrimaryStructField>()))
        );
      }

     private:

      ForeignKey & mFk;
    };

  } // namespace Impl{

  /*! \brief Get a SQL foreign key from a relation between 2 reflected structs
   *
   * \pre \a Relation must be a relation between two reflected structs
   */
  template<typename Relation>
  ForeignKey foreignKeyFromRelation(const ForeignKeySettings & fkSettings)
  {
    static_assert( Mdt::Reflection::TypeTraits::IsRelation<Relation>::value, "Relation must be a relation between two reflected structs" );

    ForeignKey fk;
    Impl::AddFieldPairToForeignKey f(fk);

    fk.setTableName( QLatin1String( Mdt::Reflection::relatedNameFromRelation<Relation>() ) );
    fk.setForeignTableName( QLatin1String( Mdt::Reflection::primaryNameFromRelation<Relation>() ) );
    Mdt::Reflection::forEachFieldPairInRelation<Relation>(f);
    fk.setSettings(fkSettings);

    return fk;
  }

}}}} // namespace Mdt{ namespace Sql{ namespace Schema{ namespace Reflection{


#endif // #ifndef MDT_SQL_REFLECTION_SCHEMA_REFLECTION_FOREIGN_KEY_ALGORITHM_H
