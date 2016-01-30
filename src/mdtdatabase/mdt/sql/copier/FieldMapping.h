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
#ifndef MDT_SQL_COPIER_FIELD_MAPPING_H
#define MDT_SQL_COPIER_FIELD_MAPPING_H

#include "AbstractTableMappingItem.h"
#include "mdt/sql/FieldIndexList.h"

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Table mapping item implementation for field mapping
   */
  class FieldMapping : public AbstractTableMappingItem
  {
   public:

    /*! \brief Create a invalid field mapping
     */
    FieldMapping()
     : AbstractTableMappingItem()
    {
      qDebug() << "C  FieldMapping::FieldMapping() - ref: " << ref.load();
    }

    /*! \brief Destructor
     */
    ~FieldMapping()
    {
      qDebug() << "D  FieldMapping::~FieldMapping() - ref: " << ref.load();
    }

    /*! \brief Polymorphic copy
     */
    FieldMapping* clone() const override
    {
      qDebug() << " FieldMapping::clone() - ref: " << ref.load();
      return new FieldMapping(*this);
    }

    /*! \brief Check if field mapping is null
     *
     * Field mapping is null if source field index is < 0 or no destination field index was set.
     */
    bool isNull() const override
    {
      return ( pvSourceFieldIndex.isEmpty() || (destinationFieldIndexCount() < 1) );
    }

    /*! \brief Set a field mapping
     */
    void setFieldMapping(int sourceFieldIndex, int destinationFieldIndex) override
    {
      pvSourceFieldIndex.clear();
      if(sourceFieldIndex > -1){
        pvSourceFieldIndex.append(sourceFieldIndex);
      }
      setDestinationFieldIndex(destinationFieldIndex);
    }

    /*! \brief Get count of source field indexes
     */
    int sourceFieldIndexCount() const override
    {
      return pvSourceFieldIndex.count();
    }

    /*! \brief Get list of source field indexes
     */
    FieldIndexList sourceFieldIndexList() const override
    {
      return pvSourceFieldIndex;
    }

   private:

    /*! \brief Clear field mapping
     */
    void clearItem() override
    {
      pvSourceFieldIndex.clear();
    }

    /*! \brief Copy constructor (used by clone)
     */
    FieldMapping(const FieldMapping & other)
     : AbstractTableMappingItem(other),
       pvSourceFieldIndex(other.pvSourceFieldIndex)
    {
      qDebug() << "CPY  FieldMapping::FieldMapping(other) - ref: " << ref.load();
    }

    FieldIndexList pvSourceFieldIndex;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_FIELD_MAPPING_H
