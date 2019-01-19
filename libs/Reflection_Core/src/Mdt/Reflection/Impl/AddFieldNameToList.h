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
#ifndef MDT_REFLECTION_IMPL_ADD_FIELD_NAME_TO_LIST_H
#define MDT_REFLECTION_IMPL_ADD_FIELD_NAME_TO_LIST_H

#include "../FieldAlgorithm.h"
#include "../TypeTraits/IsField.h"
#include <QLatin1String>
#include <QStringList>

namespace Mdt{ namespace Reflection{ namespace Impl{

    struct AddFieldNameToList
    {
      AddFieldNameToList(QStringList & list)
      : mFieldNameList(list)
      {
      }

      template<typename Field>
      void operator()(Field)
      {
        static_assert( Mdt::Reflection::TypeTraits::IsField<Field>::value , "Field must be a field defined in a struct definition associated with a reflected struct" );

        mFieldNameList << QLatin1String( fieldName<Field>() );
      }

    private:

      QStringList & mFieldNameList;
    };

}}} // namespace Mdt{ namespace Reflection{ namespace Impl{

#endif // #ifndef MDT_REFLECTION_IMPL_ADD_FIELD_NAME_TO_LIST_H
