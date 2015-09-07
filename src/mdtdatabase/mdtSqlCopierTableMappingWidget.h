/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_SQL_COPIER_TABLE_MAPPING_WIDGET_H
#define MDT_SQL_COPIER_TABLE_MAPPING_WIDGET_H

#include "ui_mdtSqlCopierTableMappingWidget.h"
#include "mdtSqlCopierTableMapping.h"
#include "mdtSqlCopierFieldMapping.h"
#include "mdtError.h"
#include <QWidget>
#include <vector>

class mdtSqlCopierTableMappingWidgetItem;
class QVBoxLayout;

/*! \brief Widget that provides edition of mapping between source and destination tables
 */
class mdtSqlCopierTableMappingWidget : public QWidget, Ui::mdtSqlCopierTableMappingWidget
{
 friend class mdtSqlCopierTableMappingWidgetItem;

 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlCopierTableMappingWidget(QWidget *parent = nullptr);

 private slots:

  /*! \brief Update regarding source codec settings
   */
  void updateSourceCodecSettings(const mdtSqlCopierCodecSettings & cs);

  /*! \brief Update regarding destination codec settings
   */
  void updateDestinationCodecSettings(const mdtSqlCopierCodecSettings & cs);

  /*! \brief Add a field mapping item
   */
  void addFieldMapping();

 private:

  /*! \brief Edit a field mapping
   */
  void editFieldMapping(mdtSqlCopierTableMappingWidgetItem *item);

  /*! \brief Edit a field mapping item
   */
  void removeFieldMapping(mdtSqlCopierTableMappingWidgetItem *item);

  /*! \brief Rebuild the field mapping list
   */
  void rebuildFieldMappingList();

  /*! \brief Display error
   */
  void displayError(const mdtError & error);

  Q_DISABLE_COPY(mdtSqlCopierTableMappingWidget)

  QWidget *pvItemsContainerWidget;
  QVBoxLayout *pvItemsContainerLayout;
  /*
   * To keep field mapping items and widget items coherent,
   * we need a vector to store both.
   * When a item is added or removed,
   * we also rebuild pvTableMapping's vector
   */
  struct FieldMappingItem
  {
    FieldMappingItem(mdtSqlCopierFieldMapping m, mdtSqlCopierTableMappingWidgetItem *w)
     : mapping(m), widget(w)
    {
      Q_ASSERT(widget != nullptr);
    }
    FieldMappingItem()
     : widget(nullptr)
    {
    }
    mdtSqlCopierFieldMapping mapping;
    mdtSqlCopierTableMappingWidgetItem *widget;
  };
  std::vector<FieldMappingItem> pvFieldMappingItems;
  mdtSqlCopierTableMapping pvTableMapping;
  
  
  /*! \brief Add field mapping
   */
  void addFieldMapping(const mdtSqlCopierFieldMapping & fm, mdtSqlCopierTableMappingWidgetItem *widgetItem);

  /*! \brief Get field mapping of given index
   */
  mdtSqlCopierFieldMapping fieldMappingAt(int index) const;

  /*! \brief Update field mapping of given index
   */
  void setFieldMappingAt(int index, const mdtSqlCopierFieldMapping & fm);

  /*! \brief Get mapping index of given widget item
   */
  int fieldMappingIndex(mdtSqlCopierTableMappingWidgetItem *item);

  /*! \brief Add n field mapping widget items
   */
  void addFieldMappingWidgetItems(int n);

  /*! \brief Remove (max) n field mapping widget items
   */
  void removeFieldMappingWidgetItems(int n);

  /*! \brief Update all field mapping widget items
   */
  void updateFieldMappingWidgetItems();
};

#endif // #ifndef MDT_SQL_COPIER_TABLE_MAPPING_WIDGET_H
