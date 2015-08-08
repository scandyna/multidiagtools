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
#ifndef MDT_SQL_FIELD_SETUP_DATA_H
#define MDT_SQL_FIELD_SETUP_DATA_H

#include <QSqlField>
#include <QString>
#include <QVariant>

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

/*! \brief Field setup data container
 */
struct mdtSqlFieldSetupData
{
  /*! \brief Table name
   */
  QString tableName;

  /*! \brief True if this field is part of primary key
   */
  bool isPartOfPrimaryKey;

  /*! \brief Field attributes
   */
  QSqlField field;

  /*! \brief Edition mode
   *
   * Default edition mode is Selection
   */
  mdtSqlFieldSetupEditionMode_t editionMode;

  /*! \brief Constructor
   */
  mdtSqlFieldSetupData()
   : isPartOfPrimaryKey(false),
     editionMode(mdtSqlFieldSetupEditionMode_t::Selection)
  {
  }

  /*! \brief Check if setup data is null
   *
   * Setup data is null if table name is empty,
   *  field's name is empty or field's type is invalid
   */
  bool isNull()
  {
    return ( tableName.isEmpty() || field.name().isEmpty() || (field.type() == QVariant::Invalid) );
  }

  /*! \brief Clear setup data
   */
  void clear()
  {
    tableName.clear();
    isPartOfPrimaryKey = false;
    field = QSqlField();
    editionMode = mdtSqlFieldSetupEditionMode_t::Selection;
  }
};

#endif // #ifndef MDT_SQL_FIELD_SETUP_DATA_H
