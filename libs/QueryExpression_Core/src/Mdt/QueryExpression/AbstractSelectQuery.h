/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_QUERY_EXPRESSION_ABSTRACT_SELECT_QUERY_H
#define MDT_QUERY_EXPRESSION_ABSTRACT_SELECT_QUERY_H

#include "SelectStatement.h"
#include "FieldName.h"
#include "SelectField.h"
#include "Mdt/Error.h"
#include "MdtQueryExpression_CoreExport.h"
#include <QObject>
#include <QVariant>

// #include <QDebug>

namespace Mdt{ namespace QueryExpression{

  class CallSelectQueryImplFieldIndex;

  /*! \brief Base class to implement a backend specific select query
   *
   * Using a select query could look like:
   * \code
   * using namespace Mdt::QueryExpression;
   *
   * PersonList personList;
   *
   * SelectField age( FieldName("age"), "A" );
   *
   * SelectStatement stm;
   * stm.setEntityName("Person");
   * stm.addField( FieldName("name") );
   * stm.addField(age);
   * stm.addField( FieldName("remarks") );
   * stm.setFilter( age > 29 );
   *
   * QSqlDatabase db; // See Qt documentation and Mdt::Sql to setup db
   * SqlSelectQuery query;
   * query.setDatabase(db);
   * if(!query.exec(stm)){
   *   // Error handling
   * }
   * while(query.next()){
   *   // PersonList::append() takes name, age, remarks. It accpets QVariant and handles null values
   *   personList.append( query.value(0), query.value(1), query.value(2) );
   * }
   * \endcode
   *
   * Notice that we get the value at column 0, 1 and 2 from the internal record in the query.
   *  It could be better to specify those indexes by names.
   *  fieldIndex() cane help to fetch them.
   *  This method only accepts SelectField, so it can assume a unique index for each field
   *  in a query that refers to multiple entities.
   *  Here is the new implementation:
   * \code
   * using namespace Mdt::QueryExpression;
   *
   * PersonList personList;
   *
   * SelectField age( FieldName("age"), "A" );
   * SelectField name( FieldName("name") );
   * SelectField remarks( FieldName("remarks") );
   *
   * SelectStatement stm;
   * stm.setEntityName("Person");
   * stm.addField(name);
   * stm.addField(age);
   * stm.addField(remarks);
   * stm.setFilter( age > 29 );
   *
   * QSqlDatabase db; // See Qt documentation and Mdt::Sql to setup db
   * SqlSelectQuery query;
   * query.setDatabase(db);
   * if(!query.exec(stm)){
   *   // Error handling
   * }
   * const auto nameIndex = query.fieldIndex(name);
   * const auto ageIndex = query.fieldIndex(age);
   * const auto remarksIndex = query.fieldIndex(remarks);
   * while(query.next()){
   *   // PersonList::append() takes name, age, remarks. It accpets QVariant and handles null values
   *   personList.append( query.value(nameIndex), query.value(ageIndex), query.value(remarksIndex) );
   * }
   * \endcode
   *
   * In above examples, instances of the backend specific query was used.
   *  It may be better to not depend, for example in SQL, in the business logic.
   *  This can be done using a factory.
   *  See AbstractSelectQueryFactory documentation for that.
   *
   * To implement a specific select query:
   * \code
   * class MyBackendSelectQuery : public Mdt::QueryExpression::AbstractSelectQuery
   * {
   *  public:
   *
   *   MyBackendSelectQuery(QObject *parent = nullptr);
   *
   *   void setConfiguration(const MyConfiguration & config);
   *   bool exec(const SelectStatement & statement) override;
   *   bool next() override;
   *   int fieldCount() const override;
   *   int fieldIndex(const EntityAndField & field) const override;
   *   QVariant value(int fieldIndex) const override;
   * };
   * \endcode
   *
   * \sa AbstractSelectQueryFactory
   * \sa SqlSelectQuery
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT AbstractSelectQuery : public QObject
  {
   public:

    /*! \brief Constructor
     */
    explicit AbstractSelectQuery(QObject *parent = nullptr)
     : QObject(parent)
    {
    }

    AbstractSelectQuery(const AbstractSelectQuery &) = delete;
    AbstractSelectQuery & operator=(const AbstractSelectQuery &) = delete;
    AbstractSelectQuery(AbstractSelectQuery &&) = delete;
    AbstractSelectQuery & operator=(AbstractSelectQuery &&) = delete;

    /*! \brief Execute \a statement
     *
     * \todo should add maxRows argument
     */
    virtual bool exec(const SelectStatement & statement) = 0;

    /*! \brief Get the next record, if avaliable, and position this query to that record
     */
    virtual bool next() = 0;

    /*! \brief Get count of field for the last executed statement
     */
    virtual int fieldCount() const = 0;

    /*! \brief Get the field index of \a field
     *
     * Returns the index of \a field if it exists,
     *  otherwise -1
     *  If more than 1 field matches, the first one is returned.
     *
     * \pre \a field must contain a EntityAndField
     */
    int fieldIndex(const SelectField & field) const;

    /*! \brief Get the value for \a fieldIndex from the current record
     *
     * \pre \a fieldIndex must be in a valid range ( 0 <= \a fieldIndex < fieldCount() ).
     */
    virtual QVariant value(int fieldIndex) const = 0;

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

   protected:

    /*! \brief Get the field index of \a field
     *
     * Returns the index of \a field if it exists,
     *  otherwise -1
     *  If more than 1 field matches, the first one is returned.
     */
    virtual int fieldIndexImpl(const EntityAndField & field) const = 0;

    /*! \brief Set last error
     */
    void setLastError(const Mdt::Error & error);

   private:

    friend class CallSelectQueryImplFieldIndex;

    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_ABSTRACT_SELECT_QUERY_H
