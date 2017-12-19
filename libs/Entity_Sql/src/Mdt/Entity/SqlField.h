/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_ENTITY_SQL_FIELD_H
#define MDT_ENTITY_SQL_FIELD_H

#include "Mdt/Sql/Schema/Field.h"
#include "MdtEntity_SqlExport.h"
#include <QString>

namespace Mdt{ namespace Entity{

  /*! \brief Helper to reflect a entity fielf to a SQL field
   */
  class MDT_ENTITY_SQL_EXPORT SqlField
  {
   public:

    /*! \brief Create a SQL field from \a entityField
     *
     * \tparam Field Entity field. Must provide fieldName() method:
     * \code
     * static QString fieldName();
     * \endcode
     *
     * \pre entityField must have a non empty fieldName
     */
    template<typename Field>
    static Mdt::Sql::Schema::Field fromEntityField(const Field & entityField)
    {
      Q_ASSERT(!entityField.fieldName().isEmpty());
      return fromEntityField(entityField.fieldName());
    }

   private:

    static Mdt::Sql::Schema::Field fromEntityField(const QString & entityFieldName);
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_SQL_FIELD_H
