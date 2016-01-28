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
#ifndef MDT_SQL_COPIER_RELATED_TABLE_INSERT_EXPRESSION_H
#define MDT_SQL_COPIER_RELATED_TABLE_INSERT_EXPRESSION_H

#include "AbstractTableMappingItem.h"
#include "ExpressionMatchItem.h"
#include "FieldIndexList.h"
#include <QString>
#include <vector>

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Fetch value in related table in SQL copier
   */
  class RelatedTableInsertExpression : public AbstractTableMappingItem
  {
   public:

    /*! \brief Create a invalid expression
     */
    RelatedTableInsertExpression()
     : AbstractTableMappingItem()
    {
      qDebug() << "C  RelatedTableInsertExpression::RelatedTableInsertExpression() - ref: " << ref.load();
    }

    /*! \brief Destructor
     */
    ~RelatedTableInsertExpression()
    {
      qDebug() << "D  RelatedTableInsertExpression::~RelatedTableInsertExpression() - ref: " << ref.load();
    }

    /*! \brief Copy constructor
     */
    RelatedTableInsertExpression(const RelatedTableInsertExpression & other)
     : AbstractTableMappingItem(other),
       pvSourceRelatedTableName(other.pvSourceRelatedTableName),
       pvDestinationRelatedTableName(other.pvDestinationRelatedTableName),
       pvDestinationRelatedTableKey(other.pvDestinationRelatedTableKey),
       pvMatchItems(other.pvMatchItems)
    {
      qDebug() << "CPY  RelatedTableInsertExpression::RelatedTableInsertExpression(other) - ref: " << ref.load();
    }

    /*! \brief Polymorphic copy
     */
    RelatedTableInsertExpression* clone() const override
    {
      qDebug() << " RelatedTableInsertExpression::clone() - ref: " << ref.load();
      return new RelatedTableInsertExpression(*this);
    }

    /*! \brief Add a destination field index
     */
    void addDestinationFieldIndex(int index)
    {
      Q_ASSERT(index >= 0);
      AbstractTableMappingItem::addDestinationFieldIndex(index);
    }

    /*! \brief Set source related table name
     *
     * By default, if no sourceRelatedTableName is set,
     *  source value fields refers to source table of table mapping.
     *
     * If table mapping's source is a database table,
     *  it is possible to fetch source values
     *  from a other table in source database
     *  by specifying its name here.
     *  In this case, source value fields will refer
     *  to the specified table.
     *
     * \sa sourceRelatedTableName()
     */
    void setSourceRelatedTableName(const QString & tableName)
    {
      pvSourceRelatedTableName = tableName;
    }

    /*! \brief Get source related table name
     *
     * \sa setSourceRelatedTableName()
     */
    QString sourceRelatedTableName() const
    {
      return pvSourceRelatedTableName;
    }

    /*! \brief Check if expression is null
     *
     * Expression is null if:
     *  - not match item was set
     *  - no destination field index was set
     *  - destination related table name was not set
     *  - destination related table key was not set
     */
    bool isNull() const override
    {
      return ( pvMatchItems.empty()
               || (destinationFieldIndexCount() < 1)
               || pvDestinationRelatedTableName.isEmpty()
               || pvDestinationRelatedTableKey.isEmpty()
             );
    }

   private:

    /*! \brief Clear expression
     */
    void clearItem() override
    {
      pvSourceRelatedTableName.clear();
      pvDestinationRelatedTableName.clear();
      pvDestinationRelatedTableKey.clear();
      pvMatchItems.clear();
    }

    QString pvSourceRelatedTableName;
    QString pvDestinationRelatedTableName;
    FieldIndexList pvDestinationRelatedTableKey;
    std::vector<ExpressionMatchItem> pvMatchItems;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_RELATED_TABLE_INSERT_EXPRESSION_H
