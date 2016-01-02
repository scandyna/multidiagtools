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

#include <QSharedDataPointer>

namespace mdt{ namespace sql{ namespace copier{

  class AbstractTableMappingItem;

  /*! \brief Mapping item of mdtSqlCopierTableMapping
   */
  class TableMappingItem
  {
   public:

    /*! \brief Table impping item type
     */
    enum Type
    {
      FieldMappingType,       /*!< A mapping of source field to destination field */
      FixedValueType,         /*!< A fixed value will be copied to defined destination field */
      UniqueInsertExpression  /*!< A expression that search a (primary) key by matching fields in destination table
                                   with values in source table. */
    };

    /*! \brief Construct a mapping item of given type
     */
    TableMappingItem(Type type);

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

   private:

    Type pvType;
    QSharedDataPointer<AbstractTableMappingItem> pvShared;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_TABLE_MAPPING_ITEM_H
