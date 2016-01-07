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
#ifndef MDT_SQL_COPIER_TABLE_MAPPING_ITEM_H
#define MDT_SQL_COPIER_TABLE_MAPPING_ITEM_H

#include "TableMappingItemState.h"
#include <QSharedDataPointer>
#include <QVariant>
#include <QVector>

namespace mdt{ namespace sql{ namespace copier{

  class AbstractTableMappingItem;
  class UniqueInsertExpression;

  /*! \brief Mapping item of mdtSqlCopierTableMapping
   */
  class TableMappingItem
  {
   public:

    /*! \brief Table impping item type
     */
    enum Type
    {
      FieldMappingType,           /*!< A mapping of source field to destination field */
      FixedValueType,             /*!< A fixed value will be copied to defined destination field */
      UniqueInsertExpressionType  /*!< A expression that search a (primary) key by matching fields in destination table
                                       with values in source table. */
    };

    /*! \brief Construct a mapping item of given type
     */
    TableMappingItem(Type type = FieldMappingType);

    /*! \brief Copy constructor
     */
    TableMappingItem(const TableMappingItem & other);

    /*! \brief Destructor
     */
    ~TableMappingItem();

    /*! \brief Copy assignment
     */
    TableMappingItem & operator=(const TableMappingItem & other);

    /*! \brief Get mapping item type
     */
    Type type() const
    {
      return pvType;
    }

    /*! \brief Check if mapping item is null
     */
    bool isNull() const;

    /*! \brief Clear mapping item
     */
    void clear();

    /*! \brief Set a field mapping
     */
    void setFieldMapping(int sourceFieldIndex, int destinationFieldIndex);

    /*! \brief Get source field index
     *
     * Has sense for FieldMappingType only.
     */
    int sourceFieldIndex() const;

    /*! \brief Get list of destination field indexes
     */
    QVector<int> destinationFieldIndexList() const;

    /*! \brief Set fixed value
     */
    void setFixedValue(const QVariant & value, int destinationFieldIndex);

    /*! \brief Get fixed value
     *
     * Has sense for FixedValueType only.
     */
    QVariant fixedValue() const;

    /*! \brief Set a unique insert expression
     */
    void setUniqueInsertExpression(const UniqueInsertExpression & exp);

    /*! \brief Set mapping state for this mapping item
     */
    void setMappingState(TableMappingItemState state);

    /*! \brief Get mapping state for this mapping item
     */
    TableMappingItemState mappingState() const;

   private:

    /*! \brief Reset to new type
     */
    void reset(Type type);

    /*! \brief Construct shared data
     */
    void constructShared();

    Type pvType;
    QSharedDataPointer<AbstractTableMappingItem> pvShared;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_TABLE_MAPPING_ITEM_H
