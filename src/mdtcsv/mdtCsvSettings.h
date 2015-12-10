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
#ifndef MDT_CSV_SETTINGS_H
#define MDT_CSV_SETTINGS_H

#include <QMetaType>
#include <string>

// Set OS native End Of Line sequence
#ifdef Q_OS_WIN
 #define MDT_CSV_NATIVE_EOL "\r\n"
#else
 #define MDT_CSV_NATIVE_EOL "\n"
#endif

/*! \brief Common CSV settings
 *
 * \note Some part of this API documentation
 *       refers to CSV-1203 standard.
 *       CSV-1203 is a open standard available here: http://mastpoint.com/csv-1203
 */
struct mdtCsvCommonSettings
{
  /*! \brief Constructor
   */
  mdtCsvCommonSettings()
   : fieldSeparator(','),
     fieldProtection('\"')
  {
  }

  /*! \brief Field (or column) separator
   *
   * As suggested by CSV-1203 standard, rule 4.3,
   *  the default field separator is comma ","
   */
  char fieldSeparator;

  /*! \brief Field payload protection
   *
   * Field payload protection is explained
   *  in CSV-1203 standard, §9.
   *
   * The default is the double quote '"' .
   *
   * The ability to choose another protection
   *  is a non standard extention.
   */
  char fieldProtection;

  /*! \brief Clear
   *
   * Will reset to default settings.
   */
  void clear()
  {
    fieldSeparator = ',';
    fieldProtection = '\"';
  }
};

/*! \brief CSV parser settings
 */
struct mdtCsvParserSettings : public mdtCsvCommonSettings
{

  /*! \brief Parse Excel protection marker
   *
   * The Excel protection marker (EXP) is explained
   *  in CSV-1203 standard, §10.
   *
   * When parseExp is true,
   *  when a field begins with a ~ (=EXP),
   *  it will not be stored in resulting data.
   *
   * The ability to not parse EXP
   *  is a non standard extention.
   */
  bool parseExp;

  /*! \brief Constructor
   */
  mdtCsvParserSettings()
   : mdtCsvCommonSettings(),
     parseExp(true)
  {
  }

  /*! \brief Clear
   *
   * Will reset to default settings.
   */
  void clear()
  {
    mdtCsvCommonSettings::clear();
    parseExp = true;
  }
};
Q_DECLARE_METATYPE(mdtCsvParserSettings)

/*! \brief CSV generator settings
 */
struct mdtCsvGeneratorSettings : public mdtCsvCommonSettings
{
  /*! \brief End of line
   *
   * The default is native end of line
   *  (\\n on unix, \\r\\n on Windows)
   */
  std::string eol;

  /*! \brief Allways quote text fields
   *
   * Normal way to generate CSV from a text field
   *  is to protect it when needed.
   *  If this flag is set, text fields will always be protected,
   *  witch can produce overhead in generated file/string.
   */
  bool allwaysProtectTextFields;

  /*! \brief Constructor
   */
  mdtCsvGeneratorSettings()
   : mdtCsvCommonSettings(),
     eol(MDT_CSV_NATIVE_EOL),
     allwaysProtectTextFields(false)
  {
  }

  /*! \brief Clear
   *
   * Will reset to default settings.
   */
  void clear()
  {
    mdtCsvCommonSettings::clear();
    eol = MDT_CSV_NATIVE_EOL;
    allwaysProtectTextFields = false;
  }
};
Q_DECLARE_METATYPE(mdtCsvGeneratorSettings)

#endif // #ifndef MDT_CSV_SETTINGS_H
