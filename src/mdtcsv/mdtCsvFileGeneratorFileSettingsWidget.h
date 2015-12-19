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
#ifndef MDT_CSV_FILE_GENERATOR_FILE_SETTINGS_WIDGET_H
#define MDT_CSV_FILE_GENERATOR_FILE_SETTINGS_WIDGET_H

#include "mdtError.h"
#include "mdtCsvSettings.h"
#include "ui_mdtCsvFileGeneratorSettingsDialog.h"
#include "ui_mdtCsvFileGeneratorFileSettingsWidget.h"
#include <QWidget>
#include <QString>
#include <QByteArray>

/*! \brief Widget to let the user select target file and encoding
 */
class mdtCsvFileGeneratorFileSettingsWidget : public QWidget, Ui::mdtCsvFileGeneratorFileSettingsWidget
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtCsvFileGeneratorFileSettingsWidget(QWidget *parent = nullptr);

  mdtCsvFileGeneratorFileSettingsWidget(const mdtCsvFileGeneratorFileSettingsWidget &) = delete;
  mdtCsvFileGeneratorFileSettingsWidget & operator=(const mdtCsvFileGeneratorFileSettingsWidget &) = delete;

  /*! \brief Set file settings
   */
  void setFileSettings(const QString & path, const QByteArray & encoding);

  /*! \brief Get file path
   */
  QString filePath() const;

  /*! \brief Get file encoding
   */
  QByteArray fileEncoding() const;

 private:

  /*! \brief Select target file
   */
  void selectFile();

  QString pvDefaultDirectory;
};

#endif // #ifndef MDT_CSV_FILE_GENERATOR_FILE_SETTINGS_WIDGET_H
