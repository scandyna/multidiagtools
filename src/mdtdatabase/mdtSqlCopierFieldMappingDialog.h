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
#ifndef MDT_SQL_COPIER_FIELD_MAPPING_DIALOG_H
#define MDT_SQL_COPIER_FIELD_MAPPING_DIALOG_H

#include "ui_mdtSqlCopierFieldMappingDialog.h"
#include "mdtSqlCopierFieldMapping.h"
#include "mdtSqlCopierCodecSettings.h"
#include "mdtError.h"
#include <QDialog>
#include <QString>

class mdtSqlFieldSetupWidget;

/*! \brief Field mapping dialog
 *
 * \todo Should be reamed mdtSqlCopierFieldMappingDialog
 */
class mdtSqlCopierFieldMappingDialog : public QDialog, Ui::mdtSqlCopierFieldMappingDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlCopierFieldMappingDialog(QWidget *parent = nullptr);

  /*! \brief Set source
   */
  bool setSource(const mdtSqlCopierCodecSettings & cs);

  /*! \brief Set source field
   */
  void setSourceField(int fieldIndex);

  /*! \brief Get the name of select source field
   */
  QString sourceFieldName() const;

  /*! \brief Set destination
   */
  bool setDestination(const mdtSqlCopierCodecSettings & cs);

  /*! \brief Set destination field
   */
  void setDestinationField(int fieldIndex);

  /*! \brief Get the name of select destination field
   */
  QString destinationFieldName() const;

  /*! \brief Set field mapping
   */
  void setMapping(const mdtSqlCopierFieldMapping & mapping);

  /*! \brief Get field mapping
   */
  mdtSqlCopierFieldMapping mapping() const
  {
    return pvMapping;
  }

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private slots:

  /*! \brief Enable/disable source splitting options widgets regarding rbNoSourceSplitting's state
   */
  void setSourceSplittingEditionEnabled(bool rbNoSourceSplittingEnabled);

  /*! \brief Enable/disable destination cat options widgets regarding rbNoDestinationCat's state
   */
  void setDestinationCatEditionEnabled(bool rbNoDestinationCatEnabled);

  /*! \brief Update mapping data, check and accept if OK
   */
  void accept();

 private:

  /*! \brief Build and setup source selection widget
   */
  void buildSourceSelectionWidget(mdtSqlCopierCodecSettings::CodecType codecType);

  /*! \brief Build and setup SQL database source selection widget
   */
  void buildSourceDatabaseSelectionWidget();

  /*! \brief Build and setup destination selection widget
   */
  void buildDestinationSelectionWidget(mdtSqlCopierCodecSettings::CodecType codecType);

  /*! \brief Build and setup SQL database destination selection widget
   */
  void buildDestinationDatabaseSelectionWidget();

  Q_DISABLE_COPY(mdtSqlCopierFieldMappingDialog)

  mdtError pvLastError;
  mdtSqlCopierFieldMapping pvMapping;
  mdtSqlFieldSetupWidget *pvSourceDatabaseFieldSelectionWidget;
  mdtSqlFieldSetupWidget *pvDestinationDatabaseFieldSelectionWidget;
};

#endif // #ifndef MDT_SQL_COPIER_FIELD_MAPPING_DIALOG_H
