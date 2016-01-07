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
#ifndef MDT_SQL_COPIER_TABLE_MAPPING_H
#define MDT_SQL_COPIER_TABLE_MAPPING_H

#include "mdtError.h"
#include "TableMappingItem.h"
#include <QVector>
#include <QString>
#include <QStringList>

#include "mdtSqlCopierFieldMapping.h"

class QSqlDatabase;

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Common base class for SQL copier table mapping
  *
  * \todo Copy should be disabled in this base class (slicing!)
  */
  class TableMapping
  {
  public:

    /*! \brief Table mapping state
    */
    enum MappingState
    {
      MappingNotSet,      /*!< Source or destination table was not set,
                              or field mapping was not set. */
      MappingComplete,    /*!< Source and destination table have been successfully set.
                              Field mapping was done automatically without any mismatch,
                              or confirmed by the user as complete. */
      MappingPartial,     /*!< Source and destination table have been successfully set.
                              Field mapping was done automatically, but some mismatch
                              was detected, and a action is required from the user to fix it */
      MappingError        /*!< A error was detected during source or detination table set,
                              or during field mapping. Get more information with lastError() . */
    };

    /*! \brief Field key type
    */
    enum FieldKeyType
    {
      NotAKey,    /*!< Field a not a key */
      PrimaryKey  /*!< Field is part of primary key */
    };

    /*! \brief Construct a empty table mapping
    */
    TableMapping();

    /*! \brief Destructor
    */
    virtual ~TableMapping() = default;

    /*! \brief Get table mapping state
    */
    MappingState mappingState() const
    {
      return pvMappingState;
    }

    /*! \brief Get field count
    *
    * Field count depends on number of field mapping set.
    *
    * \todo Will be wrong now !!
    */
    int fieldCount() const
    {
      return pvFieldMappingList.size();
    }

    /*! \brief Get count of map items
     */
    int itemsCount() const
    {
      return pvItems.size();
    }

    /*! \brief Get table mapping item's mapping state
     *
     * \pre index must be in a valid range
     */
    TableMappingItemState itemMappingState(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < pvFieldMappingList.size());
      return pvItems.at(index).mappingState();
    }

    /*! \brief Get field mapping state for given index
    *
    * \pre index must be in a valid range
    */
    mdtSqlCopierFieldMapping::MappingState fieldMappingState(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < pvFieldMappingList.size());
      return pvFieldMappingList.at(index).mappingState;
    }

    /*! \brief Get source table name
    */
    virtual QString sourceTableName() const = 0;

    /*! \brief Get destination table name
    */
    virtual QString destinationTableName() const = 0;

    /*! \brief Set source type (field or fixed value)
    *
    * \pre index must be in a valid range
    */
  //   void setSourceType(int index, mdtSqlCopierFieldMapping::SourceType type);

    /*! \brief Get table mapping item type
     *
     * \pre index must be in a valid range
     */
    TableMappingItem::Type itemType(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < pvFieldMappingList.size());
      return pvItems.at(index).type();
    }

    /*! \brief Get source type (field or fixed value)
    *
    * \pre index must be in a valid range
    */
    mdt::sql::copier::SourceField::Type sourceType(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < pvFieldMappingList.size());
      return pvFieldMappingList.at(index).sourceField.type();
    }

    /*! \brief Get field count in source table
     */
    virtual int sourceTableFieldCount() const = 0;

    /*! \brief Get field count in destination table
     */
    virtual int destinationTableFieldCount() const = 0;

    /*! \brief Set source field for given field mapping index
    *
    * If source field name is empty,
    *  the source field will be removed for given index.
    *
    * \pre index must be in a valid range
    */
    void setSourceField(int index, const QString & fieldName);

    /*! \brief Get field name for given fieldIndex in source table
     *
     * \pre fieldIndex must be in valid range
     */
    QString sourceTableFieldNameAt(int fieldIndex) const;

    /*! \brief Get source field name for map item at given index
     *
     * \pre itemIndex must be in valid range
     */
    QString sourceFieldNameAtItem(int itemIndex) const;

    /*! \brief Get source field type name for map item at given index
     *
     * \pre itemIndex must be in valid range
     * \pre map item for given itemIndex must be TableMappingItem::FieldMappingType
     */
    QString sourceFieldTypeNameAtItem(int itemIndex) const;

    /*! \brief Check if source field is part of a key for map item at given index
     *
     * \pre itemIndex must be in valid range
     * \pre map item for given itemIndex must be TableMappingItem::FieldMappingType
     */
    FieldKeyType sourceFieldKeyTypeAtItem(int itemIndex) const;

    /*! \brief Get source field name for given field mapping index
    *
    * \pre index must be in valid range.
    * \pre source type for given index must be mdtSqlCopierFieldMapping::Field
    *
    * \deprecated use sourceFieldNameAtItem()
    */
    [[deprecated]]
    QString sourceFieldName(int index) const;

    /*! \brief Get source field type name for given field mapping index
    *
    * \pre index must be in valid range.
    * \pre source type for given index must be mdtSqlCopierFieldMapping::Field
    *
    * \deprecated use sourceFieldTypeNameAtItem()
    */
    QString sourceFieldTypeName(int index) const;

    /*! \brief Check if source field is part of a key
    *
    * \pre index must be in valid range.
    * \pre source type for given index must be mdtSqlCopierFieldMapping::Field
    *
    * \deprecated use sourceFieldKeyTypeAtItem()
    */
    FieldKeyType sourceFieldKeyType(int index) const;

    /*! \brief Set source fixed value for given field mapping index
    *
    * \pre index must be in a valid range
    * \pre source type for given index must be mdtSqlCopierFieldMapping::FixedValue
    */
    void setSourceFixedValue(int index, const QVariant & value);

    /*! \brief Get source fixed value for given field mapping index
    *
    * \pre index must be in a valid range
    * \pre source type for given index must be mdtSqlCopierFieldMapping::FixedValue
    */
    QVariant sourceFixedValue(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < pvFieldMappingList.size());
      Q_ASSERT(pvFieldMappingList.at(index).sourceField.type() == mdt::sql::copier::SourceField::SourceFixedValueType);
      return pvFieldMappingList.at(index).sourceField.fixedValue();
    }

    /*! \brief Get field name for given fieldIndex in destination table
     *
     * \pre fieldIndex must be in valid range
     */
    QString destinationTableFieldNameAt(int fieldIndex) const;

    /*! \brief Get destination field name list for map item at given index
     *
     * \pre itemIndex must be in valid range
     */
    QStringList destinationFieldNameListAtItem(int itemIndex) const;

    /*! \brief Get destination field type name list for map item at given index
     *
     * \pre itemIndex must be in valid range
     */
    QStringList destinationFieldTypeNameListAtItem(int itemIndex) const;

    /*! \brief Check if destination fields are part of a key for map item at given index
     *
     * \pre itemIndex must be in valid range
     */
    QVector<FieldKeyType> destinationFieldKeyTypeListAtItem(int itemIndex) const;

    /*! \brief Get destination field name for given field mapping index
    *
    * \pre index must be in valid range.
    *
    * \deprecated use destinationFieldNameListAtItem()
    */
    virtual QString destinationFieldName(int index) const = 0;

    /*! \brief Get destination field type name for given field mapping index
    *
    * \pre index must be in valid range.
    *
    * \deprecated use destinationFieldTypeNameListAtItem()
    */
    virtual QString destinationFieldTypeName(int index) const = 0;

    /*! \brief Check if destination field is part of a key
    *
    * Default implementation allways returns NotAKey.
    *
    * \deprecated use destinationFieldKeyTypeListAtItem()
    */
    virtual FieldKeyType destinationFieldKeyType(int index) const
    {
      Q_UNUSED(index);
      return NotAKey;
    }

    /*! \brief Get last error
    */
    mdtError lastError() const
    {
      return pvLastError;
    }

    /*! \brief Generate field mapping by name
    *
    * Will first reset field mapping.
    *  Then, for each field in destination table,
    *  source field is defined by destination field name.
    */
    virtual void generateFieldMappingByName() = 0;

    /*! \brief Reset field mapping
    *
    * Will clear field mapping,
    *  then, generate it for each available field in destination table.
    *  After this, no field from source table is assigned to destination table.
    */
    void resetFieldMapping();

    /*! \brief Clear field mapping
    */
    void clearFieldMapping();

    /*! \brief Get SQL statement to count rows in source table
    *
    * \note We cannot use internal databases, because this function can be called from non main thread (f.ex. mdtSqlDatabaseCopierThread)
    */
    QString getSqlForSourceTableCount(const QSqlDatabase & db) const;

    /*! \brief Get SQL statement to select source table data
    *
    * Will build a SQL SELECT statement that contains
    *  fields regarding mapping.
    *
    * \note We cannot use internal databases, because this function can be called from non main thread (f.ex. mdtSqlDatabaseCopierThread)
    */
    QString getSqlForSourceTableSelect(const QSqlDatabase & db) const;

    /*! \brief Get SQL statement to insert data into destination table
    *
    * Will build a INSERT INTO statement that contains
    *  fields regarding mapping.
    *  This stamenet can be used with QSqlQuery's bind vaue with positional placeholder.
    *
    * Format is:
    *  INSERT INTO SourceTable (field0, field1, ..., fieldN) VALUES(?, ?, ..., ?)
    *
    * \note We cannot use internal databases, because this function can be called from non main thread (f.ex. mdtSqlDatabaseCopierThread)
    */
    QString getSqlForDestinationTableInsert(const QSqlDatabase & db) const;

  protected:

    // Copy of this base class is not allowed
    TableMapping(const TableMapping &) = default;
    TableMapping & operator=(const TableMapping &) = default;

    /*! \brief Set source field index for given field mapping
    */
    virtual void setSourceFieldIndex(mdtSqlCopierFieldMapping & fm, const QString & sourceFieldName) = 0;

    /*! \brief Get field name for given fieldIndex in source table
     */
    virtual QString fetchSourceTableFieldNameAt(int fieldIndex) const = 0;

    /*! \brief Get field type name for given fieldIndex in source table
     */
    virtual QString fetchSourceTableFieldTypeNameAt(int fieldIndex) const = 0;

    /*! \brief Check if field is part of a key for given field index in source table
     *
     * Default implementation allways returns NotAKey.
     */
    virtual FieldKeyType fetchSourceTableFieldKeyType(int fieldIndex) const
    {
      Q_UNUSED(fieldIndex);
      return NotAKey;
    }

    /*! \brief Get field name for given fieldIndex in destination table
     */
    virtual QString fetchDestinationTableFieldNameAt(int fieldIndex) const = 0;

    /*! \brief Get field type name for given fieldIndex in destination table
     */
    virtual QString fetchDestinationTableFieldTypeNameAt(int fieldIndex) const = 0;

    /*! \brief Check if field is part of a key for given field index in destination table
     *
     * Default implementation allways returns NotAKey.
     */
    virtual FieldKeyType fetchDestinationTableFieldKeyType(int fieldIndex) const
    {
      Q_UNUSED(fieldIndex);
      return NotAKey;
    }

    /*! \brief Last error
    */
    mdtError pvLastError;

    /*! \brief Access field mapping list (read only)
    */
    const QVector<mdtSqlCopierFieldMapping> & constFieldMappingList() const
    {
      return pvFieldMappingList;
    }

    /*! \brief Access field mapping list
    */
    QVector<mdtSqlCopierFieldMapping> & fieldMappingList()
    {
      return pvFieldMappingList;
    }

    /*! \brief Access field mapping for given index
    *
    * \pre index must be valid
    */
    const mdtSqlCopierFieldMapping & fieldMappingAt(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < pvFieldMappingList.size());
      return pvFieldMappingList.at(index);
    }

    /*! \brief Update field mapping at given index
    *
    * \pre index must be valid
    * 
    * \todo Check if used
    */
    void updateFieldMappingAt(int index, const mdtSqlCopierFieldMapping & fm)
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < pvFieldMappingList.size());
      pvFieldMappingList[index] = fm;
    }

    /*! \brief Check if source field is compatible with destination field
    *
    * Subclass should return true if it is possible
    *  to copy data from source field to destination field.
    */
    virtual bool areFieldsCompatible(int sourceFieldIndex, int destinationFieldIndex) const = 0;

    /*! \brief Update given field mapping state
    */
    void updateFieldMappingState(mdtSqlCopierFieldMapping & fm);

    /*! \brief Update table mapping state
    */
    void updateTableMappingState();

  private:

    MappingState pvMappingState;
    QVector<TableMappingItem> pvItems;
    
    QVector<mdtSqlCopierFieldMapping> pvFieldMappingList;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_TABLE_MAPPING_H
