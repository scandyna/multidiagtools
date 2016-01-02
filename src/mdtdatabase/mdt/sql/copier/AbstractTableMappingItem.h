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

#include <QSharedData>

//#include <QDebug>

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Base class for SQL copier table mapping item
   */
  class AbstractTableMappingItem : public QSharedData
  {
   public:

    /*! \brief Default constructor
     */
    AbstractTableMappingItem()
     : QSharedData()
    {
    }

    /*! \brief Destructor
     */
    virtual ~AbstractTableMappingItem()
    {
    }

    // Disable copy (must use clone())
    AbstractTableMappingItem(const AbstractTableMappingItem &) = delete;
    AbstractTableMappingItem & operator=(const AbstractTableMappingItem &) = delete;

    /*! \brief Check if mapping item is null
     */
    virtual bool isNull() const = 0;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_ABSTRACT_TABLE_MAPPING_ITEM_H
