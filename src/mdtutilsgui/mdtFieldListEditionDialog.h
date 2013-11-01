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
#ifndef MDT_FIELD_LIST_EDITION_DIALOG_H
#define MDT_FIELD_LIST_EDITION_DIALOG_H

#include "ui_mdtFieldListEditionDialog.h"
#include "mdtFieldMapField.h"
#include <QDialog>
#include <QWidget>
#include <QList>

/*! \brief Dialog that provides field list edition
 */
class mdtFieldListEditionDialog : public QDialog, Ui::mdtFieldListEditionDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtFieldListEditionDialog(QWidget *parent = 0);

  /*! \brief Destructor
   */
  ~mdtFieldListEditionDialog();

  /*! \brief Set the list of available fields
   *
   * Note: here, given fields must have contigous column indexes,
   *  "holes" are not handled .
   */
  void setAvailableFields(const QList<mdtFieldMapField> &fields, int startFieldIndex);

  /*! \brief Get the list of fields
   */
  const QList<mdtFieldMapField> &fields() const;

 private slots:

  /*! \brief Add a field
   */
  void addField();

  /*! \brief Remove a field
   */
  void removeField();

  /*! \brief Move selected item up
   */
  void moveSelectedItemUp();

  /*! \brief Move selected item down
   */
  void moveSelectedItemDown();

  /*! \brief Do some check and accept if ok
   */
  void accept();

 private:

  /*! \brief Re-index fields
   */
  void reIndexFields();

  /*! \brief Dose some checks on fields and build the result fields list
   */
  bool buildResult();

  /*! \brief Check about illegal strings in field name and ask the user about a action
   */
  QString correctFieldName(const QString &name);

  /*! \brief Check about duplicate field name
   */
  bool haveDupplicateFieldName();

  /*! \brief Swap field name and display text between 2 rows
   */
  void swapRows(int row1, int row2);

  Q_DISABLE_COPY(mdtFieldListEditionDialog);

  QList<mdtFieldMapField> pvResultFields;
  int pvStartFieldIndex;
};

#endif // #ifndef MDT_FIELD_LIST_EDITION_DIALOG_H
