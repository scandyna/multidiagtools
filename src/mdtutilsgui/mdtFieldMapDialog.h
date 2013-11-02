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
#ifndef MDT_FIELD_MAP_DIALOG_H
#define MDT_FIELD_MAP_DIALOG_H

#include "ui_mdtFieldMapDialog.h"
#include "mdtFieldMap.h"
#include <QDialog>
#include <QWidget>

/*! \brief Dialog to manage field mapping
 */
class mdtFieldMapDialog : public QDialog, Ui::mdtFieldMapDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtFieldMapDialog(QWidget *parent = 0);

  /*! \brief Destructor
   */
  ~mdtFieldMapDialog();

  /*! \brief Set field map object
   *
   * \pre map must contain at least 1 source field .
   */
  void setFieldMap(const mdtFieldMap &map);

  /*! \brief Get field map object
   */
  mdtFieldMap fieldMap() const;

 private slots:

  /*! \brief Add a mapping item
   */
  void addMapItem();

  /*! \brief Remove a mapping item
   */
  void removeMapItem();

  /*! \brief Edit destination field list
   */
  void editDestinationFieldList();

 private:

  Q_DISABLE_COPY(mdtFieldMapDialog);

  /*! \brief Update the mapping table widget
   */
  void updateMappingTableView();

  /*! \brief Update the source preview
   */
  void updateSourcePreview();

  /*! \brief Update the destination preview
   */
  void updateDestinationPreview();

  mdtFieldMap pvFieldMap;
};

#endif // #ifndef MDT_FIELD_MAP_DIALOG_H
