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
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QList>
#include <QHash>
#include <QMultiHash>

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

  /*! \brief Create a empty field map
   */
  mdtFieldMap();

  /*! \brief Destroy field map
   *
   * All items are deleted.
   *  Take care that items pointers will become invalid after destroying this object.
   */
  ~mdtFieldMap();

  /*! \brief Add a item
   *
   * A item contains map informations
   *
   * \pre Item must be a valid pointer
   * \sa mdtFieldMapItem
   */
  void addItem(mdtFieldMapItem *item);

  /*! \brief Delete all items
   *
   *  Take care that items pointers will become invalid after calling this method.
   */
  void clear();

  /*! \brief Update a item
   *
   * Whenn updating a item, it's important to call this method,
   *  else internal keys are not updated.
   *
   * \param item item to update
   * \return True on success, false if item was not found
   * \pre Item must be a valid pointer
   */
  ///bool updateItem(mdtFieldMapItem *item);

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
  QList<mdtFieldMapItem*> itemsAtSourceFieldName(const QString &name);

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

  Q_DISABLE_COPY(mdtFieldMap);

  // Main container
  QList<mdtFieldMapItem*> pvItems;
  // To have a fast access, we refer items by several QHash containers
  ///QHash<int, mdtFieldMapItem*> pvItemsByFieldIndex;
  ///QHash<QString, mdtFieldMapItem*> pvItemsByFieldName;
  ///QHash<QString, mdtFieldMapItem*> pvItemsByDisplayText;
  ///QMultiHash<int, mdtFieldMapItem*> pvItemsBySourceFieldIndex;
  ///QMultiHash<QString, mdtFieldMapItem*> pvItemsBySourceFieldName;
};

#endif  // #ifndef MDT_FIELD_MAP_H
