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
#ifndef MDT_CSV_SETTINGS_WIDGET_H
#define MDT_CSV_SETTINGS_WIDGET_H

#include "mdtCsvSettings.h"
#include "ui_mdtCsvSettingsWidget.h"
#include <QWidget>
#include <string>

class QComboBox;

/*! \brief Common base for CSV settings widget
 */
class mdtCsvSettingsWidget : public QWidget, Ui::mdtCsvSettingsWidget
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtCsvSettingsWidget(QWidget *parent = nullptr);

  /*! \brief Copy disabled
   */
  mdtCsvSettingsWidget(const mdtCsvSettingsWidget &) = delete;

  /*! \brief Copy disabled
   */
  mdtCsvSettingsWidget & operator=(const mdtCsvSettingsWidget &) = delete;

 signals:

  /*! \brief Emitted whenever field protection changed
   */
  void fieldProtectionChanged();

  /*! \brief Emitted whenever field separator changed
   */
  void fieldSeparatorChanged();

  /*! \brief Emitted whenever parse EXP state changed
   */
  void parseExpStateChanged();

 protected:

  /*! \brief Set cbParseExp visibility
   */
  void setParseExpVisible(bool visible);

  /*! \brief Set EOL combobox visibility
   */
  void setEolVisible(bool visible);

  /*! \brief Set allways protect text fields visibility
   */
  void setAllwaysProtectTextFieldsVisible(bool visible);

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

  /*! \brief Set allways protect text fields (generator specific)
   */
  void setAllwaysProtectTextFields(bool set);

  /*! \brief Get allways protect text fields (generator specific)
   */
  bool allwaysProtectTextFields() const;

  /*! \brief Set parse EXP (parser specific)
   */
  void setParseExp(bool set);

  /*! \brief Get parse EXP (parser specific)
   */
  bool parseExp() const;

 private:

  /*! \brief Find a item data for current text of given combobox
   */
  QVariant findItemDataForItemText(QComboBox *cb) const;

  /*! \brief Extract char in var
   */
  char getCharFromVariant(const QVariant & var) const;
};

#endif // #ifndef MDT_CSV_SETTINGS_WIDGET_H
