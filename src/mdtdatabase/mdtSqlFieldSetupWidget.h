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
#ifndef MDT_SQL_FIELD_SETUP_WIDGET_H
#define MDT_SQL_FIELD_SETUP_WIDGET_H

#include "ui_mdtSqlFieldSetupWidget.h"
#include "mdtSqlFieldSetupData.h"
#include "mdtError.h"
#include <QWidget>
#include <QSqlDatabase>
#include <QVariant>
#include <QString>

class mdtSqlSchemaTable;
class mdtSqlSchemaTableModel;

/*! \brief Field setup edition mode
 *
 * This enum is used by mdtSqlFieldSetupData and mdtSqlFieldSetupWidget
 */
enum class mdtSqlFieldSetupEditionMode_t
{
  Creation,     /*!< For creation of a new field.
                     Will not list available fields, but let the user enter a field name.
                     All field attributes are editable. */
  Selection,    /*!< For selection of a existing field.
                     Will list available fields. Only field selection is possible, no attribute is editable. */
  Edition,      /*!< For selection and edition of a existing field.
                     Will list available fields. Its possible to edit selected field's attributes. */
  
  /// \todo View only mode ? (no selection, no edition)
};

/// \todo Show/hide database specific attributes ?

/*! \brief Field setup widget
 */
class mdtSqlFieldSetupWidget : public QWidget, Ui::mdtSqlFieldSetupWidget
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlFieldSetupWidget(QWidget *parent);

  /*! \brief Set edition mode
   */
  void setEditionMode(mdtSqlFieldSetupEditionMode_t mode);

  /*! \brief Set table
   *
   * Will also reverse table to list all aviable fields
   */
  bool setTable(const QString & tableName, QSqlDatabase db);

  /*! \brief Set table
   *
   * Will also list all aviable fields
   */
  bool setTable(const mdtSqlSchemaTable & tableSchema, QSqlDatabase db);

  /*! \brief Set given field index as selected one
   */
  void setField(int fieldIndex);

  /*! \brief Set setup data
   */
//   bool setData(const mdtSqlFieldSetupData & data, QSqlDatabase db);

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private slots:

  /*! \brief Update field parameters
   */
  void updateFieldParameters(int cbFieldIndex);

 private:

  /*! \brief Populate field type combobox
   */
  bool populateFieldTypeCombobox(const QString & driverName);

  /*! \brief Set field type
   */
  void setFieldType(QVariant::Type type);

  /*! \brief Set creation edition mode
   */
  void setEditionModeCreation();

  /*! \brief Set selection edition mode
   */
  void setEditionModeSelection();

  /*! \brief Set Edition edition mode
   */
  void setEditionModeEdition();

  Q_DISABLE_COPY(mdtSqlFieldSetupWidget)

  mdtSqlSchemaTableModel *pvSchemaTabledModel;
//   mdtSqlFieldSetupData pvSetupData;
  mdtError pvLastError;
};

#endif // #ifndef MDT_SQL_FIELD_SETUP_WIDGET_H
