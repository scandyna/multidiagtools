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
#ifndef MDT_CSV_STRING_GENERATOR_H
#define MDT_CSV_STRING_GENERATOR_H

///#include "mdtError.h"
#include "mdtCsvSettings.h"
#include "mdtCsvData.h"
#include <QString>

/*! \brief String CSV generator
 *
 * \note Some part of this API documentation refers to following standards:
 *       \li CSV-1203 available here: http://mastpoint.com/csv-1203
 *       \li RFC 4180 available here: https://tools.ietf.org/html/rfc4180
 */
class mdtCsvStringGenerator
{
 public:

  /*! \brief Constructor
   *
   * \pre csvSettings must be valid
   */
  mdtCsvStringGenerator(const mdtCsvGeneratorSettings & csvSettings)
   : pvCsvSettings(csvSettings)
  {
    Q_ASSERT(csvSettings.isValid());
  }

  /*! \brief Copy disabled
   */
  mdtCsvStringGenerator(const mdtCsvStringGenerator &) = delete;

  /*! \brief Assignement disabled
   */
  mdtCsvStringGenerator & operator=(const mdtCsvStringGenerator &) = delete;

  /*! \brief Write a CSV line
   */
  QString getCsvString(const mdtCsvRecord & record) const;

  /*! \brief Write all CSV data
   */
  QString getCsvString(const mdtCsvData & data) const;

 private:

  /*! \brief Format field data
   */
  QString formatFieldData(const QVariant & v) const;

  /*! \brief Protect field data
   */
  QString protectedFieldData(const QString & s) const;

  /*! \brief Check if field is a text field
   */
  bool isTextField(const QVariant & v) const
  {
    return (v.type() == QVariant::String);
  }

  mdtCsvGeneratorSettings pvCsvSettings;
};

#endif // #ifndef MDT_CSV_STRING_GENERATOR_H
