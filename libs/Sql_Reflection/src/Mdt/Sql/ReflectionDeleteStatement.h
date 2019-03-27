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
#ifndef MDT_SQL_REFLECTION_DELETE_STATEMENT_H
#define MDT_SQL_REFLECTION_DELETE_STATEMENT_H

#include "Mdt/Sql/DeleteStatement.h"
// #include "PrimaryKeyRecordAlgorithm.h"
#include "Mdt/Reflection/StructAlgorithm.h"
#include "Mdt/Reflection/TypeTraits/IsStructDef.h"
// #include "Mdt/Reflection/TypeTraits/IsStructDefAssociatedWithReflectedStruct.h"
// #include "Mdt/Reflection/TypeTraits/IsPrimaryKeyClass.h"
// #include "Mdt/Sql/DeleteStatement.h"
// #include "Mdt/Sql/PrimaryKeyRecord.h"
// #include "Mdt/Sql/FieldName.h"
#include <QString>
// #include <QLatin1String>
// #include <type_traits>

namespace Mdt{ namespace Sql{

  /*! \brief Represents a SQL DELETE statement with the use of the reflection library
   *
   * \pre \a StructDef must be a struct definition assiocated with a reflected struct
   *
   * \sa DeleteStatement
   * \sa DeleteQuery
   */
  template<typename StructDef>
  class ReflectionDeleteStatement : public DeleteStatement
  {
    static_assert( Mdt::Reflection::TypeTraits::IsStructDef<StructDef>::value,
                   "StructDef must be a struct definition assiocated with a reflected struct" );

    using BaseClass = DeleteStatement;

   public:

    using BaseClass::BaseClass;

    /*! \brief Construct a delete statement
     */
    ReflectionDeleteStatement()
     : BaseClass()
    {
      setTableName( Mdt::Reflection::nameFromStructDefQString<StructDef>() );
    }
  };

}} // namespace Mdt{ namespace Sql{


#endif // #ifndef MDT_SQL_REFLECTION_DELETE_STATEMENT_H
