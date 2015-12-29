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
#ifndef MDT_CSV_FILE_SETTINGS_WIDGET_H
#define MDT_CSV_FILE_SETTINGS_WIDGET_H

#include "mdtError.h"
#include "mdtCsvSettings.h"
#include "ui_mdtCsvFileSettingsWidget.h"
#include <QWidget>
#include <QString>
#include <QByteArray>

/*! \brief Widget to let the user select target file and encoding
 */
class mdtCsvFileSettingsWidget : public QWidget, Ui::mdtCsvFileSettingsWidget
{
 Q_OBJECT

 public:

  /*! \brief Mode of file selection
   */
  enum SelectFileMode
  {
    SelectOpen,   /*!< Open a existing file */
    SelectSave    /*!< Save a file */
  };

  /*! \brief Constructor
   *
   * Will also set SelectFileMode to SelectOpen.
   */
  mdtCsvFileSettingsWidget(QWidget *parent = nullptr);

  mdtCsvFileSettingsWidget(const mdtCsvFileSettingsWidget &) = delete;
  mdtCsvFileSettingsWidget & operator=(const mdtCsvFileSettingsWidget &) = delete;

  /*! \brief Set file settings
   */
  void setFileSettings(const QString & path, const QByteArray & encoding);

  /*! \brief Get file path
   */
  QString filePath() const;

  /*! \brief Get file encoding
   */
  QByteArray fileEncoding() const;

  /*! \brief Set file selection mode
   */
  void setSelectFileMode(SelectFileMode mode);

 private:

  /*! \brief Select target file
   */
  void selectFile();

  SelectFileMode pvSelectFileMode;
  QString pvDefaultDirectory;
};

#endif // #ifndef MDT_CSV_FILE_SETTINGS_WIDGET_H
