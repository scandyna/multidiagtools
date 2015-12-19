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
#ifndef MDT_CSV_GENERATOR_SETTINGS_WIDGET_H
#define MDT_CSV_GENERATOR_SETTINGS_WIDGET_H

#include "mdtCsvSettings.h"
#include "ui_mdtCsvGeneratorSettingsWidget.h"
#include <QWidget>
#include <string>

class QComboBox;

/*! \brief CSV generator settings widget
 */
class mdtCsvGeneratorSettingsWidget : public QWidget, Ui::mdtCsvGeneratorSettingsWidget
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtCsvGeneratorSettingsWidget(QWidget *parent = nullptr);

  /*! \brief Copy disabled
   */
  mdtCsvGeneratorSettingsWidget(const mdtCsvGeneratorSettingsWidget &) = delete;

  /*! \brief Copy disabled
   */
  mdtCsvGeneratorSettingsWidget & operator=(const mdtCsvGeneratorSettingsWidget &) = delete;

  /*! \brief Set settings
   */
  void setSettings(const mdtCsvGeneratorSettings & settings);

  /*! \brief Get settings
   */
  mdtCsvGeneratorSettings getSettings() const;

 private:

  /*! \brief Find a item data for current text of given combobox
   */
  QVariant findItemDataForItemText(QComboBox *cb) const;

  /*! \brief Extract char in var
   */
  char getCharFromVariant(const QVariant & var) const;

  /*! \brief Select field seperator
   */
  void selectFieldSeparator(char sep);

  /*! \brief Get selected field separator
   */
  char getSelectedFieldSeparator() const;

  /*! \brief Select field protection
   */
  void selectFieldProtection(char protection);

  /*! \brief Get selected field protection
   */
  char getSelectedFieldProtection() const;

  /*! \brief Select eol
   */
  void selectEol(const std::string & eol);

  /*! \brief Get selected eol
   */
  std::string getSelectedEol() const;
};

#endif // #ifndef MDT_CSV_GENERATOR_SETTINGS_WIDGET_H
