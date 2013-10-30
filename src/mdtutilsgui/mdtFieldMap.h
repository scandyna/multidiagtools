/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#ifndef MDT_FIELD_MAP_H
#define MDT_FIELD_MAP_H

#include "mdtFieldMapItem.h"
#include "mdtFieldMapField.h"
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QList>
#include <QHash>
#include <QMultiHash>
#include <QSharedDataPointer>
#include <QSharedData>

/*
 * mdtFieldMapData definition
 */
class mdtFieldMapData : public QSharedData
{
 public:

  mdtFieldMapData();
  mdtFieldMapData(const mdtFieldMapData &other);
  ~mdtFieldMapData();

  // Available fields container
  QList<mdtFieldMapField> pvSourceFields;
  QList<mdtFieldMapField> pvDestinationFields;
  // Mapping items container
  QList<mdtFieldMapItem*> pvItems;
};

/*! \brief Map fields between a source and a destination data set
 *
 * To explain this class, we take a example.
 *  We have to import data from a CSV file into a table model.
 *  The problem is that some data are grouped in one field, we call it "Group1".
 *  To display the data to a conveniant way, we have to split Group1's data and header.
 *  We know that Group1 contains 3 fileds:
 *   - SubID: index 0 to 1
 *   - SubName: index 2 to 5
 *   - SubValue: index 6 to 9
 *
 * During import, we have to create 3 fields in model, called SubID, SubName and SubValue,
 *  and store correct part of Group1 into each created field.
 *
 * This class was made to help solving such problem.
 */
class mdtFieldMap
{
 public:

  /*! \brief Field reference
   *
   * Used by some methods that need to know what
   *  the reference is to find a field .
   */
  enum FieldReference_t {
                          ReferenceByIndex ,  /*!< Field index is used as reference */
                          ReferenceByName     /*!< Field name is used as reference */
                        };

  /*! \brief Create a empty field map
   */
  mdtFieldMap();

  /*! \brief Copy constructor
   */
  mdtFieldMap(const mdtFieldMap &other);

  /*! \brief Destroy field map
   *
   * All items are deleted.
   *  Take care that items pointers will become invalid after destroying this object.
   */
  ~mdtFieldMap();

  /*! \brief Set the list of source fields
   */
  void setSourceFields(const QList<mdtFieldMapField> &fields);

  /*! \brief Get the list of source fields
   */
  const QList<mdtFieldMapField> &sourceFields() const;

  /*! \brief Get a list of source fields that are not allready mapped
   */
  const QList<mdtFieldMapField> notMappedSourceFields(FieldReference_t reference) const;

  /*! \brief Get the header of source fields
   *
   * The returned list is sorted by column indexes .
   *  For fields that not have a display text set,
   *   the field name is put into header .
   */
  const QStringList sourceHeader() const;

  /*! \brief Set the list of destination fields
   *
   * This can be used if destination fields is allready known .
   *  If mdtFieldMapDialog is used, this fields are also displayed to the
   *  user . For auto map generation, these fields are also needed .
   */
  void setDestinationFields(const QList<mdtFieldMapField> &fields);

  /*! \brief Get the list of destination fields
   */
  const QList<mdtFieldMapField> &destinationFields() const;

  /*! \brief Get a list of destination fields that are not allready mapped
   */
  const QList<mdtFieldMapField> notMappedDestinationFields(FieldReference_t reference) const;

  /*! \brief Get the header of destination fields
   *
   * The returned list is sorted by column indexes .
   *  For fields that not have a display text set,
   *   the field name is put into header .
   */
  const QStringList destinationHeader() const;

  /*! \brief Add a item
   *
   * A item contains map informations
   *
   * \pre Item must be a valid pointer
   *
   * \sa mdtFieldMapItem
   */
  void addItem(mdtFieldMapItem *item);

  /*! \brief Remove item with given source field index
   *
   * If item was not found, nothing happens .
   */
  void removeItemAtSourceFieldIndex(int index);

