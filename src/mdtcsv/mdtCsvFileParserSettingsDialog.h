/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_CSV_FILE_PARSER_SETTINGS_DIALOG_H
#define MDT_CSV_FILE_PARSER_SETTINGS_DIALOG_H

#include "mdtCsvSettings.h"
#include "ui_mdtCsvFileParserSettingsDialog.h"
#include "mdtCsvFileGeneratorFileSettingsWidget.h"
#include <QDialog>
#include <QString>
#include <QByteArray>

class QWidget;

/*! \brief CSV file generator settings dialog
 */

class mdtCsvFileParserSettingsDialog : public QDialog, Ui::mdtCsvFileParserSettingsDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtCsvFileParserSettingsDialog(QWidget *parent = nullptr);

  /*! \brief Copy disabled
   */
  mdtCsvFileParserSettingsDialog(const mdtCsvFileParserSettingsDialog &) = delete;

  /*! \brief Copy disabled
   */
  mdtCsvFileParserSettingsDialog & operator=(const mdtCsvFileParserSettingsDialog &) = delete;

  /*! \brief Set file settings
   */
  void setFileSettings(const QString & path, const QByteArray & encoding);

  /*! \brief Get file path
   */
  QString filePath() const;

  /*! \brief Get file encoding
   */
  QByteArray fileEncoding() const;

  /*! \brief Set CSV settings
   */
  void setCsvSettings(const mdtCsvParserSettings & settings);

  /*! \brief Get CSV settings
   */
  mdtCsvParserSettings getCsvSettings() const;
};

#endif // #ifndef MDT_CSV_FILE_PARSER_SETTINGS_DIALOG_H
