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
      return mView;
    }

    /*! \brief Get the name of the view
     */
    QString name() const
    {
      return mView.name();
    }

   protected:

    /*! \brief Set the name of the view
     */
    void setName(const QString & name)
    {
      mView.setName(name);
    }

    /*! \brief Set select operator
     */
    void setSelectOperator(SelectOperator op)
    {
      mView.setSelectOperator(op);
    }

    /*! \brief Add a field by specifying table and field
     *
     * \pre table must not be null
     * \pre field must not be null
     */
    void addField(const SelectTable & table, const FieldName & field , const QString & fieldAlias = QString())
    {
      mView.addField(table, field, fieldAlias);
    }

    /*! \brief Add a field by specifying field
     *
     * \pre field must not be null
     */
    void addField(const FieldName & field , const QString & fieldAlias = QString())
    {
      mView.addField(field, fieldAlias);
    }

    /*! \brief Add all fields for table (the * in SQL)
     *
     * \pre table must not be null
     */
    void addAllFields(const SelectTable & table)
    {
      mView.addAllFields(table);
    }

    /*! \brief Add all fields (the * in SQL, not table defined)
     */
    void addAllFields()
    {
      mView.addAllFields();
    }

    /*! \brief Add a raw SQL string to select a field
     *
     * \pre sql must not be a empty string
     */
    void addRawSqlFieldExpression(const QString & sql)
    {
      mView.addRawSqlFieldExpression(sql);
    }

    /*! \brief Set from table
     *
     * If a raw SQL string was set,
     *  it will be cleared.
     *
     * \pre No joined table must allready been set.
     * \pre table must not be null.
     */
    void setFromTable(const SelectTable & table)
    {
      mView.setFromTable(table);
    }

    /*! \brief Join a table
     *
     * Join table with expr as constraint
     *
     * \pre table must not be null
     * \pre expr must not be null
     * \pre From table must allready been set (see setFromTable()).
     */
    void joinTable(const SelectTable & table, const JoinConstraintExpression & expr)
    {
      mView.joinTable(table, expr);
    }

    /*! \brief Join a table automatically
     *
     * Will fetch table's foreign key list
     *  and fromTable's foreign key list about relation
     *  and also generate the apropriate join constraint.
     *
     * \pre table must not be null
     * \pre table or fromTable must have a relation defined by their foreign key
     * \pre From table must allready been set (see setFromTable()).
     */
    void joinTable(const SelectTable & table)
    {
      mView.joinTable(table);
    }

    /*! \brief Join a table automatically
     *
     * Will fetch table's foreign key list
     *  and constraintOnTable's foreign key list about relation
     *  and also generate the apropriate join constraint.
     *
     * \pre table must not be null
     * \pre constraintOnTable must not be null
     * \pre table or constraintOnTable must have a relation defined by their foreign key
     * \pre From table must allready been set (see setFromTable()).
     */
    void joinTable(const SelectTable & table, const SelectTable & constraintOnTable)
    {
      mView.joinTable(table, constraintOnTable);
    }

    /*! \brief Left join a table
     *
     * Join table with expr as constraint
     *
     * \pre table must not be null
     * \pre expr must not be null
     * \pre From table must allready been set (see setFromTable()).
     */
    void leftJoinTable(const SelectTable & table, const JoinConstraintExpression & expr)
    {
      mView.leftJoinTable(table, expr);
    }

    /*! \brief Left join a table automatically
     *
     * Will fetch table's foreign key list
     *  and fromTable's foreign key list about relation
     *  and also generate the apropriate join constraint.
     *
     * \pre table must not be null
     * \pre table or fromTable must have a relation defined by their foreign key
     * \pre From table must allready been set (see setFromTable()).
     */
    void leftJoinTable(const SelectTable & table)
    {
      mView.leftJoinTable(table);
    }

    /*! \brief Left join a table automatically
     *
     * Will fetch table's foreign key list
     *  and constraintOnTable's foreign key list about relation
     *  and also generate the apropriate join constraint.
     *
     * \pre table must not be null
     * \pre constraintOnTable must not be null
     * \pre table or constraintOnTable must have a relation defined by their foreign key
     * \pre From table must allready been set (see setFromTable()).
     */
    void leftJoinTable(const SelectTable & table, const SelectTable & constraintOnTable)
    {
      mView.leftJoinTable(table, constraintOnTable);
    }

   private:

    View mView;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{
#endif // #ifndef MDT_SQL_SCHEMA_VIEW_TEMPLATE_H
