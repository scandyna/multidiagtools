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

  /*! \brief Base class to implement a backend specific select query
   *
   * \code
   * class MySqlSelectQuery : public Mdt::QueryExpression::AbstractSelectQuery
   * {
   *  public:
   *
   *   MySqlSelectQuery(QObject *parent = nullptr);
   * };
   * \endcode
   *
   * \sa SelectQuery
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
     */
    virtual int fieldIndex(const EntityAndField & field) const = 0;

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

    /*! \brief Set last error
     */
    void setLastError(const Mdt::Error & error);

   private:

    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_ABSTRACT_SELECT_QUERY_H
