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
      Q_ASSERT(index < pvItems.size());
      return pvItems.at(index).mappingState();
    }

    /*! \brief Get source table name
    */
    virtual QString sourceTableName() const = 0;

    /*! \brief Get destination table name
    */
    virtual QString destinationTableName() const = 0;

    /*! \brief Get table mapping item type
     *
     * \pre index must be in a valid range
     */
    TableMappingItem::Type itemType(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < pvItems.size());
      return pvItems.at(index).type();
    }

    /*! \brief Get field count in source table
     */
    virtual int sourceTableFieldCount() const = 0;

    /*! \brief Get field count in destination table
     */
    virtual int destinationTableFieldCount() const = 0;

    /*! \brief Get a list of available field names in source table
     */
    QStringList getSourceTableFieldNameList() const;

    /*! \brief Get a list of available field names in destination table
     */
    QStringList getDestinationTableFieldNameList() const;

    /*! \brief Set source field at given item index
     *
     * The mapping item at given index will become
     *  a FieldMappingType (if not allready is).
     *
     * If source field name is empty,
     *  the field mapping item will become null.
     *
     * \pre itemIndex must be in a valid range
     * \pre Current mapping item at itemIndex must have exactly 1 destination field index
     */
    void setSourceFieldAtItem(int itemIndex, const QString & fieldName);

    /*! \brief Get field name for given fieldIndex in source table
     *
     * \pre fieldIndex must be in valid range
     */
    QString sourceTableFieldNameAt(int fieldIndex) const;

    /*! \brief Get field index for given fieldName in source table
     */
    int sourceTableFieldIndexOf(const QString & fieldName) const
    {
      return fetchSourceTableFieldIndexOf(fieldName);
    }

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

    /*! \brief Set source fixed value for given mapping item index
     *
     * The mapping item at given index will become
     *  a FixedValueType (if not allready is).
     *
     * \pre itemIndex must be in a valid range
     * \pre Current mapping item at itemIndex must have exactly 1 destination field index
     */
    void setSourceFixedValueAtItem(int itemIndex, const QVariant & value);

    /*! \brief Get source fixed value for given mapping item index
     *
     * \pre itemIndex must be in a valid range
     * \pre item at itemIndex must be of type TableMappingItem::FixedValueType
     */
    QVariant sourceFixedValueAtItem(int itemIndex) const
    {
      Q_ASSERT(itemIndex >= 0);
      Q_ASSERT(itemIndex < pvItems.size());
      Q_ASSERT(pvItems.at(itemIndex).type() == TableMappingItem::FixedValueType);
      return pvItems.at(itemIndex).fixedValue();
    }

    /*! \brief Get field name for given fieldIndex in destination table
     *
     * \pre fieldIndex must be in valid range
     */
    QString destinationTableFieldNameAt(int fieldIndex) const;

    /*! \brief Get field index for given fieldName in destination table
     */
    int destinationTableFieldIndexOf(const QString & fieldName) const
    {
      return fetchDestinationTableFieldIndexOf(fieldName);
    }

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
    void generateFieldMappingByName();

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

    /*! \brief Get a list of available field names in source table
     */
    virtual QStringList fetchSourceTableFieldNameList() const = 0;

    /*! \brief Get a list of available field names in destination table
     */
    virtual QStringList fetchDestinationTableFieldNameList() const = 0;

    /*! \brief Get field index of given field name in source table
     */
    virtual int fetchSourceTableFieldIndexOf(const QString & fieldName) const = 0;

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

    /*! \brief Get field index of given field name in destination table
     */
    virtual int fetchDestinationTableFieldIndexOf(const QString & fieldName) const = 0;

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

    /*! \brief Check if source field is compatible with destination field
    *
    * Subclass should return true if it is possible
    *  to copy data from source field to destination field.
    */
    virtual bool areFieldsCompatible(int sourceFieldIndex, int destinationFieldIndex) const = 0;

    /*! \brief Update given mapping item state
     */
    void updateMappingItemState(TableMappingItem & item);

    /*! \brief Update table mapping state
    */
    void updateTableMappingState();

  private:

    MappingState pvMappingState;
    QVector<TableMappingItem> pvItems;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_TABLE_MAPPING_H
