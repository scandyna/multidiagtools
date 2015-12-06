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
#ifndef MDT_CSV_FILE_PARSER_H
#define MDT_CSV_FILE_PARSER_H

#include "mdtCsvSettings.h"
#include "mdtCsvData.h"
#include "mdtCsvFileParserIterator.h"
#include "mdtError.h"
#include <QString>
#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <memory>

template <typename InputIterator>
class mdtCsvParserTemplate;

/*! \brief CSV parser that acts on a file as input
 */
class mdtCsvFileParser
{
 public:

  /*! \brief Default constructor
   */
  mdtCsvFileParser(const mdtCsvParserSettings & csvSettings = mdtCsvParserSettings());

  /*! \brief Destructor
   */
//   ~mdtCsvFileParser();

  /*! \internal Copy disabled
   */
  mdtCsvFileParser(const mdtCsvFileParser &) = delete;

  /*! \brief Open CSV file
   *
   * \param fileInfo Path to CSV file that must be open
   * \param encoding Encoding name of the CSV file format.
   *               Will use QTextCodec::codecForName() to get apropriate codec.
   * \return false if CSV file could not be open, or no codec could be found for given encoding,
   *         true if all goes well.
   */
  bool openFile(const QFileInfo & fileInfo, const QByteArray & encoding);

  /*! \brief Close CSV file
   */
  void closeFile();

  /*! \brief Check about end of file
   */
  bool atEnd() const;

  /*! \brief Read one line in CSV file
   */
  mdtCsvRecord readLine();

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  /*! \brief Translate (calls QObject::tr() )
   */
  QString tr(const char *sourceText);

  QFile pvFile;
  mdtCsvFileParserIterator pvFileIterator;
  std::unique_ptr<mdtCsvParserTemplate<mdtCsvFileParserMultiPassIterator> > pvParser;
  mdtError pvLastError;
};

#endif // #ifndef MDT_CSV_FILE_PARSER_H
