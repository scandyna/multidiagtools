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
#ifndef MDT_CSV_PARSER_SETTINGS_WIDGET_H
#define MDT_CSV_PARSER_SETTINGS_WIDGET_H

#include "mdtCsvSettings.h"
#include "mdtCsvSettingsWidget.h"
#include <QWidget>
#include <string>

class QComboBox;

/*! \brief CSV parser settings widget
 */
class mdtCsvParserSettingsWidget : public mdtCsvSettingsWidget
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtCsvParserSettingsWidget(QWidget *parent = nullptr);

  /*! \brief Copy disabled
   */
  mdtCsvParserSettingsWidget(const mdtCsvParserSettingsWidget &) = delete;

  /*! \brief Copy disabled
   */
  mdtCsvParserSettingsWidget & operator=(const mdtCsvParserSettingsWidget &) = delete;

  /*! \brief Set settings
   */
  void setSettings(const mdtCsvParserSettings & settings);

  /*! \brief Get settings
   */
  mdtCsvParserSettings getSettings() const;

 signals:

  /*! \brief Emitted whenever CSV settinsg changed
   */
  void csvSettingsChanged(const mdtCsvParserSettings & settings);

 private slots:
  
  /*! \brief Called each time the user choose a field separator, protections, ...
   */
  void onSettingsSelectionChanged();
};

#endif // #ifndef MDT_CSV_PARSER_SETTINGS_WIDGET_H

