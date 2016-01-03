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
#ifndef MDT_SQL_COPIER_UNIQUE_INSERT_EXPRESSION_H
#define MDT_SQL_COPIER_UNIQUE_INSERT_EXPRESSION_H

#include "AbstractTableMappingItem.h"
#include "mdtSqlWhereOperator.h"

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Container for UniqueInsertExpression
   */
  struct UniqueInsertMatchExpressionItem
  {
    /*! \brief Field index for matching value in source
     */
    int sourceValueFieldIndex;

    /*! \brief Field index to match in destination
     */
    int destinationFieldIndex;

    /*! \brief WHERE operator with previous match item
     */
    mdtSqlWhereOperator operatorWithPrevious;

    /*! \brief Construct a match item
     */
    UniqueInsertMatchExpressionItem(int sourceValueFieldIndex, int destinationFieldIndex)
     : sourceValueFieldIndex(sourceValueFieldIndex),
       destinationFieldIndex(destinationFieldIndex)
    {
    }

    /*! \brief Construct a match item
     */
    UniqueInsertMatchExpressionItem(const mdtSqlWhereOperator & op, int sourceValueFieldIndex, int destinationFieldIndex)
     : sourceValueFieldIndex(sourceValueFieldIndex),
       destinationFieldIndex(destinationFieldIndex),
       operatorWithPrevious(op)
    {
    }

    /*! \brief Check if match item is null
     */
    bool isNull() const
    {
      return ( (sourceValueFieldIndex < 0) || (destinationFieldIndex < 0) );
    }

    /*! \brief Clear match item
     */
    
  };

  /*! \brief Unique insert expression for SQL copier
   *
   * Can be used when copying primary keys has no sense,
   *  or source does not contain any primary key.
   *  To solve this, we must provide a couple of fields
   *  in source that can identify a record uniquely in destination.
   *
   * Then, at insertion time (into destination),
   *  the defined (primary) key is fetched.
   */
  class UniqueInsertExpression : public AbstractTableMappingItem
  {
   public:

    /*! \brief Create a invalid expression
     */
    UniqueInsertExpression()
     : AbstractTableMappingItem()
    {
      qDebug() << "C  UniqueInsertExpression::UniqueInsertExpression() - ref: " << ref.load();
    }

    /*! \brief Destructor
     */
    ~UniqueInsertExpression()
    {
      qDebug() << "D  UniqueInsertExpression::~UniqueInsertExpression() - ref: " << ref.load();
    }

    /*! \brief Copy constructor
     */
    UniqueInsertExpression(const UniqueInsertExpression & other)
     : AbstractTableMappingItem(other)
    {
      qDebug() << "CPY  UniqueInsertExpression::UniqueInsertExpression(other) - ref: " << ref.load();
    }

    /*! \brief Polymorphic copy
     */
    UniqueInsertExpression* clone() const override
    {
      qDebug() << " UniqueInsertExpression::clone() - ref: " << ref.load();
      return new UniqueInsertExpression(*this);
    }

    /*! \brief Check if field mapping is null
     *
     * 
     */
    bool isNull() const override
    {
      ///return ( (sourceFieldIndex < 0) || (destinationFieldIndex < 0) );
    }

    /*! \brief Clear expression
     */
    void clear() override
    {
    }

  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_UNIQUE_INSERT_EXPRESSION_H
