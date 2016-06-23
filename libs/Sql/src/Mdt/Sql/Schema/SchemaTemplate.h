/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_SQL_SCHEMA_SCHEMA_TEMPLATE_H
#define MDT_SQL_SCHEMA_SCHEMA_TEMPLATE_H

#include "Schema.h"

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Template class to define SQL schema
   *
   * SchemaTemplate is a wrapper of Schema that permit to define a schema.
   *
   * Example of TestSchema.h:
   * \include libs/Sql/tests/src/Schema/TestSchema.h
   *
   * TestSchema.cpp would look like this:
   * \include libs/Sql/tests/src/Schema/TestSchema.cpp
   *
   */
  template<typename Derived>
  class SchemaTemplate
  {
   public:

    /*! \brief Get schema instance
     */
    Schema toSchema() const
    {
      return pvSchema;
    }

   protected:

    /*! \brief Add a table
     */
    void addTable(const Table & table)
    {
      pvSchema.addTable(table);
    }

    /*! \brief Add a table
     */
    template<typename T>
    void addTable(const TableTemplate<T> & table)
    {
      pvSchema.addTable(table);
    }

    /*! \brief Add a view
     */
    void addView(const View & view)
    {
      pvSchema.addView(view);
    }

    /*! \brief Add a view
     */
    template<typename T>
    void addView(const ViewTemplate<T> & view)
    {
      pvSchema.addView(view);
    }

   private:

    Schema pvSchema;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_SCHEMA_TEMPLATE_H
