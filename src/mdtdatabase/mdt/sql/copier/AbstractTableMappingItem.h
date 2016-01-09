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
#ifndef MDT_SQL_COPIER_ABSTRACT_TABLE_MAPPING_ITEM_H
#define MDT_SQL_COPIER_ABSTRACT_TABLE_MAPPING_ITEM_H

#include "TableMappingItemState.h"
#include "FieldIndexList.h"
#include <QSharedData>
#include <QVariant>
#include <QVector>

#include <QDebug>

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Base class for SQL copier table mapping item
   */
  class AbstractTableMappingItem : public QSharedData
  {
   public:

    /*! \brief Default constructor
     */
    AbstractTableMappingItem()
     : QSharedData(),
       pvMappingState(TableMappingItemState::MappingNotSet)
    {
      qDebug() << "C AbstractTableMappingItem::AbstractTableMappingItem() - ref: " << ref.load();
    }

    /*! \brief Destructor
     */
    virtual ~AbstractTableMappingItem()
    {
      qDebug() << "D AbstractTableMappingItem::~AbstractTableMappingItem() - ref: " << ref.load();
    }

    // Disable copy (must use clone())
    AbstractTableMappingItem & operator=(const AbstractTableMappingItem &) = delete;

    /*! \brief Polymorphic copy
     */
    virtual AbstractTableMappingItem* clone() const = 0;

    /*! \brief Check if mapping item is null
     */
    virtual bool isNull() const = 0;

    /*! \brief Clear mapping item
     */
    void clear()
    {
      pvMappingState = TableMappingItemState::MappingNotSet;
      pvDestinationFieldIndexList.clear();
      clearItem();
    }

    /*! \brief Set a field mapping
     *
     * This default implementation does nothing
     */
    virtual void setFieldMapping(int sourceFieldIndex, int destinationFieldIndex)
    {
      Q_UNUSED(sourceFieldIndex);
      Q_UNUSED(destinationFieldIndex);
    }

    /*! \brief Get source field index
     *
     * This default implementation allways returns -1.
     */
    virtual int sourceFieldIndex() const
    {
      return -1;
    }

    /*! \brief Get list of destination field indexes
     */
//     QVector<int> destinationFieldIndexList() const
    FieldIndexList destinationFieldIndexList() const
    {
      return pvDestinationFieldIndexList;
    }

    /*! \brief Get count of destination field indexes
     */
    int destinationFieldIndexCount() const
    {
      return pvDestinationFieldIndexList.count();
    }

    /*! \brief Set fixed value
     *
     * This default implementation does nothing
     */
    virtual void setFixedValue(const QVariant & value, int destinationFieldIndex)
    {
      Q_UNUSED(value);
      Q_UNUSED(destinationFieldIndex);
    }

    /*! \brief Get fixed value
     *
     * This default implementation allways returns a null value.
     */
    virtual QVariant fixedValue() const
    {
      return QVariant();
    }

    /*! \brief Set mapping state for this mapping item
     */
    void setMappingState(TableMappingItemState state)
    {
      pvMappingState = state;
    }

    /*! \brief Get mapping state for this mapping item
     */
    TableMappingItemState mappingState() const
    {
      return pvMappingState;
    }

   protected:

    /*! \brief Clear
     */
    virtual void clearItem() = 0;

    /*! \brief Set destination field index
     */
    void setDestinationFieldIndex(int index)
    {
      pvDestinationFieldIndexList.clear();
      pvDestinationFieldIndexList.append(index);
    }

    /*! \brief Add a destination field index
     */
    void addDestinationFieldIndex(int index)
    {
      pvDestinationFieldIndexList.append(index);
    }

    /*! \brief Clear destination field index list
     */
    void clearDestinationFieldIndexList()
    {
      pvDestinationFieldIndexList.clear();
    }

    /*! \brief Copy constructor
     */
    AbstractTableMappingItem(const AbstractTableMappingItem & other)
     : QSharedData(other),
       pvMappingState(other.pvMappingState),
       pvDestinationFieldIndexList(other.pvDestinationFieldIndexList)
    {
      qDebug() << "CPY AbstractTableMappingItem::AbstractTableMappingItem(other) - ref: " << ref.load();
    }

    TableMappingItemState pvMappingState;
//     QVector<int> pvDestinationFieldIndexList;
    FieldIndexList pvDestinationFieldIndexList;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_ABSTRACT_TABLE_MAPPING_ITEM_H
