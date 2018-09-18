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
#ifndef MDT_ENTITY_IMPL_ADD_FIELD_NAME_TO_FIELD_NAME_LIST_H
#define MDT_ENTITY_IMPL_ADD_FIELD_NAME_TO_FIELD_NAME_LIST_H

#include "../TypeTraits/IsEntityFieldDef.h"
#include "MdtEntity_CoreExport.h"
#include <QStringList>

namespace Mdt{ namespace Entity{ namespace Impl{

  /*! \internal Add a field name to a field name list
   */
  class MDT_ENTITY_CORE_EXPORT AddFieldNameToFieldNameList
  {
  public:

    template<typename Field>
    void operator()(const Field &)
    {
      static_assert( TypeTraits::IsEntityFieldDef<Field>::value, "Field must be a entity field" );

      mFieldNameList.append( Field::fieldName() );
    }

    QStringList fieldNameList() const
    {
      return mFieldNameList;
    }

  private:

    QStringList mFieldNameList;
  };

}}} // namespace Mdt{ namespace Entity{ namespace Impl{

#endif // #ifndef MDT_ENTITY_IMPL_ADD_FIELD_NAME_TO_FIELD_NAME_LIST_H
