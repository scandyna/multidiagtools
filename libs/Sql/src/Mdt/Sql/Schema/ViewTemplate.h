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
#ifndef MDT_SQL_SCHEMA_VIEW_TEMPLATE_H
#define MDT_SQL_SCHEMA_VIEW_TEMPLATE_H

#include "View.h"

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Template class to define SQL schema view
   *
   * ViewTemplate is a wrapper of View that permit to define a schema view.
   *
   * Example of ClientAddressView.h:
   * \include libs/Sql/tests/src/Schema/ClientAddressView.h
   *
   * ClientAddressView.cpp would look like this:
   * \include libs/Sql/tests/src/Schema/ClientAddressView.cpp
   *
   */
  template<typename Derived>
  class ViewTemplate
  {
   public:

    /*! \brief Get View instance
     */
    View toView() const
    {
      return pvView;
    }

    /*! \brief Get the name of the view
     */
    QString name() const
    {
      return pvView.name();
    }

   protected:

    /*! \brief Set the name of the view
     */
    void setName(const QString & name)
    {
      pvView.setName(name);
    }

    /*! \brief Set select operator
     */
    void setSelectOperator(SelectOperator op)
    {
      pvView.setSelectOperator(op);
    }

    /*! \brief Set table
     */
    void setTable(const ViewTable & table)
    {
      pvView.setTable(table);
    }

    /*! \brief Add a field to select
     */
    void addSelectField(const ViewTable & table, const Field & field, const QString & fieldAlias = QString())
    {
      pvView.addSelectField(table, field, fieldAlias);
    }

    /*! \brief Add a field to select
     */
    void addSelectField(const ViewTable & table, const FieldName & fieldName, const QString & fieldAlias = QString())
    {
      pvView.addSelectField(table, fieldName, fieldAlias);
    }

    /*! \brief Add a field to select
     */
    void addSelectField(const ViewTable & table, const AutoIncrementPrimaryKey & pk, const QString & fieldAlias = QString())
    {
      pvView.addSelectField(table, pk, fieldAlias);
    }

    /*! \brief Add a field to select
     */
    void addSelectField(const ViewTable & table, const SingleFieldPrimaryKey & pk, const QString & fieldAlias = QString())
    {
      pvView.addSelectField(table, pk, fieldAlias);
    }

    /*! \brief Add a select all field for table
     */
    void addSelectAllFields(const ViewTable & table)
    {
      pvView.addSelectAllFields(table);
    }

    /*! \brief Add a JOIN clause
     */
    void addJoinClause(const JoinClause & join)
    {
      pvView.addJoinClause(join);
    }

   private:

    View pvView;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{
#endif // #ifndef MDT_SQL_SCHEMA_VIEW_TEMPLATE_H
