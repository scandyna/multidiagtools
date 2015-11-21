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

#include <string>

/*! \brief CSV parser settings
 *
 * \note Some part of this API documentation
 *       refers to CSV-1203 standard.
 *       CSV-1203 is a open standard available here: http://mastpoint.com/csv-1203
 */
struct mdtCsvParserSettings
{
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
   *  The ability to choose another protection
   *  is a non standard extention.
   */
  char fieldProtection;

  /*! \brief Constructor
   */
  mdtCsvParserSettings()
   : fieldSeparator(','),
     fieldProtection('\"')
  {
  }

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

#endif // #ifndef MDT_CSV_SETTINGS_H
