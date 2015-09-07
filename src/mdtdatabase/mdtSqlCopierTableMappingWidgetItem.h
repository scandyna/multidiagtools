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
#ifndef MDT_SQL_COPIER_TABLE_MAPPING_WIDGET_ITEM_H
#define MDT_SQL_COPIER_TABLE_MAPPING_WIDGET_ITEM_H

#include "ui_mdtSqlCopierTableMappingWidgetItem.h"
#include <QWidget>
#include <QString>

class mdtSqlCopierTableMappingWidget;

/*! \brief Item of mdtSqlCopierTableMappingWidget
 *
 * \todo Should be renamed mdtSqlCopierTableMappingWidgetItem
 */
class mdtSqlCopierTableMappingWidgetItem : public QWidget, Ui::mdtSqlCopierTableMappingWidgetItem
{
 public:

  /*! \brief Constructor
   *
   * \param parent Parent widget
   * \param owner The container that creates this item
   * 
   * \pre owner must be a valid pointer
   */
  mdtSqlCopierTableMappingWidgetItem(QWidget *parent, mdtSqlCopierTableMappingWidget *owner);

  /*! \brief Set separation line visible
   */
  void setSeparationLineVisible(bool visible);

  /*! \brief Set source field name
   */
  void setSourceFieldName(const QString & name);

  /*! \brief Set destination field name
   */
  void setDestinationFieldName(const QString & name);

 private slots:

  /*! \brief Edit field mapping
   */
  void editFieldMapping();

  /*! \brief Remove field mapping
   */
  void removeFieldMapping();

 private:

  Q_DISABLE_COPY(mdtSqlCopierTableMappingWidgetItem)

  mdtSqlCopierTableMappingWidget *pvOwner;
};

#endif // #ifndef MDT_SQL_COPIER_TABLE_MAPPING_WIDGET_ITEM_H
