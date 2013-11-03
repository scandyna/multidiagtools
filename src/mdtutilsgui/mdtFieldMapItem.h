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
#ifndef MDT_FIELD_MAP_ITEM_H
#define MDT_FIELD_MAP_ITEM_H

#include "mdtFieldMapField.h"
#include <QString>
#include <QStringList>
#include <QVariant>

/*! \brief Provide informations for mdtFieldMap
 *
 * \sa mdtFieldMap
 */
class mdtFieldMapItem
{
 public:

  /*! \brief Create a item with default parameters
   *
   * Default parameters are:
   *  - fieldIndex set to -1
   *  - sourceFieldIndex set to -1
   *  - Empty values/strings
   *  - sourceFieldDataStartOffset set to -1
   *  - sourceFieldDataEndOffset set to -1
   *  - dataType set to QVariant::Invalid
   */
  mdtFieldMapItem();

  ~mdtFieldMapItem();

  /*! \brief Set destination field index
   */
  void setDestinationFieldIndex(int value);

  /*! \brief Get destination field index
   */
  int destinationFieldIndex() const;

  /*! \brief Set destination field
   */
  void setDestinationField(const mdtFieldMapField &field);

  /*! \brief Get destination field
   */
  mdtFieldMapField destinationField() const;

  /*! \brief Set destination field name
   */
  void setDestinationFieldName(const QString &value);

  /*! \brief Get destination field name
   */
  QString destinationFieldName() const;

  /*! \brief Set destination field display text
   */
  void setDestinationFieldDisplayText(const QString &value);

  /*! \brief Get destination field display text
   */
  QString destinationFieldDisplayText() const;

  /*! \brief Set source field index
   */
  void setSourceFieldIndex(int value);

  /*! \brief Get source field index
   */
  int sourceFieldIndex() const;

  /*! \brief Set source field
   */
  void setSourceField(const mdtFieldMapField &field);

  /*! \brief Get source field
   */
  mdtFieldMapField sourceField() const;

  /*! \brief Set source field name
   */
  void setSourceFieldName(const QString &value);

  /*! \brief Get source field name
   */
  QString sourceFieldName() const;

  /*! \brief Set source field display text
   */
  void setSourceFieldDisplayText(const QString &text);

  /*! \brief Get source field display text
   */
  QString sourceFieldDisplayText() const;

  /*! \brief Set source data start offset
   *
   * If offset is < 0, the entiere left part os end offset is considered.
   */
  void setSourceFieldDataStartOffset(int offset);

  /*! \brief Get source data start offset
   */
  int sourceFieldDataStartOffset() const;

  /*! \brief Set source data end offset
   *
   * If offset is < 0, the entiere right part of start offset is considered.
   */
  void setSourceFieldDataEndOffset(int offset);

  /*! \brief Get source data end offset
   */
  int sourceFieldDataEndOffset() const;

  /*! \brief Set data type
   */
  void setDataType(QVariant::Type type);

  /*! \brief Get data type
   */
  QVariant::Type dataType() const;

  /*! \brief Get destination data from source row
   *
   * This method will extract data in sourceRow
   *  at sourceFieldIndex (if valid).
   *
   * If a source field start and end offsets are set,
   *  only the part from start to end is returned.
   */
  QVariant destinationData(const QStringList &sourceRow) const;

  /*! \brief Get destination data from source data
   *
   * If a source field start and end offsets are set,
   *  only the part from start to end is returned.
   */
  QVariant destinationData(const QString &sourceData) const;

 private:

  mdtFieldMapField pvSourceField;
  mdtFieldMapField pvDestinationField;
  int pvSourceFieldDataStartOffset;
  int pvSourceFieldDataEndOffset;
};
Q_DECLARE_METATYPE(mdtFieldMapItem*);

#endif  // #ifndef MDT_FIELD_MAP_ITEM_H