  /*! \brief Remove item with given destination field index
   *
   * If item was not found, nothing happens .
   */
  void removeItemAtDestinationFieldIndex(int index);

  /*! \brief Get a list of all items
   */
  const QList<mdtFieldMapItem*> &items() const;

  /*! \brief Delete all items
   *
   *  Take care that items pointers will become invalid after calling this method.
   */
  void clear();

  /*! \brief Get the item for a given index
   *
   * \param index In above example, it's the model column index.
   * \return Pointer to item if found, else 0 (null pointer).
   */
  mdtFieldMapItem *itemAtFieldIndex(int index) const;

  /*! \brief Get the item for a given filed name
   *
   * \param name Technical field name.
   * \return Pointer to item if found, else 0 (null pointer).
   */
  mdtFieldMapItem *itemAtFieldName(const QString &name) const;

  /*! \brief Get the item for a given display text
   *
   * \param text In above example, it's the model header text.
   * \return Pointer to item if found, else 0 (null pointer).
   */
  mdtFieldMapItem *itemAtDisplayText(const QString &text);

  /*! \brief Get the source field name for a given field index
   *
   * \param index In above example, it's the model column index.
   * \return Source field name, or a empty string if field index was not found.
   */
  QString sourceFieldNameAtFieldIndex(int index) const;

  /*! \brief Get the items for given source index
   *
   * \param index In above example, it's the index of field in CSV header.
   * \return Pointer to item if found, else 0 (null pointer).
   */
  QList<mdtFieldMapItem*> itemsAtSourceFieldIndex(int index) const;

  /*! \brief Get the items for given source field name
   *
   * \param name In above example, it's the technical field name in CSV header.
   * \return Pointer to item if found, else 0 (null pointer).
   */
  QList<mdtFieldMapItem*> itemsAtSourceFieldName(const QString &name) const;

  /*! \brief Get data for a given index in source data
   *
   * \param sourceData In above example, it's a line in CSV file.
   * \param fieldIndex In above example, it's the model column index.
   * \return Converted data, or invalid QVariant if index was not found, or on failed conversion.
   */
  QVariant dataForFieldIndex(const QStringList &sourceData, int fieldIndex) const;

  /*! \brief Get data for a given field name in source data
   *
   * \param sourceData In above example, it's a line in CSV file.
   * \param fieldName Technical field name.
   * \return Converted data, or invalid QVariant if name was not found, or on failed conversion.
   */
  QVariant dataForFieldName(const QStringList &sourceData, const QString &fieldName) const;

  /*! \brief Get data for a given field display text in source data
   *
   * \param sourceData In above example, it's a line in CSV file.
   * \param displayText In above example, it's the model header text.
   * \return Converted data, or invalid QVariant if index was not found, or on failed conversion.
   */
  QVariant dataForDisplayText(const QStringList &sourceData, const QString &displayText);

  /*! \brief Get source data for given source field index
   *
   * \param data In above example it's the data in model.
   * \param sourceFieldIndex In above example, it's the index in a CSV file's line.
   */
  QString dataForSourceFieldIndex(const QList<QVariant> &data, int sourceFieldIndex);

  /*! \brief Get source data for given source field name
   *
   * \param data In above example it's the data in model.
   * \param sourceFieldName In above example, it's the technical name of field in a CSV file's line.
   */
  QString dataForSourceFieldName(const QList<QVariant> &data, const QString &sourceFieldName);

  /*! \brief Get display texts referenced by field names
   *
   * \return QHash with fieldName as key and displayText as value.
   */
  QHash<QString, QString> displayTextsByFieldNames() const;

 private:

  /*! \brief Insert data into string regarding item's source data offsets
   */
  void insertDataIntoSourceString(QString &str, const QVariant &data, mdtFieldMapItem *item);

  QSharedDataPointer<mdtFieldMapData> d;
};

#endif  // #ifndef MDT_FIELD_MAP_H
